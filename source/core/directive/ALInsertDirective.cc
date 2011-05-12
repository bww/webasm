// 
// $Id: ALInsertDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALInsertDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"
#include "ALHTMLSourceFilter.hh"

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
ALInsertDirective::ALInsertDirective(const ALDirective *parent, CFDictionaryRef properties, CFStringRef resource) : super(parent, properties) {
  _resource = (resource != NULL) ? (CFStringRef)CFRetain(resource) : NULL;
  _options  = kALInsertOptionNone;
}

/**
 * Clean up
 */
ALInsertDirective::~ALInsertDirective() {
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
ALInsertDirective * ALInsertDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALInsertDirective *directive = NULL;
  CFStringRef src = NULL;
  
  if((src = el->getAttribute(CFSTR("resource"))) == NULL || CFStringGetLength(src) < 1){
    KSLogError("Insert directive requires a resource attribute");
    vstatus = KSStatusError;
    goto error;
  }
  
  directive = new ALInsertDirective(parent, properties, src);
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
CFStringRef ALInsertDirective::getName(void) const {
  return CFSTR("Insert Directive");
}

/**
 * Obtain the resource path
 * 
 * @return resource path
 */
CFStringRef ALInsertDirective::getResourcePath(void) const {
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
KSStatus ALInsertDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  KSInputStream *ins = NULL;
  ALSourceFilter *subfilter = NULL;
  CFStringRef apath = NULL;
  CFDictionaryRef subinfo = NULL;
  ALSourceFilterContext *subcontext = NULL;
  KSFile *file = NULL;
  JSObjectRef peer = NULL;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
  }
  
  CFStringRef spath;
  if((spath = context->getSourcePath()) == NULL){
    KSLogError("No source path");
    return KSStatusError;
  }
  
  CFStringRef dpath;
  if((dpath = context->getDestinationPath()) == NULL){
    KSLogError("No destination path");
    return KSStatusError;
  }
  
  CFStringRef bpath;
  if((bpath = context->getSourceBasePath()) == NULL){
    KSLogError("No base path");
    return KSStatusError;
  }
  
  CFStringRef opath;
  if((opath = context->getDestinationBasePath()) == NULL){
    KSLogError("No base path");
    return KSStatusError;
  }
  
  CFStringRef dspath;
  if((dspath = CFStringCopyPathDirectory(spath)) == NULL){
    KSLogError("Unable to copy directory path: %@", spath);
    return KSStatusError;
  }
  
  apath = CFStringCreateCanonicalPath(dspath, getResourcePath());
  
  // note the direct dependency
  ALDependencyGraph *graph;
  if((graph = context->getDependencyGraph()) != NULL){
    graph->addDirectDependency(apath);
  }
  
  const void * keys[] = { kALSourceFilterSourcePathKey, kALSourceFilterDestinationPathKey, kALSourceFilterSourceBasePathKey, kALSourceFilterDestinationBasePathKey };
  const void * vals[] = { apath, dpath, bpath, opath };
  
  file  = new KSFile(apath);
  
  if(options->isDebugging()){
    KSLog("Emitting directive: insert (%@)", getResourcePath());
  }
  
  if((status = pushJSContext(jsContext, &peer, filter, outs, context)) != KSStatusOk){
    goto error;
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
  
  subinfo = CFDictionaryCreate(NULL, (const void **)keys, (const void **)vals, 4, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  subcontext = new ALSourceFilterContext(subinfo, NULL);
  subfilter = new ALHTMLSourceFilter(options, filter, this, subcontext, jsContext);
  
  if((status = subfilter->filter(ins, outs)) != KSStatusOk){
    KSLogError("Unable to insert: %@", apath);
    goto error;
  }
  
error:
  if(peer != NULL){
    if((status = restoreJSContext(jsContext, peer)) != KSStatusOk){
      KSLogError("Unable to restore peer");
    }
  }
  
  if(apath)       CFRelease(apath);
  if(dspath)      CFRelease(dspath);
  if(file)        KSRelease(file);
  if(subinfo)     CFRelease(subinfo);
  if(subcontext)  KSRelease(subcontext);
  if(subfilter)   KSRelease(subfilter);
  
  if(ins){
    ins->close();
    KSRelease(ins);
  }
  
  return status;
}

