// 
// $Id: ALUtility.cc 162 2011-05-08 22:01:47Z brian $
// Web Assembler
// 
// Copyright (c) 2009 Wolter Group New York, Inc., All rights reserved.
// 
// This software ("WGNY Software") is supplied to you by Wolter Group New York
// ("WGNY") in consideration of your acceptance of the terms of the Agreement
// under which the software was licensed to you. Your use or installation of
// this Software constitutes acceptance of these terms.  If you do not agree
// with these terms, do not use or install this software.
// 
// The WGNY Software is provided by WGNY on an "AS IS" basis.  WGNY MAKES NO
// WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE IMPLIED
// WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE, REGARDING THE WGNY SOFTWARE OR ITS USE AND OPERATION
// ALONE OR IN COMBINATION WITH YOUR PRODUCTS.
// 
// IN NO EVENT SHALL WGNY BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) ARISING IN ANY WAY OUT OF THE USE, REPRODUCTION, MODIFICATION
// AND/OR DISTRIBUTION OF THE WGNY SOFTWARE, HOWEVER CAUSED AND WHETHER UNDER
// THEORY OF CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY OR
// OTHERWISE, EVEN IF WGNY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
// Designed and Engineered by Wolter Group in New York City
// 

#include "ALUtility.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSStatus.hh>
#include <Keystone/KSUtility.hh>
#include <Keystone/KSOptionParser.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSFileOutputStream.hh>
#include <Keystone/CFStringAdditions.hh>

#include "ALHTMLSourceFilter.hh"
#include "ALDirectorySourceCollection.hh"
#include "ALSourceFileFilter.hh"
#include "ALExpressionFileFilter.hh"
#include "ALSourceFilterContext.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

/**
 * Process a path recursively
 * 
 * @param path the path
 * @param bpath source base path
 * @param opath output base path
 * @return status
 */
KSStatus ALAssemblerProcessPath(const ALOptionsModel *options, CFStringRef path, CFStringRef bpath, CFStringRef opath, KSError **error) {
  KSStatus status = KSStatusOk;
  
  ALDirectorySourceCollection *collection = NULL;
  ALDependencyGraph *graph = NULL;
  KSFileFilter *filter = NULL;
  CFArrayRef excluded = NULL;
  CFStringRef pattern = NULL;
  CFStringRef vpath = NULL;
  CFStringRef dpath = NULL;
  
  excluded = CFArrayCreate(NULL, (const void **)&opath, 1, &kCFTypeArrayCallBacks);
  
  if((pattern = options->getProcessFilePattern()) != NULL){
    filter = new ALExpressionFileFilter(pattern, excluded, options->getExcludedFilePrefixes(), options->getIncludeHiddenFiles());
  }else{
    filter = new ALSourceFileFilter(options->getProcessFileTypes(), excluded, options->getExcludedFilePrefixes(), options->getIncludeHiddenFiles());
  }
  
  collection = new ALDirectorySourceCollection(options, path, true, filter);
  
  if(!options->isVerbose() && !options->isQuiet()){
    fputs("Doing it: ", stderr);
  }
  
  while((vpath = collection->copyNextSourcePath()) != NULL){
    dpath = CFStringCreateByRelocatingPath(vpath, opath, false);
    graph = new ALDependencyGraph(vpath);
    
    if(graph->init(NULL) != KSStatusOk){
      if(options->isDebugging()){
        KSLog("Warning: unable to load dependency cache for: %@; processing anyway", vpath);
      }
    }
    
    bool outOfDate = false;
    if(!options->getForce()){
      if((status = ALAssemblerIsFileOutOfDate(gSharedOptions, vpath, dpath, &outOfDate, 0, error)) != KSStatusOk){
        goto error;
      }
    }
    
    if(outOfDate || options->getForce()){
      if(!options->isVerbose() && !options->isQuiet()) fputc('+', stderr);
      if((status = ALAssemblerProcessFile(options, vpath, dpath, bpath, opath, graph, error)) != KSStatusOk) goto error;
    }else{
      if(!options->isVerbose() && !options->isQuiet()) fputc('.', stderr);
    }
    
    if(graph->write(NULL) != KSStatusOk){
      KSLog("Warning: unable to write dependency cache for: %@", vpath);
    }
    
    CFReleaseSafe(dpath);
    CFReleaseSafe(vpath);
    KSRelease(graph);
  }
  
  if(!options->isVerbose() && !options->isQuiet()){
    fputs(" Done!\n", stderr);
  }
  
error:
  KSRelease(collection);
  KSRelease(filter);
  CFReleaseSafe(excluded);
  CFReleaseSafe(vpath);
  CFReleaseSafe(dpath);
  
  return status;
}

/**
 * Process a file
 * 
 * @param path file path
 * @param dpath destination path
 * @param bpath source base path
 * @param opath output base path
 */
KSStatus ALAssemblerProcessFile(const ALOptionsModel *options, CFStringRef path, CFStringRef dpath, CFStringRef bpath, CFStringRef opath, ALDependencyGraph *graph, KSError **error) {
  KSStatus status = KSStatusOk;
  KSFile *dest = NULL;
  KSFile *source = NULL;
  KSInputStream *ins = NULL;
  KSOutputStream *outs = NULL;
  
  if(options->isVerbose()){
    CFStringRef cwdpath = KSFile::copyCurrentWorkingDirectory();
    CFStringRef rpath = CFStringCreateRelativePath(cwdpath, path);
    KSLog(" + %@", rpath);
    if(rpath) CFRelease(rpath);
    if(cwdpath) CFRelease(cwdpath);
  }
  
  dest = new KSFile(dpath);
  
  if((status = dest->makeDirectories()) != KSStatusOk){
    if(error) *error = new KSError(status, "Unable to create path to: %@: %@", dpath, KSStatusGetMessage(status));
    goto error;
  }
  
  if((outs = dest->createOutputStream(&status)) == NULL){
    if(error) *error = new KSError(status, "Unable to open output stream for: %@: %@", dpath, KSStatusGetMessage(status));
    goto error;
  }
  
  source = new KSFile(path);
  
  if((ins = source->createInputStream(&status)) == NULL){
    if(error) *error = new KSError(status, "Unable to open input stream for: %@: %@", path, KSStatusGetMessage(status));
    goto error;
  }
  
  if((status = ALAssemblerProcessStream(options, ins, outs, path, dpath, bpath, opath, graph, error)) != KSStatusOk){
    goto error;
  }
  
error:
  KSRelease(dest);
  KSRelease(source);
  
  if(ins){
    ins->close();
    KSRelease(ins);
  }
  
  if(outs){
    outs->close();
    KSRelease(outs);
  }
  
  return status;
}

/**
 * Process a stream
 * 
 * @param istream input stream
 * @param ostream output stream
 * @param spath source path
 * @param dpath destination path
 * @param bpath source base path
 * @param opath output base path
 * @return status
 */
KSStatus ALAssemblerProcessStream(const ALOptionsModel *options, KSInputStream *istream, KSOutputStream *ostream, CFStringRef spath, CFStringRef dpath, CFStringRef bpath, CFStringRef opath, ALDependencyGraph *graph, KSError **error) {
  KSStatus status = KSStatusOk;
  ALSourceFilter *filter = NULL;
  
  const void * keys[] = { kALSourceFilterSourcePathKey, kALSourceFilterDestinationPathKey, kALSourceFilterSourceBasePathKey, kALSourceFilterDestinationBasePathKey };
  const void * vals[] = { spath, dpath, bpath, opath };
  
  CFDictionaryRef paths = CFDictionaryCreate(NULL, (const void **)keys, (const void **)vals, 4, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  ALSourceFilterContext *context = new ALSourceFilterContext(paths, graph);
  
  filter = new ALHTMLSourceFilter(options, NULL, NULL, context);
  
  if((status = filter->filter(istream, ostream)) != KSStatusOk){
    if(error) *error = new KSError(status, "An error occured while assembling: %@", KSStatusGetMessage(status));
    goto error;
  }
  
error:
  CFReleaseSafe(paths);
  KSRelease(context);
  KSRelease(filter);
  
  return status;
}

/**
 * Determine if an output file is out-of-date relative to its source counterpart.
 * An output file is considered to be out-of-date if it's modification time is older
 * than it's source counterpart's modification time.
 * 
 * @param options option model
 * @param spath source path
 * @param opath output path
 * @param outOfDate on output if not NULL: whether or not the output file is out-of-date or not
 * @param depth the recursion depth of the call
 * @param error on output if not NULL and the status is not KSStatusOk: an error describing the problem
 * @return status
 */
KSStatus ALAssemblerIsFileOutOfDate(const ALOptionsModel *options, CFStringRef spath, CFStringRef opath, bool *outOfDate, CFIndex depth, KSError **error) {
  KSStatus status = KSStatusOk;
  ALDependencyGraph *dGraph = NULL;
  CFStringRef dGraphPath = NULL;
  CFSetRef dependencies = NULL;
  void **dPaths = NULL;
  char *cpath = NULL;
  struct stat ssb, osb;
  bool oodate = false;
  
  if(spath == NULL){
    status = KSStatusInvalidParameter;
    if(error) *error = new KSError(KSStatusInvalidParameter, "Source path is null");
    goto error;
  }
  
  if(opath == NULL){
    status = KSStatusInvalidParameter;
    if(error) *error = new KSError(KSStatusInvalidParameter, "Output path is null");
    goto error;
  }
  
  if((cpath = CFStringCopyCString(spath, kDefaultStringEncoding)) == NULL){
    status = KSStatusInvalidParameter;
    if(error) *error = new KSError(KSStatusInvalidParameter, "Unable to convert source path to UTF-8");
    goto error;
  }
  
  if(stat(cpath, &ssb) < 0){
    // if the stat error indicates the source file does not exist, this is always a "real" error
    status = KSStatusInvalidParameter;
    if(error) *error = new KSError(KSStatusInvalidParameter, "Unable to stat file: %@", spath);
    goto error;
  }
  
  KSFreeSafe(cpath);
  
  if((cpath = CFStringCopyCString(opath, kDefaultStringEncoding)) == NULL){
    status = KSStatusInvalidParameter;
    if(error) *error = new KSError(KSStatusInvalidParameter, "Unable to convert source path to UTF-8");
    goto error;
  }
  
  if(stat(cpath, &osb) < 0){
    // if the stat error indicates the output file does not exist, this is interpreted
    // as the output file being out of date; otherwise a "real" error is produced
    if(errno == ENOENT){
      oodate = true;
      goto done;
    }else{
      status = KSStatusInvalidParameter;
      if(error) *error = new KSError(KSStatusInvalidParameter, "Unable to stat file: %@", opath);
      goto error;
    }
  }
  
  // determine if the file itself is out of date; if so we're done here
  if(ssb.st_mtimespec.tv_sec > osb.st_mtimespec.tv_sec){
    oodate = true;
  }else if(ssb.st_mtimespec.tv_sec == osb.st_mtimespec.tv_sec){
    oodate = ssb.st_mtimespec.tv_nsec > osb.st_mtimespec.tv_nsec;
  }
  
  // otherwise, we have to check the dependency graph for any out-of-date dependencies
  if(oodate) goto done;
  
  if((dGraphPath = ALDependencyGraph::copyStandardDependencyCachePath(spath)) == NULL){
    status = KSStatusError;
    if(error) *error = new KSError(KSStatusInvalidParameter, "Unable to copy dependency graph path for source path: %@", spath);
    goto error;
  }
  
  dGraph = new ALDependencyGraph(spath);
  
  if((status = dGraph->initWithContentsOfFile(dGraphPath, error)) != KSStatusOk){
    goto error;
  }
  
  if((dependencies = dGraph->getDirectDependencies()) == NULL){
    goto done; // this is fine; just use false
  }
  
  if((dPaths = (void **)malloc(sizeof(void *) * CFSetGetCount(dependencies))) == NULL){
    status = KSStatusError;
    if(error) *error = new KSError(KSStatusInvalidParameter, "Unable to allocate for dependency set");
    goto error;
  }
  
  CFSetGetValues(dependencies, (const void **)dPaths);
  
  for(int i = 0; i < CFSetGetCount(dependencies); i++){
    CFStringRef dpath = (CFStringRef)dPaths[i];
    if((status = ALAssemblerIsFileOutOfDate(options, dpath, opath, &oodate, depth + 1, error)) == KSStatusOk){
      if(oodate) goto done;
    }else{
      goto error;
    }
  }
  
  // only display top level paths for verbose output
  if(options->isVerbose() && depth < 1){
    for(int i = 0; i < depth; i++) fputs("  ", stderr);
    KSLog(" ~ %@", spath);
  }
  
done:
error:
  if(outOfDate) *outOfDate = oodate;
  CFReleaseSafe(dGraphPath);
  KSRelease(dGraph);
  KSFreeSafe(dPaths);
  KSFreeSafe(cpath);
  
  return status;
}

