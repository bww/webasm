// 
// $Id: ALJSDirective.cc 123 2010-02-02 03:34:34Z brian $
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

#include "ALJSDirective.hh"
#include "ALDirective.hh"
#include "ALInsertDirective.hh"
#include "ALHTMLSourceFilter.hh"
#include "ALJSUtility.hh"
#include "ALJSContext.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSUtility.hh>
#include <Keystone/CFStringAdditions.hh>

static void ALJSDirective_init(JSContextRef jsContext, JSObjectRef object) {
}

static void ALJSDirective_finalize(JSObjectRef object) {
}

static JSValueRef ALJSDirective_getProperty(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALJSContext *context = (ALJSContext *)JSObjectGetPrivate(object);
  ALDirective *directive = (ALDirective *)context->getPeer();
  CFStringRef name      = NULL;
  CFStringRef value     = NULL;
  JSStringRef jsname    = NULL;
  JSStringRef jsvalue   = NULL;
  JSValueRef  result    = NULL;
  
  if(argc < 1){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("getProperty(name) requires 1 argument: the name of the property to obtain"), error);
    }
  }
  
  if((jsname = JSValueToStringCopy(jsContext, argv[0], exception)) == NULL){
    KSLogError("Unable to copy name argument");
    goto error;
  }
  
  name = JSStringCopyCFString(NULL, jsname);
  
  if((value = (CFStringRef)directive->getProperty(name)) != NULL){
    jsvalue = JSStringCreateWithCFString(value);
    result = JSValueMakeString(jsContext, jsvalue);
  }else{
    result = JSValueMakeUndefined(jsContext);
  }
  
error:
  if(name)    CFRelease(name);
  if(jsname)  JSStringRelease(jsname);
  if(jsvalue) JSStringRelease(jsvalue);
  
  return result;
}

/**
 * Set a property at the level of the caller directive.
 */
static JSValueRef ALJSDirective_setProperty(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALJSContext *context = (ALJSContext *)JSObjectGetPrivate(object);
  ALDirective *directive = (ALDirective *)context->getPeer();
  JSStringRef jstring = NULL;
  CFStringRef key = NULL;
  CFStringRef value = NULL;
  
  if(argc < 2){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("setProperty(name, value) requires 2 arguments: 1) the name of the property to set, 2) the value to set"), error);
    }else{
      goto error;
    }
  }
  
  if((jstring = JSValueToStringCopy(jsContext, argv[0], exception)) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Unable to copy argument"), error);
    }else{
      goto error;
    }
  }
  
  if((key = JSStringCopyCFString(NULL, jstring)) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Unable to convert argument"), error);
    }else{
      goto error;
    }
  }
  
  JSStringRelease(jstring); // immediately assigned again below
  
  if((jstring = JSValueToStringCopy(jsContext, argv[1], exception)) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Unable to copy argument"), error);
    }else{
      goto error;
    }
  }
  
  if((value = JSStringCopyCFString(NULL, jstring)) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Unable to convert argument"), error);
    }else{
      goto error;
    }
  }
  
  // set the property value
  directive->setProperty(key, value);
  
error:
  if(jstring)   JSStringRelease(jstring);
  if(key)       CFRelease(key);
  if(value)     CFRelease(value);
  
  return JSValueMakeUndefined(jsContext);
}

static JSValueRef ALJSDirective_toString(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALJSContext *context = (ALJSContext *)JSObjectGetPrivate(object);
  ALDirective *directive = (ALDirective *)context->getPeer();
  JSValueRef result = NULL;
  JSStringRef string = NULL;
  CFStringRef descr = NULL;
  
  descr = CFStringCreateWithFormat(NULL, 0, CFSTR("<%@>"), directive->getName());
  string = JSStringCreateWithCFString(descr);
  result = JSValueMakeString(jsContext, string);
  
error:
  if(descr)   CFRelease(descr);
  if(string)  JSStringRelease(string);
  
  return result;
}

/**
 * Insert a resource into the document as if an insert directive were nested in the caller directive.
 */
static JSValueRef ALJSDirective_insert(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALJSContext *context = (ALJSContext *)JSObjectGetPrivate(object);
  ALDirective *directive = (ALDirective *)context->getPeer();
  ALHTMLSourceFilter *filter = NULL;
  ALInsertDirective *insert = NULL;
  KSOutputStream *ostream = NULL;
  JSStringRef jresource = NULL;
  CFStringRef resource = NULL;
  KSStatus status = KSStatusOk;
  
  if(argc < 1){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("insert() requires 1 argument (the path to a resource to insert)"), error);
    }else{
      goto error;
    }
  }
  
  if((filter = (ALHTMLSourceFilter *)context->getSourceFilter()) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("No filter"), error);
    }else{
      goto error;
    }
  }
  
  if((ostream = context->getOutputStream()) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("No current output stream"), error);
    }else{
      goto error;
    }
  }
  
  if((jresource = JSValueToStringCopy(jsContext, argv[0], exception)) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Unable to copy argument content"), error);
    }else{
      goto error;
    }
  }
  
  if((resource = JSStringCopyCFString(NULL, jresource)) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Unable to convert argument content"), error);
    }else{
      goto error;
    }
  }
  
  insert = new ALInsertDirective(directive, NULL, resource);
  
  if((status = insert->emit(filter, ostream, jsContext, context->getSourceFilterContext())) != KSStatusOk){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Unable to insert"), error);
    }else{
      goto error;
    }
  }
  
error:
  if(insert)    KSRelease(insert);
  if(jresource) JSStringRelease(jresource);
  if(resource)  CFRelease(resource);
  
  return JSValueMakeUndefined(jsContext);
}

static JSStaticValue ALJSDirective_values[] = {
  { 0, 0, 0, 0 }
};

static JSStaticFunction ALJSDirective_functions[] = {
  { "getProperty",        ALJSDirective_getProperty,        kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "setProperty",        ALJSDirective_setProperty,        kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "toString",           ALJSDirective_toString,           kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "insert",             ALJSDirective_insert,             kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { 0, 0, 0 }
};

/**
 * Obtain the JS class
 * 
 * @param context JS context
 * @return class
 */
JSClassRef ALJSDirectiveGetClass(JSContextRef ctx) {
  static JSClassRef clazz = NULL;
  if(clazz == NULL){
    JSClassDefinition classDefinition = kJSClassDefinitionEmpty;
    
    classDefinition.staticValues    = ALJSDirective_values;
    classDefinition.staticFunctions = ALJSDirective_functions;
    classDefinition.initialize      = ALJSDirective_init;
    classDefinition.finalize        = ALJSDirective_finalize;
    
    clazz = JSClassCreate(&classDefinition);
  }
  return clazz;
}

