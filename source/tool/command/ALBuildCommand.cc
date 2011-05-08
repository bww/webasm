// 
// $Id: ALBuildCommand.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALBuildCommand.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSStatus.hh>
#include <Keystone/KSUtility.hh>
#include <Keystone/KSOptionParser.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSFileOutputStream.hh>
#include <Keystone/CFStringAdditions.hh>

#include <AssemblerCore/ALOptionsModel.hh>
#include <AssemblerCore/ALUtility.hh>
#include <AssemblerCore/ALHTMLSourceFilter.hh>
#include <AssemblerCore/ALDirectorySourceCollection.hh>
#include <AssemblerCore/ALSourceFileFilter.hh>
#include <AssemblerCore/ALExpressionFileFilter.hh>

#include "ALHelpCommand.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <getopt.h>

typedef enum ALBuildMode_ {
  ALBuildModeProject,
  ALBuildModeFile
} ALBuildMode;

typedef enum ALBuildFlagsMask_ {
  ALBuildFlagNone           = 0,
  ALBuildFlagHTTPHeaders    = 1 << 0,
} ALBuildFlagsMask;

typedef UInt32 ALBuildFlags;

/**
 * Execute this command
 * 
 * @param options global options
 * @param argc argument count
 * @param argv arguments
 * @return result
 */
int ALBuildCommand::__execute(int argc, char * const argv[]) {
  KSStatus status = KSStatusOk;
  
  ALBuildMode mode = ALBuildModeProject;
  ALBuildFlags flags = ALBuildFlagNone;
  CFStringRef outpath = NULL;
  CFStringRef inpath = NULL;
  KSError *error;
  int acount = 0;
  
  static KSOptionParserOption ksoptions[] = {
    { "output",           'outp',   kKSOptionArgumentRequired,  NULL },
    { "http",             'http',   kKSOptionNoArgument,        NULL },
    NULL
  };
  
  KSOptionParser *parser = new KSOptionParser(ksoptions, argc, argv);
  KSFourCharCode fid;
  
  while((status = parser->getNextOption(&fid, &error)) == KSStatusOk && fid != kKSOptionParserEndOptions){
    switch(fid){
      
      case 'outp':
        outpath = CFStringCreateWithCString(NULL, parser->getArgument(), kDefaultStringEncoding);
        break;
        
      case 'http':
        flags |= ALBuildFlagHTTPHeaders;
        break;
        
    }
  }
  
  if(status != KSStatusOk){
    KSLogK("\nUh oh! Something went wrong:\n  build: %K", error->getMessage());
    fputs("\nHere's some more information on building projects with Web Assembler:\n\n", stderr);
    KSRelease(error);
    ALHelpCommand::displaySection(kALHelpSectionBuild);
    return -1;
  }
  
  acount = parser->getIndex();
  argc -= acount;
  argv += acount;
  
  KSRelease(parser);
  
  if(argc > 0){
    inpath = CFStringCreateWithCString(NULL, argv[0], kDefaultStringEncoding);
    mode = ALBuildModeFile;
  }
  
  char *ccwd = getcwd(NULL, 0);
  CFStringRef from  = CFStringCreateWithCString(NULL, ccwd, kDefaultStringEncoding);
  
  CFStringRef sbase = gSharedOptions->getProjectBasePath();
  CFStringRef obase = gSharedOptions->getOutputBasePath();
  
  if(mode == ALBuildModeProject){
    if(gSharedOptions->isVerbose()) KSLog("Building Project...");
    
    if((status = ALAssemblerProcessPath(gSharedOptions, sbase, sbase, obase, &error)) != KSStatusOk){
      if(error) KSLogError("Error: %@", KSUnwrap(error->getMessage()));
      KSRelease(error);
    }
    
  }else if(mode == ALBuildModeFile){
    if(gSharedOptions->isVerbose()) KSLog("Building File...");
    CFStringRef vpath = CFStringCreateCanonicalPath(from, inpath);
    
    if(outpath != NULL){
      
      if((status = ALAssemblerProcessFile(gSharedOptions, vpath, outpath, sbase, obase, NULL, &error)) != KSStatusOk){
        if(error) KSLogError("Error: %@", KSUnwrap(error->getMessage()));
        KSRelease(error);
      }
      
    }else{
      CFStringRef dpath = CFStringCreateByRelocatingPath(vpath, obase, false);
      KSOutputStream *ostream = new KSFileOutputStream(fileno(stdout));
      
      if((status = processFile(vpath, ostream, dpath, sbase, obase, &error)) != KSStatusOk){
        if(error) KSLogError("Error: %@", KSUnwrap(error->getMessage()));
        KSRelease(error);
      }
      
      KSRelease(ostream);
      CFReleaseSafe(dpath);
    }
    
    CFReleaseSafe(vpath);
  }else{
    KSLogError("Unknown build mode: %d", mode);
  }
  
error:
  if(from)    CFRelease(from);
  if(ccwd)    free(ccwd);
  if(outpath) CFRelease(outpath);
  if(inpath)  CFRelease(inpath);
  
  return status;
}

/**
 * Process a file to a stream
 * 
 * @param path input file path
 * @param ostream output stream
 * @param dpath destination path
 * @param bpath source base path
 * @param opath output base path
 * @return status
 */
KSStatus ALBuildCommand::processFile(CFStringRef path, KSOutputStream *ostream, CFStringRef dpath, CFStringRef bpath, CFStringRef opath, KSError **error) {
  KSStatus status = KSStatusOk;
  KSFile *source = NULL;
  KSInputStream *istream = NULL;
  
  if(gSharedOptions->isVerbose()){
    CFStringRef cwdpath = KSFile::copyCurrentWorkingDirectory();
    CFStringRef rpath = CFStringCreateRelativePath(cwdpath, path);
    KSLog(" %@", rpath);
    if(rpath) CFRelease(rpath);
    if(cwdpath) CFRelease(cwdpath);
  }
  
  source = new KSFile(path);
  
  if((istream = source->createInputStream(&status)) == NULL){
    if(error) *error = new KSError(status, "Unable to open input stream for: %@: %@", path, KSStatusGetMessage(status));
    goto error;
  }
  
  if((status = ALAssemblerProcessStream(gSharedOptions, istream, ostream, path, dpath, bpath, opath, NULL, error)) != KSStatusOk){
    goto error;
  }
  
error:
  KSRelease(source);
  
  if(istream){
    istream->close();
    KSRelease(istream);
  }
  
  return status;
}

