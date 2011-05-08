// 
// $Id: ALParseCommand.cc 161 2010-12-06 23:21:36Z brian $
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

#include "ALParseCommand.hh"

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
int ALParseCommand::__execute(int argc, char * const argv[]) {
  
  if(gSharedOptions->isVerbose()){
    KSLog("Parse Tree:");
  }
  
  char *cwd = getcwd(NULL, 0);
  CFStringRef from = CFStringCreateWithCString(NULL, cwd, kDefaultStringEncoding);
  CFStringRef sbase = gSharedOptions->getProjectBasePath();
  CFStringRef obase = gSharedOptions->getOutputBasePath();
  
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
 * @param status
 */
KSStatus ALParseCommand::processPath(CFStringRef path, CFStringRef bpath, CFStringRef opath) {
  KSStatus status = KSStatusOk;
  ALDirectorySourceCollection *collection = NULL;
  KSFileFilter *filter = NULL;
  CFArrayRef excluded = NULL;
  CFStringRef pattern = NULL;
  CFStringRef vpath = NULL;
  CFStringRef dpath = NULL;
  
  excluded = CFArrayCreate(NULL, (const void **)&opath, 1, &kCFTypeArrayCallBacks);
  
  if((pattern = gSharedOptions->getProcessFilePattern()) != NULL){
    filter = new ALExpressionFileFilter(pattern, excluded, NULL, gSharedOptions->getIncludeHiddenFiles());
  }else{
    filter = new ALSourceFileFilter(gSharedOptions->getProcessFileTypes(), excluded, NULL, gSharedOptions->getIncludeHiddenFiles());
  }
  
  collection = new ALDirectorySourceCollection(gSharedOptions, path, true, filter);
  
  while((vpath = collection->copyNextSourcePath()) != NULL){
    dpath = CFStringCreateByRelocatingPath(vpath, opath, false);
    if((status = processFile(vpath, dpath, bpath)) != KSStatusOk){
      goto error;
    }
  }
  
error:
  if(collection) KSRelease(collection);
  if(filter) KSRelease(filter);
  if(excluded) CFRelease(excluded);
  if(vpath) CFRelease(vpath);
  if(dpath) CFRelease(dpath);
  
  return status;
}

/**
 * Process a file
 * 
 * @param path file path
 * @param dpath destination path
 * @param bpath base path
 */
KSStatus ALParseCommand::processFile(CFStringRef path, CFStringRef dpath, CFStringRef bpath) {
  KSStatus status = KSStatusOk;
  KSStatus vstatus;
  KSFile *source = NULL;
  KSInputStream *ins = NULL;
  ALParser *parser = NULL;
  KSError *error = NULL;
  int *depth = NULL;
  
  if(gSharedOptions->isVerbose()){
    KSLog("+ %@", path);
  }
  
  source = new KSFile(path);
  
  if((ins = source->createInputStream(&vstatus)) == NULL){
    KSLogError("Unable to open input stream for: %@: %@", path, KSStatusGetMessage(vstatus));
    status = vstatus;
    goto error;
  }
  
  parser = new ALWAMLParser(gSharedOptions, this);
  
  depth = (int *)malloc(sizeof(int));
  *depth = 0;
  
  if((vstatus = parser->parse(ins, &error, depth)) != KSStatusOk){
    KSLogError("Unable to parse: %@: %@", path, KSUnwrap(error->getMessage()));
    status = vstatus;
    goto error;
  }
  
error:
  KSRelease(parser);
  KSRelease(source);
  KSRelease(error);
  
  if(depth) free(depth);
  
  if(ins){
    ins->close();
    KSRelease(ins);
  }
  
  return status;
}

KSStatus ALParseCommand::openElement(const ALParser *parser, CFStringRef name, void *info, KSError **error) {
  int depth = *((int *)info);
  
  register int i;
  for(i = 0; i < depth; i++) fputs("  ", stdout);
  
  CFStringRef display = CFStringCreateWithFormat(NULL, 0, CFSTR("<%@>\n"), name);
  char *cdisplay = CFStringCopyCString(display, kDefaultStringEncoding);
  
  fputs(cdisplay, stdout);
  
  if(display) CFRelease(display);
  if(cdisplay) free(cdisplay);
  
  *((int *)info) = ++depth;
  
  return KSStatusOk;
}

KSStatus ALParseCommand::closeElement(const ALParser *parser, CFStringRef name, void *info, KSError **error) {
  int depth = *((int *)info) - 1;
  
  register int i;
  for(i = 0; i < depth; i++) fputs("  ", stdout);
  
  CFStringRef display = CFStringCreateWithFormat(NULL, 0, CFSTR("</%@>\n"), name);
  char *cdisplay = CFStringCopyCString(display, kDefaultStringEncoding);
  
  fputs(cdisplay, stdout);
  
  if(display) CFRelease(display);
  if(cdisplay) free(cdisplay);
  
  *((int *)info) = depth;
  
  return KSStatusOk;
}

KSStatus ALParseCommand::addElementAttribute(const ALParser *parser, CFStringRef name, CFStringRef value, void *info, KSError **error) {
  int depth = *((int *)info);
  
  register int i;
  for(i = 0; i < depth; i++) fputs("  ", stdout);
  
  CFStringRef display = CFStringCreateWithFormat(NULL, 0, CFSTR("%@ = \"%@\"\n"), name, value);
  char *cdisplay = CFStringCopyCString(display, kDefaultStringEncoding);
  
  fputs(cdisplay, stdout);
  
  if(display) CFRelease(display);
  if(cdisplay) free(cdisplay);
  
  return KSStatusOk;
}

KSStatus ALParseCommand::addCharacterData(const ALParser *parser, CFStringRef content, void *info, KSError **error) {
  int depth = *((int *)info);
  
  CFIndex max = 64;
  CFIndex actual = 0;
  CFCharacterSetRef ignore = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("\t\v\n\r"));
  UniChar lastchar = 0;
  
  register int i;
  for(i = 0; actual < max && i < CFStringGetLength(content); i++){
    UniChar c = CFStringGetCharacterAtIndex(content, i);
    if(!CFCharacterSetIsCharacterMember(ignore, c) && !(c == ' ' && lastchar == ' ') && !(actual == 0 && c == ' ')){
      if(actual == 0){
        register int j;
        for(j = 0; j < depth; j++) fputs("  ", stdout);
        fputs("\"", stdout);
      }
      fputc(c, stdout);
      lastchar = c;
      actual++;
    }
  }
  
  if(CFStringGetLength(content) > i) fputs("...", stdout);
  if(actual > 0) fputs("\"\n", stdout);
  
  if(ignore) CFRelease(ignore);
  
  return KSStatusOk;
}

KSStatus ALParseCommand::handleParseError(const ALParser *parser, KSStatus error, CFStringRef message, void *info) {
  KSLog("Error: %@", message);
  return KSStatusOk;
}


