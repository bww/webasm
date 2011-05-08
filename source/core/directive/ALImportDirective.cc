// 
// $Id: ALImportDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALImportDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"

#include <Keystone/CFStringAdditions.hh>
#include <Keystone/KSLog.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSInputStream.hh>

#define super ALDirective

/**
 * Constructor
 * 
 * @param parent parent directive
 * @param properties properties
 * @param resource resource name
 */
ALImportDirective::ALImportDirective(const ALDirective *parent, CFDictionaryRef properties, CFStringRef resource) : super(parent, properties) {
  _resource = (resource != NULL) ? (CFStringRef)CFRetain(resource) : NULL;
}

/**
 * Clean up
 */
ALImportDirective::~ALImportDirective() {
  if(_resource) CFRelease(_resource);
}

/**
 * Create from WAML
 * 
 * @param options options model
 * @param parent parent directive
 * @param properties properties
 * @param el root element
 * @param status on output: status
 */
ALImportDirective * ALImportDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALImportDirective *directive = NULL;
  CFStringRef src = NULL;
  
  if((src = el->getAttribute(CFSTR("resource"))) == NULL || CFStringGetLength(src) < 1){
    KSLogError("Import directive requires a resource attribute");
    vstatus = KSStatusError;
    goto error;
  }
  
  directive = new ALImportDirective(parent, properties, src);
  directive->processWAMLChildren(options, el);
  
error:
  if(status) *status = vstatus;
  
  return directive;
}

/**
 * Obtain this directive's name
 * 
 * @return name
 */
CFStringRef ALImportDirective::getName(void) const {
  return CFSTR("Import Directive");
}

/**
 * Obtain the resource path
 * 
 * @return resource path
 */
CFStringRef ALImportDirective::getResourcePath(void) const {
  return _resource;
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALImportDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  KSStatus vstatus;
  
  JSObjectRef peer = NULL;
  
  CFStringRef apath = NULL;
  KSFile *file = NULL;
  KSInputStream *ins = NULL;
  
  CFMutableDataRef buffer = NULL;
  CFStringRef fulltext = NULL;
  KSByte *chunk = NULL;
  KSLength chunklen = 1024;
  KSLength actual = 0;
  
  JSValueRef result = NULL;
  JSValueRef exception = NULL;
  JSStringRef source = NULL;
  JSStringRef sourceURL = NULL;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
  }
  
  if((status = pushJSContext(jsContext, &peer)) != KSStatusOk){
    goto error;
  }
  
  CFStringRef dpath;
  if((dpath = context->getDestinationPath()) == NULL){
    KSLogError("No destination path");
    status = KSStatusError;
    goto error;
  }
  
  CFStringRef bpath;
  if((bpath = context->getSourceBasePath()) == NULL){
    KSLogError("No base path");
    status = KSStatusError;
    goto error;
  }
  
  CFStringRef ddpath;
  if((ddpath = CFStringCopyPathDirectory(dpath)) == NULL){
    KSLogError("Unable to copy directory path: %@", dpath);
    status = KSStatusError;
    goto error;
  }
  
  apath = CFStringCreateCanonicalPath(ddpath, getResourcePath());
  file  = new KSFile(apath);
  
  // note the direct dependency
  ALDependencyGraph *graph;
  if((graph = context->getDependencyGraph()) != NULL){
    graph->addDirectDependency(apath);
  }
  
  if(options->isDebugging()){
    KSLog("Emitting directive: import (%@)", getResourcePath());
  }
  
  if(!file->exists()){
    KSLogError("Resource does not exist: %@ (%@)", apath, bpath);
    status = KSStatusError;
    goto error;
  }
  
  if((ins = file->createInputStream(&status)) == NULL){
    KSLogError("Unabele to open input stream: %@", apath);
    goto error;
  }
  
  buffer = CFDataCreateMutable(NULL, 0);
  chunk = (KSByte *)malloc(chunklen);
  
  while((vstatus = ins->read(chunk, chunklen, &actual)) == KSStatusOk && actual > 0){
    CFDataAppendBytes(buffer, chunk, actual);
  }
  
  free(chunk);
  
  if((fulltext = CFStringCreateWithBytes(NULL, CFDataGetBytePtr(buffer), CFDataGetLength(buffer), kDefaultStringEncoding, false)) == NULL){
    KSLogError("Unable to load JavaScript source");
    status = KSStatusError;
    goto error;
  }
  
  source = JSStringCreateWithCFString(fulltext);
  
  if((result = JSEvaluateScript(jsContext, source, NULL, sourceURL, 0, &exception)) == NULL){
    JSStringRef display;
    
    if((display = JSValueToStringCopy(jsContext, exception, &exception)) != NULL){
      CFStringRef convert = JSStringCopyCFString(NULL, display);
      KSLogError("Error evaluating script (%@): %@", apath, convert);
      if(convert) CFRelease(convert);
    }else{
      KSLogError("Error evaluating script: %@", apath);
    }
    
    status = KSStatusError;
    goto error;
  }
  
error:
  if(peer != NULL){
    if((status = restoreJSContext(jsContext, peer)) != KSStatusOk){
      KSLogError("Unable to restore peer");
    }
  }
  
  if(apath)     CFRelease(apath);
  if(ddpath)    CFRelease(ddpath);
  if(buffer)    CFRelease(buffer);
  if(fulltext)  CFRelease(fulltext);
  if(source)    JSStringRelease(source);
  if(sourceURL) JSStringRelease(sourceURL);
  if(file)      KSRelease(file);
  
  if(ins){
    ins->close();
    KSRelease(ins);
  }
  
  return status;
}

