// 
// $Id: ALPrintDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALPrintDirective.hh"
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
ALPrintDirective::ALPrintDirective(const ALDirective *parent, CFDictionaryRef properties, CFStringRef source) : super(parent, properties) {
  _source = (source != NULL) ? (CFStringRef)CFRetain(source) : NULL;
}

/**
 * Clean up
 */
ALPrintDirective::~ALPrintDirective() {
  if(_source) CFRelease(_source);
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
ALPrintDirective * ALPrintDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALPrintDirective *directive = NULL;
  CFStringRef source = NULL;
  CFArrayRef children = NULL;
  
  if((source = el->getAttribute(CFSTR("value"))) != NULL && CFStringGetLength(source) > 0){
    directive = new ALPrintDirective(parent, properties, source);
    directive->processWAMLChildren(options, el);
  }else if((children = el->getChildren()) != NULL && CFArrayGetCount(children) > 0){
    CFMutableStringRef buffer = CFStringCreateMutable(NULL, 0);
    CFStringRef chunk;
    
    register int i;
    for(i = 0; i < CFArrayGetCount(children); i++){
      ALElement *child = (ALElement *)CFArrayGetValueAtIndex(children, i);
      if(child->getType() == ALElementTypeContent && (chunk = child->getContent()) != NULL && CFStringGetLength(chunk) > 0){
        CFStringAppend(buffer, chunk);
      }
    }
    
    directive = new ALPrintDirective(parent, properties, buffer);
    directive->processWAMLChildren(options, el);
    
    if(buffer) CFRelease(buffer);
  }else{
    KSLogError("Print directive requires script expression");
    vstatus = KSStatusError;
    goto error;
  }
  
error:
  if(status) *status = vstatus;
  
  return directive;
}

/**
 * Obtain this directive's name
 * 
 * @return name
 */
CFStringRef ALPrintDirective::getName(void) const {
  return CFSTR("Print Directive");
}

/**
 * Obtain the script source
 * 
 * @return script source
 */
CFStringRef ALPrintDirective::getScriptSource(void) const {
  return _source;
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALPrintDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  JSObjectRef peer = NULL;
  
  JSValueRef result = NULL;
  JSValueRef exception = NULL;
  JSStringRef source = NULL;
  JSStringRef sourceURL = NULL;
  JSStringRef display = NULL;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
  }
  
  if((status = pushJSContext(jsContext, &peer, filter, outs, context)) != KSStatusOk){
    goto error;
  }
  
  CFStringRef cfsource;
  if((cfsource = getScriptSource()) == NULL){
    KSLogError("Script has no source");
    status = KSStatusError;
    goto error;
  }
  
  if(options->isDebugging()){
    CFStringRef condensed = CFStringCreateCondensedCopy(getScriptSource(), CFStringCondensationOptionIgnoreBreaks, 64);
    KSLog("Emitting directive: print \"%@\"", condensed);
    if(condensed) CFRelease(condensed);
  }
  
  source = JSStringCreateWithCFString(cfsource);
  
  if((result = JSEvaluateScript(jsContext, source, NULL, sourceURL, 0, &exception)) == NULL){
    
    if((display = JSValueToStringCopy(jsContext, exception, &exception)) != NULL){
      CFStringRef convert = JSStringCopyCFString(NULL, display);
      KSLogError("Error evaluating script: %@", convert);
      if(convert) CFRelease(convert);
    }else{
      KSLogError("Error evaluating script");
    }
    
    status = KSStatusError;
    goto error;
  }else{
    if((display = JSValueToStringCopy(jsContext, result, &exception)) != NULL){
      CFStringRef convert = JSStringCopyCFString(NULL, display);
      if((status = outs->writeCFString(convert, CFRangeMake(0, CFStringGetLength(convert)), NULL)) != KSStatusOk){
        KSLogError("Unable to write script result to output");
        if(convert) CFRelease(convert);
        goto error;
      }
      if(convert) CFRelease(convert);
    }else{
      KSLogError("Error converting JavaScript value to string; cannot display");
      status = KSStatusError;
      goto error;
    }
  }
  
error:
  if(peer != NULL){
    if((status = restoreJSContext(jsContext, peer)) != KSStatusOk){
      KSLogError("Unable to restore peer");
    }
  }
  
  if(display) JSStringRelease(display);
  if(source) JSStringRelease(source);
  if(sourceURL) JSStringRelease(sourceURL);
  
  return status;
}

