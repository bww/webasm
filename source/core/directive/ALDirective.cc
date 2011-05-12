// 
// $Id: ALDirective.cc 123 2010-02-02 03:34:34Z brian $
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

#include "ALDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"

#include "ALDirectiveList.hh"
#include "ALConditionalDirective.hh"
#include "ALInsertDirective.hh"
#include "ALImportDirective.hh"
#include "ALPrintDirective.hh"
#include "ALPropertyDirective.hh"
#include "ALScriptDirective.hh"
#include "ALExecuteDirective.hh"
#include "ALTypeDirective.hh"
#include "ALEscapeDirective.hh"
#include "ALVerbatimDirective.hh"

#include "ALJSDirective.hh"
#include "ALJSContext.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/CFStringAdditions.hh>

/**
 * Construct
 * 
 * @param parent parent directive
 * @param properties properties
 */
ALDirective::ALDirective(const ALDirective *parent, CFDictionaryRef properties) {
  _parent = (parent != NULL) ? KSRetain(parent) : NULL;
  _properties = 
    (properties != NULL)
    ? (CFMutableDictionaryRef)CFDictionaryCreateMutableCopy(NULL, 0, properties)
    : CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
}

/**
 * Clean up
 */
ALDirective::~ALDirective() {
  if(_parent) KSRelease(_parent);
  if(_properties) CFRelease(_properties);
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
ALDirective * ALDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  ALDirective *directive = NULL;
  KSStatus vstatus = KSStatusOk;
  CFStringRef command = NULL;
  
  if(options == NULL){
    options = gSharedOptions;
  }
  
  if(el->getType() == ALElementTypeElement){
    
    if(el->isNamed(CFSTR("insert"))){
      if((directive = ALInsertDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if(el->isNamed(CFSTR("import"))){
      if((directive = ALImportDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if(el->isNamed(CFSTR("script"))){
      if((directive = ALScriptDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if(el->isNamed(CFSTR("execute"))){
      if((directive = ALExecuteDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if(el->isNamed(CFSTR("print"))){
      if((directive = ALPrintDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if(el->isNamed(CFSTR("property"))){
      if((directive = ALPropertyDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if(el->isNamed(CFSTR("escape"))){
      if((directive = ALEscapeDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if(el->isNamed(CFSTR("text"))){
      if((directive = ALTypeDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if(el->isNamed(CFSTR("if")) || el->isNamed(CFSTR("elseif")) || el->isNamed(CFSTR("else"))){
      if((directive = ALConditionalDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
        goto error;
      }
    }else if((command = options->getCustomTagCommand(el->getName())) != NULL){
      directive = ALExecuteDirective::createWithAttributes(options, parent, properties, el->getAttributes(), command);
      if((vstatus = directive->processWAMLChildren(options, el)) != KSStatusOk){
        KSRelease(directive); directive = NULL;
        goto error;
      }
    }else{
      KSLogError("Error: unknown directive: %@", el->getName());
      vstatus = KSStatusError;
      goto error;
    }
    
  }else if(el->getType() == ALElementTypeContent){
    
    if((directive = ALVerbatimDirective::createFromWAML(options, parent, properties, el, status)) == NULL){
      goto error;
    }
    
  }
  
error:
  if(status) *status = vstatus;
  
  return directive;
}

/**
 * Process common children nodes
 * 
 * @param options options model
 * @param a node
 */
KSStatus ALDirective::processWAMLChildren(const ALOptionsModel *options, ALElement *el) {
  KSStatus status = KSStatusOk;
  
  CFArrayRef children;
  if((children = el->getChildren()) != NULL){
    
    register int i;
    for(i = 0; i < CFArrayGetCount(children); i++){
      ALElement *child = (ALElement *)CFArrayGetValueAtIndex(children, i);
      
      if(child->getType() != ALElementTypeElement)
        continue;
      
      if(child->isNamed(CFSTR("define"))){
        CFStringRef key, val;
        if((key = child->getAttribute(CFSTR("name"))) != NULL && (val = child->getAttribute(CFSTR("value"))) != NULL){
          setProperty(key, val);
        }
      }
      
    }
    
  }
  
  return status;
}

/**
 * Push our context
 * 
 * @param peer on output: the current peer, if any
 */
KSStatus ALDirective::pushJSContext(JSContextRef jsContext, JSObjectRef *peer, const ALSourceFilter *filter, KSOutputStream *outs, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  JSStringRef jsthis = JSStringCreateWithCFString(CFSTR("directive"));
  JSObjectRef global;
  JSClassRef clazz;
  JSObjectRef newpeer;
  
  if((global = JSContextGetGlobalObject(jsContext)) == NULL){
    KSLogError("No global object; cannot push JavaScript context");
    status = KSStatusError;
    goto error;
  }
  
  if((clazz = ALJSDirectiveGetClass(jsContext)) != NULL){
    newpeer = JSObjectMake(jsContext, clazz, (void *)new ALJSContext(this, filter, outs, context));
  }else{
    KSLogError("Unable to create JavaScript peer context");
  }
  
  // note the current peer
  if(peer) *peer = (JSObjectRef)JSObjectGetProperty(jsContext, global, jsthis, NULL);
  // set ourself as the peer
  JSObjectSetProperty(jsContext, global, jsthis, newpeer, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly, NULL);
  
error:
  JSStringRelease(jsthis);
  
  return status;
}

/**
 * Pop our context
 * 
 * @param peer the peer to restore
 */
KSStatus ALDirective::restoreJSContext(JSContextRef jsContext, JSObjectRef peer) const {
  KSStatus status = KSStatusOk;
  JSStringRef jsthis = JSStringCreateWithCFString(CFSTR("directive"));
  ALJSContext *peerContext = NULL;
  JSObjectRef currentPeer = NULL;
  JSObjectRef global;
  
  if((global = JSContextGetGlobalObject(jsContext)) == NULL){
    KSLogError("No global object; cannot restore JavaScript context");
    status = KSStatusError;
    goto error;
  }
  
  if((currentPeer = (JSObjectRef)JSObjectGetProperty(jsContext, global, jsthis, NULL)) != NULL){
    peerContext = (ALJSContext *)JSObjectGetPrivate(currentPeer);
  }
  
  JSObjectSetProperty(jsContext, global, jsthis, (peer != NULL) ? peer : (JSObjectRef)JSValueMakeUndefined(jsContext), kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly, NULL);
  
error:
  KSRelease(peerContext);
  JSStringRelease(jsthis);
  
  return status;
}

/**
 * Obtain this directive's name
 * 
 * @return name
 */
CFStringRef ALDirective::getName(void) const {
  return CFSTR("Directive");
}

/**
 * Obtain this directive's parent
 * 
 * @return parent directive
 */
const ALDirective * ALDirective::getParent(void) const {
  return _parent;
}

/**
 * Obtain the directives in this list
 * 
 * @return directives
 */
CFArrayRef ALDirective::getDirectives(void) const {
  return NULL;
}

/**
 * Obtain all properties
 * 
 * @return properties
 */
CFDictionaryRef ALDirective::getProperties(void) const {
  return _properties;
}

/**
 * Obtain all property names, including those defined by this directive's
 * ancestors.
 * 
 * @return all property names
 */
CFArrayRef ALDirective::copyAllPropertyNames(void) const {
  CFMutableArrayRef names = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
  appendAllPropertyNames(names);
  return names;
}

/**
 * Obtain all property names defined in this directive and ancestor directives
 * to the provided list.
 * 
 * @param names property names list to append to
 */
void ALDirective::appendAllPropertyNames(CFMutableArrayRef names) const {
  
  if(names == NULL){
    KSLogError("Provided array is NULL; cannot append property names");
    return;
  }
  
  if(_properties != NULL){
    size_t count = CFDictionaryGetCount(_properties);
    void **keys = (void **)malloc(sizeof(CFTypeRef) * count);
    
    CFDictionaryGetKeysAndValues(_properties, (const void **)keys, NULL);
    
    register int i;
    for(i = 0; i < count; i++){
      CFArrayAppendValue(names, keys[i]);
    }
    
    if(keys) free(keys);
  }
  
  const ALDirective *parent;
  if((parent = getParent()) != NULL){
    parent->appendAllPropertyNames(names);
  }
  
}

/**
 * Obtain a property by name
 * 
 * @param name property name
 * @return property value
 */
CFTypeRef ALDirective::getProperty(CFStringRef name) const {
  CFTypeRef value;
  
  if((value = ((_properties != NULL) ? (CFTypeRef)CFDictionaryGetValue(_properties, name) : NULL)) == NULL){
    const ALDirective *parent;
    if((parent = getParent()) != NULL){
      return parent->getProperty(name);
    }
  }
  
  return value;
}

/**
 * Set all properties
 * 
 * @param properties properties
 */
void ALDirective::setProperties(CFDictionaryRef properties) {
  if(_properties) CFRelease(_properties);
  _properties = 
    (properties != NULL)
    ? (CFMutableDictionaryRef)CFDictionaryCreateMutableCopy(NULL, 0, properties)
    : CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
}

/**
 * Set a property
 * 
 * @param name property name
 * @param value property value
 */
void ALDirective::setProperty(CFStringRef name, CFTypeRef value) {
  if(_properties != NULL){
    CFDictionarySetValue(_properties, name, value);
  }
}

/**
 * Declare properties in the specified scripting context
 * 
 * @param jsContext script context
 * @return status
 */
KSStatus ALDirective::declareScriptProperties(JSContextRef jsContext) const {
  // this method is no longer used
  return KSStatusOk;
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef ALDirective::copyDescription(void) const {
  CFStringRef descr = NULL;
  
  CFStringRef name;
  if((name = getName()) != NULL){
    descr = CFStringCreateWithFormat(NULL, 0, CFSTR("<%@ %p>"), name, this);
  }else{
    descr = CFStringCreateWithFormat(NULL, 0, CFSTR("<ALDirective %p>"), this);
  }
  
  return descr;
}

