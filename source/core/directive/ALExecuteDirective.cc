// 
// $Id: ALExecuteDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALExecuteDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"

#include <Keystone/CFStringAdditions.hh>
#include <Keystone/KSLog.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSInputStream.hh>
#include <Keystone/KSFileInputStream.hh>
#include <Keystone/KSFileOutputStream.hh>

#include <unistd.h>
#include <sys/wait.h>

#define super ALDirectiveList

/**
 * Constructor
 * 
 * @param parent parent directive
 * @param properties properties
 * @param attributes attributes
 * @param resource resource name
 */
ALExecuteDirective::ALExecuteDirective(const ALDirective *parent, CFDictionaryRef properties, CFArrayRef arguments, CFDictionaryRef attributes, CFStringRef command) : super(parent, properties) {
  _command = (command != NULL) ? (CFStringRef)CFRetain(command) : NULL;
  _arguments = (arguments != NULL) ? (CFArrayRef)CFRetain(arguments) : NULL;
  _attributes = (attributes != NULL) ? (CFDictionaryRef)CFRetain(attributes) : NULL;
}

/**
 * Clean up
 */
ALExecuteDirective::~ALExecuteDirective() {
  if(_command)    CFRelease(_command);
  if(_arguments)  CFRelease(_arguments);
  if(_attributes) CFRelease(_attributes);
}

/**
 * Create with raw attributes
 * 
 * @param options options model
 * @param parent parent directive
 * @param properties properties
 * @param rawattrs raw attributes
 * @param command executable path
 * @return directive
 */
ALExecuteDirective * ALExecuteDirective::createWithAttributes(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, CFDictionaryRef rawattrs, CFStringRef command) {
  ALExecuteDirective *directive = NULL;
  CFMutableDictionaryRef attributes = NULL;
  CFArrayRef arguments = NULL;
  
  attributes = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  
  CFIndex acount = CFDictionaryGetCount(rawattrs);
  const void **keys = (const void **)malloc(sizeof(CFTypeRef) * acount);
  const void **vals = (const void **)malloc(sizeof(CFTypeRef) * acount);
  CFDictionaryGetKeysAndValues(rawattrs, keys, vals);
  
  register int i;
  for(i = 0; i < acount; i++){
    
    if(CFStringCompare((CFStringRef)keys[i], CFSTR("arg"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
      if(CFGetTypeID(vals[i]) == CFArrayGetTypeID()){
        arguments = CFArrayCreateCopy(NULL, (CFArrayRef)vals[i]);
      }else{
        arguments = CFArrayCreate(NULL, (vals + i), 1, &kCFTypeArrayCallBacks);
      }
    }else{
      if(CFGetTypeID(vals[i]) == CFArrayGetTypeID()){
        if(CFArrayGetCount((CFArrayRef)vals[i]) > 0){
          CFDictionarySetValue(attributes, (CFStringRef)keys[i], CFArrayGetValueAtIndex((CFArrayRef)vals[i], 0));
        }
      }else{
        CFDictionarySetValue(attributes, (CFStringRef)keys[i], vals[i]);
      }
    }
    
  }
  
  if(keys) free(keys);
  if(vals) free(vals);
  
  directive = new ALExecuteDirective(parent, properties, arguments, attributes, command);
  
error:
  if(attributes)  CFRelease(attributes);
  if(arguments)   CFRelease(arguments);
  
  return directive;
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
ALExecuteDirective * ALExecuteDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALExecuteDirective *directive = NULL;
  CFStringRef command = NULL;
  
  if((command = el->getAttribute(CFSTR("command"))) == NULL || CFStringGetLength(command) < 1){
    KSLogError("Import directive requires a resource attribute");
    vstatus = KSStatusError;
    goto error;
  }
  
  directive = createWithAttributes(options, parent, properties, el->getAttributes(), command);
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
CFStringRef ALExecuteDirective::getName(void) const {
  return CFSTR("Execute Directive");
}

/**
 * Obtain the command path
 * 
 * @return command path
 */
CFStringRef ALExecuteDirective::getCommandPath(void) const {
  return _command;
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALExecuteDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  KSStatus vstatus;
  pid_t child;
  
  JSObjectRef peer = NULL;
  CFStringRef apath = NULL;
  KSFile *file = NULL;
  KSInputStream *ins = NULL;
  KSOutputStream *cmdouts = NULL;
  
  int odesc[] = { -1, -1 };
  int idesc[] = { -1, -1 };
  int z;
  
  KSByte *chunk = NULL;
  KSLength chunklen = 1024;
  KSLength actual = 0;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
  }
  
  if((status = pushJSContext(jsContext, &peer, filter, outs, context)) != KSStatusOk){
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
  
  apath = CFStringCreateCanonicalPath(ddpath, getCommandPath());
  file  = new KSFile(apath);
  
  if(options->isDebugging()){
    CFMutableStringRef args = NULL;
    
    if(_arguments != NULL){
      args = CFStringCreateMutable(NULL, 0);
      
      register int i;
      for(i = 0; i < CFArrayGetCount(_arguments); i++){
        CFStringAppend(args, CFSTR(" "));
        CFStringAppend(args, (CFStringRef)CFArrayGetValueAtIndex(_arguments, i));
      }
      
    }
    
    KSLog("Emitting directive: execute (%@%@)", getCommandPath(), ((args != NULL) ? args : CFSTR("")));
    
    if(args) CFRelease(args);
  }
  
  if(!file->exists()){
    KSLogError("Executable does not exist: %@ (%@)", apath, bpath);
    status = KSStatusError;
    goto error;
  }
  
  if((z = pipe(odesc)) < 0){
    KSLogError("Unable to create pipe: %s", strerror(errno));
    status = KSStatusError;
    goto error;
  }
  
  if((z = pipe(idesc)) < 0){
    KSLogError("Unable to create pipe: %s", strerror(errno));
    status = KSStatusError;
    goto error;
  }
  
  if((child = fork()) < 0){
    KSLogError("Unable to fork for executable: %s", strerror(errno));
    status = KSStatusError;
    goto error;
  }else if(child == 0){
    char *cpath;
    if((cpath = CFStringCopyCString(apath, kDefaultStringEncoding)) != NULL){
      CFArrayRef pnames = copyAllPropertyNames();
      CFStringRef temps = NULL;
      
      char **args;
      if(_arguments != NULL){
        args = (char **)malloc(sizeof(char *) * (CFArrayGetCount(_arguments) + 2));
        args[0] = cpath;
        
        register int i;
        for(i = 0; i < CFArrayGetCount(_arguments); i++){
          args[i + 1] = CFStringCopyCString((CFStringRef)CFArrayGetValueAtIndex(_arguments, i), kDefaultStringEncoding);
        }
        
        args[i + 1] = NULL;
      }else{
        args = (char *[]){ cpath, NULL };
      }
      
      int envcount = 10 + ((_attributes != NULL) ? CFDictionaryGetCount(_attributes) : 0) + ((pnames != NULL) ? CFArrayGetCount(pnames) : 0);
      
      close(idesc[1]);
      dup2(idesc[0], STDIN_FILENO);
      dup2(odesc[1], STDOUT_FILENO);
      
      char *envp[envcount];
      
      bzero(envp, sizeof(char *) * envcount);
      char **penvp = envp;
      
      temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_SOURCE_ROOT=%@"), filter->getOptionsModel()->getProjectBasePath());
      *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
      CFRelease(temps);
      penvp++;
      
      temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_OUTPUT_ROOT=%@"), filter->getOptionsModel()->getOutputBasePath());
      *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
      CFRelease(temps);
      penvp++;
      
      temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_QUIET=%@"), ((filter->getOptionsModel()->isQuiet()) ? CFSTR("true") : CFSTR("false")));
      *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
      CFRelease(temps);
      penvp++;
      
      temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_VERBOSE=%@"), ((filter->getOptionsModel()->isVerbose()) ? CFSTR("true") : CFSTR("false")));
      *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
      CFRelease(temps);
      penvp++;
      
      temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_DEBUGGING=%@"), ((filter->getOptionsModel()->isDebugging()) ? CFSTR("true") : CFSTR("false")));
      *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
      CFRelease(temps);
      penvp++;
      
      temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_FORCE=%@"), ((filter->getOptionsModel()->getForce()) ? CFSTR("true") : CFSTR("false")));
      *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
      CFRelease(temps);
      penvp++;
      
      temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_INCLUDE_HIDDEN_FILES=%@"), ((filter->getOptionsModel()->getIncludeHiddenFiles()) ? CFSTR("true") : CFSTR("false")));
      *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
      CFRelease(temps);
      penvp++;
      
      if(_attributes != NULL){
        const void **keys = (const void **)malloc(sizeof(const void *) * CFDictionaryGetCount(_attributes));
        const void **vals = (const void **)malloc(sizeof(const void *) * CFDictionaryGetCount(_attributes));
        
        CFDictionaryGetKeysAndValues(_attributes, keys, vals);
        
        register int i;
        for(i = 0; i < CFDictionaryGetCount(_attributes); i++){
          CFStringRef pname = CFStringCopyMangledName((CFStringRef)keys[i], '_');
          temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_TAG_ATTR_%@=%@"), pname, (CFStringRef)vals[i]);
          *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
          CFRelease(temps);
          CFRelease(pname);
          penvp++;
        }
        
        free(keys);
        free(vals);
      }
      
      if(pnames != NULL){
        register int i;
        for(i = 0; i < CFArrayGetCount(pnames); i++){
          CFStringRef kname = (CFStringRef)CFArrayGetValueAtIndex(pnames, i);
          CFStringRef pname = CFStringCopyMangledName(kname, '_');
          temps   = CFStringCreateWithFormat(NULL, 0, CFSTR("WEBASM_PROPERTY_%@=%@"), pname, (CFTypeRef)getProperty(kname));
          *penvp  = CFStringCopyCString(temps, kDefaultStringEncoding);
          CFRelease(temps);
          CFRelease(pname);
          penvp++;
        }
      }
      
      if(execve(cpath, args, envp) < 0){
        KSLogError("Unable to execute");
        exit(-1);
      }
      
      /* Unreachable... */
      
    }else{
      exit(-1);
    }
  }else{
    
    cmdouts = new KSFileOutputStream(idesc[1]);
    
    if((vstatus = super::emit(filter, cmdouts, jsContext, context)) != KSStatusOk){
      KSLogError("Unable to write to child output stream: %@", KSStatusGetMessage(vstatus));
      status = vstatus;
      goto error;
    }
    
    close(idesc[1]); idesc[1] = -1;
    close(idesc[0]); idesc[0] = -1;
    close(odesc[1]); odesc[1] = -1;
    
    ins = new KSFileInputStream(odesc[0]);
    chunk = (KSByte *)malloc(chunklen);
    
    while((vstatus = ins->read(chunk, chunklen, &actual)) == KSStatusOk && actual > 0){
      if((vstatus = outs->write(chunk, actual, &actual)) != KSStatusOk){
        KSLogError("Unable to read from child input stream: %s", strerror(errno));
        status = vstatus;
        goto error;
      }
    }
    
    free(chunk); chunk = NULL;
    
    int result;
    if((z = waitpid(child, &result, 0)) < 0){
      KSLogError("An error occured while waiting for child process (%d): %s", child, strerror(errno));
      status = KSStatusError;
      goto error;
    }
    
    int rvalue = 0;
    if(!WIFEXITED(result) || (rvalue = WEXITSTATUS(result)) != 0){
      KSLogError("Executable did not exit cleanly (%d)", rvalue);
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
  
  if(apath)   CFRelease(apath);
  if(ddpath)  CFRelease(ddpath);
  if(file)    KSRelease(file);
  if(chunk)   free(chunk);
  
  if(odesc[1] > -1) close(odesc[1]);
  if(odesc[0] > -1) close(odesc[0]);
  if(idesc[1] > -1) close(idesc[1]);
  if(idesc[0] > -1) close(idesc[0]);
  
  if(ins){
    ins->close();
    KSRelease(ins);
  }
  
  if(cmdouts){
    cmdouts->close();
    KSRelease(cmdouts);
  }
  
  return status;
}

