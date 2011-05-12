// 
// $Id: ALEscapeDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALEscapeDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"
#include "ALHTMLSourceFilter.hh"

#include <Keystone/CFStringAdditions.hh>
#include <Keystone/KSLog.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSInputStream.hh>
#include <Keystone/KSDataBufferOutputStream.hh>

#define super ALDirectiveList

/**
 * Constructor
 * 
 * @param parent parent directive
 * @param properties properties
 */
ALEscapeDirective::ALEscapeDirective(const ALDirective *parent, CFDictionaryRef properties) : super(parent, properties) {
}

/**
 * Clean up
 */
ALEscapeDirective::~ALEscapeDirective() {
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
ALEscapeDirective * ALEscapeDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALEscapeDirective *directive = NULL;
  
  directive = new ALEscapeDirective(parent, properties);
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
CFStringRef ALEscapeDirective::getName(void) const {
  return CFSTR("Escape Directive");
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALEscapeDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  KSDataBufferOutputStream *ostream = NULL;
  CFMutableDataRef ftbuffer = NULL;
  CFStringRef fulltext = NULL;
  CFCharacterSetRef permitted = NULL;
  CFMutableStringRef buffer = NULL;
  JSObjectRef peer = NULL;
  
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
  
  if(options->isDebugging()){
    KSLog("Emitting directive: escape");
  }
  
  if((status = pushJSContext(jsContext, &peer, filter, outs, context)) != KSStatusOk){
    goto error;
  }
  
  ftbuffer = CFDataCreateMutable(NULL, 0);
  ostream = new KSDataBufferOutputStream(ftbuffer);
  
  if((status = super::emit(filter, ostream, jsContext, context)) != KSStatusOk){
    KSLogError("Unable to write to child output stream: %@", KSStatusGetMessage(status));
    goto error;
  }
  
  if((fulltext = CFStringCreateWithBytes(NULL, CFDataGetBytePtr(ftbuffer), CFDataGetLength(ftbuffer), kDefaultStringEncoding, false)) == NULL){
    KSLogError("Unable to create full text from buffer");
    status = KSStatusError;
    goto error;
  }
  
  KSRelease(ostream); ostream = NULL;
  CFRelease(ftbuffer); ftbuffer = NULL;
  
  permitted = CFCharacterSetCreateWithCharactersInString(NULL, kALEscapeDirectivePermittedCharacters);
  buffer = CFStringCreateMutable(NULL, 0);
  
  register int i;
  for(i = 0; i < CFStringGetLength(fulltext); i++){
    UniChar c = CFStringGetCharacterAtIndex(fulltext, i);
    if(CFCharacterSetIsCharacterMember(permitted, c)){
      CFStringAppendCharacters(buffer, &c, 1);
    }else{
      CFStringRef format = CFStringCreateWithFormat(NULL, 0, CFSTR("&#%d;"), (int)c);
      CFStringAppend(buffer, format);
      if(format) CFRelease(format);
    }
  }
  
  if((status = outs->writeCFString(buffer, CFRangeMake(0, CFStringGetLength(buffer)), NULL)) != KSStatusOk){
    KSLogError("Unable to write escaped content: %@", KSStatusGetMessage(status));
    goto error;
  }
  
error:
  if(peer != NULL){
    if((status = restoreJSContext(jsContext, peer)) != KSStatusOk){
      KSLogError("Unable to restore peer");
    }
  }
  
  if(permitted) CFRelease(permitted);
  if(buffer)    CFRelease(buffer);
  if(ftbuffer)  CFRelease(ftbuffer);
  
  if(ostream){
    ostream->close();
    KSRelease(ostream);
  }
  
  return status;
}

