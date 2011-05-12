// 
// $Id: ALJSDocument.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALJSDocument.hh"
#include "ALHTMLSourceFilter.hh"
#include "ALJSUtility.hh"

#include "ALEscapeDirective.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSUtility.hh>
#include <Keystone/KSOutputStream.hh>
#include <Keystone/CFStringAdditions.hh>

static void ALJSDocument_init(JSContextRef jsContext, JSObjectRef object) {
}

static void ALJSDocument_finalize(JSObjectRef object) {
}

static JSValueRef ALJSDocument_getParent(JSContextRef jsContext, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception) {
  return NULL;
}

static JSValueRef ALJSDocument_getRoot(JSContextRef jsContext, JSObjectRef object, JSStringRef propertyName, JSValueRef *exception) {
  JSObjectRef parent = object;
  JSObjectRef root = object;
  JSStringRef name = JSStringCreateWithCFString(CFSTR("parent"));
  
  while(!JSValueIsUndefined(jsContext, (parent = (JSObjectRef)JSObjectGetProperty(jsContext, parent, name, NULL)))){
    root = parent;
  }
  
  if(name) JSStringRelease(name);
  
  return root;
}

static JSValueRef ALJSDocument_getTransposedPath(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALHTMLSourceFilter *filter = (ALHTMLSourceFilter *)JSObjectGetPrivate(object);
  const ALSourceFilterContext *context = NULL;
  JSValueRef result = NULL;
  CFMutableStringRef buffer = NULL;
  
  JSStringRef path        = NULL;
  JSStringRef transposed  = NULL;
  
  CFStringRef vpath       = NULL;
  CFStringRef rpath       = NULL;
  CFStringRef tpath       = NULL;
  
  CFStringRef ldpath      = NULL;
  CFStringRef ldbase      = NULL;
  CFStringRef tdbase      = NULL;
  
  CFIndex diverge;
  CFIndex ascents;
  CFIndex trailing;
  
  int i;
  
  if(argc < 1){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("getTransposedPath() requires 1 argument (a path)"), error);
    }else{
      goto error;
    }
  }
  
  if((context = filter->getContextInfo()) == NULL){
    KSLogError("No document context");
    goto error;
  }
  
  if((path = JSValueToStringCopy(jsContext, argv[0], exception)) == NULL){
    KSLogError("Unable to copy argument path");
    goto error;
  }
  
  vpath = JSStringCopyCFString(NULL, path);
  
  if((ldpath = context->getDestinationPath()) == NULL){
    KSLogError("No document destination path");
    goto error;
  }
  
  if((ldbase = CFStringCopyPathDirectory(ldpath)) == NULL){
    KSLogError("Unable to copy destination directory");
    goto error;
  }
  
  if((tdbase = context->getDestinationBasePath()) == NULL){
    KSLogError("No document base path");
    goto error;
  }
  
  if((rpath = CFStringCreateRelativePathWithOptions(tdbase, ldbase, 0, &diverge, &ascents, &trailing)) == NULL){
    KSLogError("Unable to create relative path");
    goto error;
  }
  
  buffer = CFStringCreateMutable(NULL, 0);
  
  for(i = 0; i < trailing; i++){
    if(i > 0) CFStringAppend(buffer, kPathComponentDelimiterString);
    CFStringAppend(buffer, CFSTR(".."));
  }
  
  if(i > 0) CFStringAppend(buffer, kPathComponentDelimiterString);
  CFStringAppend(buffer, vpath);
  
  transposed = JSStringCreateWithCFString(buffer);
  result = JSValueMakeString(jsContext, transposed);
  
error:
  if(vpath)       CFRelease(vpath);
  if(rpath)       CFRelease(rpath);
  if(tpath)       CFRelease(tpath);
  if(ldbase)      CFRelease(ldbase);
  if(buffer)      CFRelease(buffer);
  
  if(path)        JSStringRelease(path);
  if(transposed)  JSStringRelease(transposed);
  
  return result;
}

static JSValueRef ALJSDocument_getSourcePath(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALHTMLSourceFilter *filter = (ALHTMLSourceFilter *)JSObjectGetPrivate(object);
  const ALSourceFilterContext *context = NULL;
  JSValueRef result = NULL;
  JSStringRef string = NULL;
  
  CFStringRef spath;
  
  if((context = filter->getContextInfo()) == NULL){
    KSLogError("No document context");
    goto error;
  }
  
  if((spath = context->getSourcePath()) == NULL){
    KSLogError("No document source path");
    goto error;
  }
  
  string = JSStringCreateWithCFString(spath);
  result = JSValueMakeString(jsContext, string);
  
error:
  if(string)  JSStringRelease(string);
  
  return result;
}

static JSValueRef ALJSDocument_getDestinationPath(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALHTMLSourceFilter *filter = (ALHTMLSourceFilter *)JSObjectGetPrivate(object);
  const ALSourceFilterContext *context = NULL;
  JSValueRef result = NULL;
  JSStringRef string = NULL;
  
  CFStringRef dpath;
  
  if((context = filter->getContextInfo()) == NULL){
    KSLogError("No document context");
    goto error;
  }
  
  if((dpath = context->getDestinationPath()) == NULL){
    KSLogError("No document destination path");
    goto error;
  }
  
  string = JSStringCreateWithCFString(dpath);
  result = JSValueMakeString(jsContext, string);
  
error:
  if(string)  JSStringRelease(string);
  
  return result;
}

static JSValueRef ALJSDocument_escape(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  CFCharacterSetRef permitted = NULL;
  CFMutableStringRef buffer = NULL;
  JSStringRef jcontent = NULL;
  CFStringRef content = NULL;
  JSValueRef result = NULL;
  JSStringRef string = NULL;
  
  if(argc < 1){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("escape() requires 1 argument (a string to escape)"), error);
    }else{
      goto error;
    }
  }
  
  if((jcontent = JSValueToStringCopy(jsContext, argv[0], exception)) == NULL){
    KSLogError("Unable to copy argument content");
    goto error;
  }
  
  if((content = JSStringCopyCFString(NULL, jcontent)) == NULL){
    KSLogError("Unable to convert argument content");
    goto error;
  }
  
  permitted = CFCharacterSetCreateWithCharactersInString(NULL, kALEscapeDirectivePermittedCharacters);
  buffer = CFStringCreateMutable(NULL, 0);
  
  register int i;
  for(i = 0; i < CFStringGetLength(content); i++){
    UniChar c = CFStringGetCharacterAtIndex(content, i);
    if(CFCharacterSetIsCharacterMember(permitted, c)){
      CFStringAppendCharacters(buffer, &c, 1);
    }else{
      CFStringRef format = CFStringCreateWithFormat(NULL, 0, CFSTR("&#%d;"), (int)c);
      CFStringAppend(buffer, format);
      if(format) CFRelease(format);
    }
  }
  
  string = JSStringCreateWithCFString(buffer);
  result = JSValueMakeString(jsContext, string);
  
error:
  if(permitted) CFRelease(permitted);
  if(buffer)    CFRelease(buffer);
  if(jcontent)  JSStringRelease(jcontent);
  if(content)   CFRelease(content);
  if(string)    JSStringRelease(string);
  
  return result;
}

static JSValueRef ALJSDocument_print(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALHTMLSourceFilter *filter = (ALHTMLSourceFilter *)JSObjectGetPrivate(object);
  KSOutputStream *ostream = NULL;
  JSStringRef content = NULL;
  CFStringRef vcontent = NULL;
  
  if(argc < 1){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("print() requires 1 argument (an object to print)"), error);
    }else{
      goto error;
    }
  }
  
  if((ostream = filter->getCurrentOutputStream()) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Peer object has no current output stream"), error);
    }else{
      goto error;
    }
  }
  
  if((content = JSValueToStringCopy(jsContext, argv[0], exception)) == NULL){
    KSLogError("Unable to copy argument content");
    goto error;
  }
  
  if((vcontent = JSStringCopyCFString(NULL, content)) != NULL){
    ostream->writeCFString(vcontent, CFRangeMake(0, CFStringGetLength(vcontent)), NULL);
  }
  
error:
  if(vcontent)    CFRelease(vcontent);
  if(content)     JSStringRelease(content);
  
  return JSValueMakeUndefined(jsContext);
}

static JSValueRef ALJSDocument_println(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALHTMLSourceFilter *filter = (ALHTMLSourceFilter *)JSObjectGetPrivate(object);
  CFStringRef newline = CFSTR("\n");
  KSOutputStream *ostream = NULL;
  JSStringRef content = NULL;
  CFStringRef vcontent = NULL;
  
  if(argc < 1){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("print() requires 1 argument (an object to print)"), error);
    }else{
      goto error;
    }
  }
  
  if((ostream = filter->getCurrentOutputStream()) == NULL){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("Peer object has no current output stream"), error);
    }else{
      goto error;
    }
  }
  
  if((content = JSValueToStringCopy(jsContext, argv[0], exception)) == NULL){
    KSLogError("Unable to copy argument content");
    goto error;
  }
  
  if((vcontent = JSStringCopyCFString(NULL, content)) != NULL){
    ostream->writeCFString(vcontent, CFRangeMake(0, CFStringGetLength(vcontent)), NULL);
    ostream->writeCFString(newline, CFRangeMake(0, CFStringGetLength(newline)), NULL);
  }
  
error:
  if(vcontent)    CFRelease(vcontent);
  if(content)     JSStringRelease(content);
  
  return JSValueMakeUndefined(jsContext);
}


static JSValueRef ALJSDocument_log(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  JSStringRef content = NULL;
  CFStringRef vcontent = NULL;
  
  if(argc < 1){
    if(exception != NULL){
      ALJSMakeException(jsContext, *exception, CFSTR("print() requires 1 argument (an object to print)"), error);
    }else{
      goto error;
    }
  }
  
  if((content = JSValueToStringCopy(jsContext, argv[0], exception)) == NULL){
    KSLogError("Unable to copy argument content");
    goto error;
  }
  
  if((vcontent = JSStringCopyCFString(NULL, content)) != NULL){
    KSLog("[JS]: %@", vcontent);
  }
  
error:
  if(vcontent)    CFRelease(vcontent);
  if(content)     JSStringRelease(content);
  
  return JSValueMakeUndefined(jsContext);
}

static JSValueRef ALJSDocument_toString(JSContextRef jsContext, JSObjectRef function, JSObjectRef object, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
  ALHTMLSourceFilter *filter = (ALHTMLSourceFilter *)JSObjectGetPrivate(object);
  const ALSourceFilterContext *context = NULL;
  JSValueRef result = NULL;
  JSStringRef string = NULL;
  CFStringRef descr = NULL;
  
  CFStringRef spath;
  
  if((context = filter->getContextInfo()) == NULL){
    KSLogError("No document context");
    goto error;
  }
  
  if((spath = context->getSourcePath()) == NULL){
    KSLogError("No document source path");
    goto error;
  }
  
  descr = CFStringCreateWithFormat(NULL, 0, CFSTR("<AssemblerDocument \"%@\">"), spath);
  string = JSStringCreateWithCFString(descr);
  result = JSValueMakeString(jsContext, string);
  
error:
  if(descr)   CFRelease(descr);
  if(string)  JSStringRelease(string);
  
  return result;
}

static JSStaticValue ALJSDocument_values[] = {
  { "root",   ALJSDocument_getRoot,   NULL, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { 0, 0, 0, 0 }
};

static JSStaticFunction ALJSDocument_functions[] = {
  { "getTransposedPath",  ALJSDocument_getTransposedPath,   kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "getSourcePath",      ALJSDocument_getSourcePath,       kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "getDestinationPath", ALJSDocument_getDestinationPath,  kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "escape",             ALJSDocument_escape,              kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "print",              ALJSDocument_print,               kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "println",            ALJSDocument_println,             kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "log",                ALJSDocument_log,                 kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { "toString",           ALJSDocument_toString,            kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly },
  { 0, 0, 0 }
};

/**
 * Obtain the JS class
 * 
 * @param context JS context
 * @return class
 */
JSClassRef ALJSDocumentGetClass(JSContextRef ctx) {
  static JSClassRef clazz = NULL;
  if(clazz == NULL){
    JSClassDefinition classDefinition = kJSClassDefinitionEmpty;
    
    classDefinition.staticValues    = ALJSDocument_values;
    classDefinition.staticFunctions = ALJSDocument_functions;
    classDefinition.initialize      = ALJSDocument_init;
    classDefinition.finalize        = ALJSDocument_finalize;
    
    clazz = JSClassCreate(&classDefinition);
  }
  return clazz;
}

