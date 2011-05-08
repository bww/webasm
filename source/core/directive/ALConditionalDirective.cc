// 
// $Id: ALConditionalDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALConditionalDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/CFStringAdditions.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSInputStream.hh>

#define super ALDirectiveList

/**
 * Internal constructor
 * 
 * @param parent parent directive
 * @param properties properties
 * @param op conditional operation
 * @param source test expression source
 */
ALConditionalDirective::ALConditionalDirective(const ALDirective *parent, CFDictionaryRef properties, ALConditionalOperation op, CFStringRef source) : super(parent, properties) {
  _operation = op;
  _source = (source != NULL) ? (CFStringRef)CFRetain(source) : NULL;
  _expression = (source != NULL) ? JSStringCreateWithCFString(source) : NULL;
  _result = NULL;
}

/**
 * Clean up
 */
ALConditionalDirective::~ALConditionalDirective() {
  if(_source) CFRelease(_source);
  if(_expression) JSStringRelease(_expression);
  if(_result) CFRelease(_result);
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
ALConditionalDirective * ALConditionalDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALConditionalDirective *directive = NULL;
  CFStringRef source = NULL;
  
  if(el->isNamed(CFSTR("if")) || el->isNamed(CFSTR("elseif"))){
    if((source = el->getAttribute(CFSTR("test"))) == NULL || CFStringGetLength(source) < 1){
      KSLogError("Variable directive requires a test attribute");
      vstatus = KSStatusError;
      goto error;
    }
  }
  
  ALConditionalOperation op;
  if(el->isNamed(CFSTR("if"))){
    op = ALConditionalIf;
  }else if(el->isNamed(CFSTR("elseif"))){
    op = ALConditionalElseIf;
  }else{
    op = ALConditionalElse;
  }
  
  directive = new ALConditionalDirective(parent, properties, op, source);
  if((vstatus = directive->processWAMLChildren(options, el)) != KSStatusOk){
    KSRelease(directive);
    directive = NULL;
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
CFStringRef ALConditionalDirective::getName(void) const {
  return CFSTR("Conditional Directive");
}

/**
 * Obtain the expression source
 * 
 * @return expression source
 */
CFStringRef ALConditionalDirective::getExpressionSource(void) const {
  return _source;
}

/**
 * Obtain the evaulation result.  The result is a boolean wrapped in a CFNumber
 * (kCFNumberSInt8Type) which indicates the evaluated test expression.
 * 
 * @return evaulation result
 */
CFTypeRef ALConditionalDirective::getEvaulationResult(void) const {
  return _result;
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALConditionalDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  JSValueRef result = NULL;
  JSValueRef exception = NULL;
  JSStringRef sourceURL = NULL;
  CFNumberRef nfalse = NULL;
  const ALDirective *parent = NULL;
  JSObjectRef peer = NULL;
  int8_t value = 0;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
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
  
  if(_expression == NULL && _operation != ALConditionalElse){
    KSLogError("Expression is NULL; not processing");
    return KSStatusError;
  }
  
  if(jsContext == NULL && _operation != ALConditionalElse){
    KSLogError("No JavaScript context provided; not processing");
    return KSStatusError;
  }
  
  if((parent = getParent()) == NULL){
    KSLogError("No parent directive for conditional; not processing");
    return KSStatusError;
  }
  
  if(options->isDebugging()){
    if(_operation != ALConditionalElse){
      CFStringRef display = JSStringCopyCFString(NULL, _expression);
      CFStringRef condensed = CFStringCreateCondensedCopy(display, CFStringCondensationOptionIgnoreBreaks, 64);
      KSLog("Emitting conditional: %@ (%@)", ((_operation == ALConditionalIf) ? CFSTR("if") : CFSTR("else if")), condensed);
      if(condensed) CFRelease(condensed);
      if(display)   CFRelease(display);
    }else{
      KSLog("Emitting conditional: else");
    }
  }
  
  if((status = pushJSContext(jsContext, &peer)) != KSStatusOk){
    goto error;
  }
  
  if(_operation != ALConditionalIf){
    
    ALDirective *previous = (ALDirective *)this;
    while((previous = parent->getPreviousSibling(previous)) != NULL){
      if(!previous->isVerbatim()) break;
    }
    
    if(previous == NULL){
      KSLogError("Dependent conditional has no previous sibling; not processing");
      status = KSStatusError;
      goto error;
    }
    
    CFTypeRef psresult;
    if((psresult = previous->getEvaulationResult()) == NULL){
      KSLogError("Dependent conditional previous sibling has null result; not processing");
      status = KSStatusError;
      goto error;
    }
    
    if(CFGetTypeID(psresult) != CFNumberGetTypeID()){
      KSLogError("Dependent conditional previous sibling result is not boolean; not processing");
      status = KSStatusError;
      goto error;
    }
    
    int8_t bpsresult;
    CFNumberGetValue((CFNumberRef)psresult, kCFNumberSInt8Type, &bpsresult);
    
    // stop processing if the previous sibling satisfied the condition
    if(bpsresult != 0){
      goto error;
    }
    
  }
  
  if(_operation != ALConditionalElse){
    sourceURL = JSStringCreateWithCFString(bpath);
    
    if((result = JSEvaluateScript(jsContext, _expression, NULL, sourceURL, 0, &exception)) == NULL){
      JSStringRef display;
      
      if((display = JSValueToStringCopy(jsContext, exception, &exception)) != NULL){
        CFStringRef convert = JSStringCopyCFString(NULL, display);
        KSLogError("Error executing condition: %@", convert);
        if(convert) CFRelease(convert);
      }else{
        KSLogError("Error executing condition AND error obtaining cause");
      }
      
      status = KSStatusError;
      goto error;
    }
    
    if(JSValueToBoolean(jsContext, result)){
      super::emit(filter, outs, jsContext, context);
      value = 1;
    }
    
  }else{
    super::emit(filter, outs, jsContext, context);
    value = 1;
  }
  
error:
  if(peer != NULL){
    if((status = restoreJSContext(jsContext, peer)) != KSStatusOk){
      KSLogError("Unable to restore peer");
    }
  }
  
  if(sourceURL) JSStringRelease(sourceURL);
  if(nfalse)    CFRelease(nfalse);
  
  _result = CFNumberCreate(NULL, kCFNumberSInt8Type, &value);
  
  return status;
}

