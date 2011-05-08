// 
// $Id$
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

#include "ALPlanCommand.hh"
#include <Keystone/KSLog.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/CFStringAdditions.hh>

#include <AssemblerCore/ALOptionsModel.hh>
#include <AssemblerCore/ALDirectorySourceCollection.hh>
#include <AssemblerCore/ALSourceFileFilter.hh>
#include <AssemblerCore/ALExpressionFileFilter.hh>

#include <unistd.h>

/**
 * Execute this command
 * 
 * @param options global options
 * @param argc argument count
 * @param argv arguments
 * @return result
 */
int ALPlanCommand::__execute(int argc, char * const argv[]) {
  
  if(gSharedOptions->isVerbose()){
    KSLog("Project Plan:");
  }
  
  char *cwd = getcwd(NULL, 0);
  CFStringRef from = CFStringCreateWithCString(NULL, cwd, kDefaultStringEncoding);
  CFStringRef sbase = gSharedOptions->getProjectBasePath();
  CFStringRef obase = gSharedOptions->getOutputBasePath();
  
  fprintf(stderr, "BASE: %s\n", CFStringCopyCString(sbase, kDefaultStringEncoding));
  
  processPath(sbase, sbase, obase);
  
  if(cwd) free(cwd);
  if(from) CFRelease(from);
  
  return 0;
}

/**
 * Process a path
 * 
 * @param path the path
 * @param bpath source base path
 * @param opath output base path
 */
KSStatus ALPlanCommand::processPath(CFStringRef path, CFStringRef bpath, CFStringRef opath) {
  KSStatus status = KSStatusOk;
  ALDirectorySourceCollection *collection = NULL;
  KSFileFilter *filter = NULL;
  CFArrayRef excluded = NULL;
  CFStringRef pattern = NULL;
  CFStringRef vpath = NULL;
  CFStringRef dpath = NULL;
  CFStringRef cwdpath = NULL;
  
  excluded = CFArrayCreate(NULL, (const void **)&opath, 1, &kCFTypeArrayCallBacks);
  
  if((pattern = gSharedOptions->getProcessFilePattern()) != NULL){
    filter = new ALExpressionFileFilter(pattern, excluded, NULL, gSharedOptions->getIncludeHiddenFiles());
  }else{
    filter = new ALSourceFileFilter(gSharedOptions->getProcessFileTypes(), excluded, NULL, gSharedOptions->getIncludeHiddenFiles());
  }
  
  collection = new ALDirectorySourceCollection(gSharedOptions, path, true, filter);
  cwdpath = KSFile::copyCurrentWorkingDirectory();
  
  while((vpath = collection->copyNextSourcePath()) != NULL){
    dpath = CFStringCreateByRelocatingPath(vpath, opath, false);
    CFStringRef rdpath = CFStringCreateRelativePath(cwdpath, dpath);
    CFStringRef rvpath = CFStringCreateRelativePath(cwdpath, vpath);
    KSLog("+ %@ => %@", rvpath, rdpath);
    if(rvpath) CFRelease(rvpath);
    if(rdpath) CFRelease(rdpath);
  }
  
error:
  if(collection) KSRelease(collection);
  if(filter) KSRelease(filter);
  if(excluded) CFRelease(excluded);
  if(cwdpath) CFRelease(cwdpath);
  if(vpath) CFRelease(vpath);
  if(dpath) CFRelease(dpath);
  
  return status;
}

