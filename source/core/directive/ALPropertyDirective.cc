// 
// $Id: ALPropertyDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALPropertyDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/CFStringAdditions.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSInputStream.hh>

#define super ALDirective

/**
 * Internal constructor
 * 
 * @param parent parent directive
 * @param properties properties
 * @param name variable name
 */
ALPropertyDirective::ALPropertyDirective(const ALDirective *parent, CFDictionaryRef properties, CFStringRef name) : super(parent, properties) {
  _name = (name != NULL) ? (CFStringRef)CFRetain(name) : NULL;
}

/**
 * Clean up
 */
ALPropertyDirective::~ALPropertyDirective() {
  if(_name) CFRelease(_name);
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
ALPropertyDirective * ALPropertyDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALPropertyDirective *directive = NULL;
  CFStringRef name = NULL;
  
  if((name = el->getAttribute(CFSTR("name"))) == NULL || CFStringGetLength(name) < 1){
    KSLogError("Property directive requires a name attribute");
    vstatus = KSStatusError;
    goto error;
  }
  
  directive = new ALPropertyDirective(parent, properties, name);
  if((vstatus = directive->processWAMLChildren(options, el)) != KSStatusOk){
    KSRelease(directive); directive = NULL;
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
CFStringRef ALPropertyDirective::getName(void) const {
  return CFSTR("Variable Directive");
}

/**
 * Obtain the variable name
 * 
 * @return variable name
 */
CFStringRef ALPropertyDirective::getVariableName(void) const {
  return _name;
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALPropertyDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  CFTypeRef value = (CFTypeRef)getProperty(getVariableName());
  JSObjectRef peer = NULL;
  CFStringRef output = NULL;
  char *cvalue = NULL;
  KSLength actual;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
  }
  
  if((status = pushJSContext(jsContext, &peer)) != KSStatusOk){
    goto error;
  }
  
  if(value != NULL){
    output = (CFGetTypeID(value) == CFStringGetTypeID()) ? (CFStringRef)CFRetain(value) : CFCopyDescription(value);
  }
  
  if(output != NULL){
    cvalue = CFStringCopyCString(output, kDefaultStringEncoding);
    if((status = outs->write((KSByte *)cvalue, strlen(cvalue), &actual)) != KSStatusOk || actual < strlen(cvalue)){
      KSLogError("An error occured while writing property directive (partial write)");
      goto error;
    }
  }
  
error:
  if(peer != NULL){
    if((status = restoreJSContext(jsContext, peer)) != KSStatusOk){
      KSLogError("Unable to restore peer");
    }
  }
  
  if(cvalue) free(cvalue);
  if(output) CFRelease(output);
  
  return status;
}

