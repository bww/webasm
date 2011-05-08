// 
// $Id: ALVerbatimDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALVerbatimDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"
#include "ALHTMLSourceFilter.hh"

#include <Keystone/CFStringAdditions.hh>
#include <Keystone/KSLog.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSInputStream.hh>

#define super ALDirectiveList

/**
 * Constructor
 * 
 * @param parent parent directive
 * @param properties properties
 * @param content verbatim content
 */
ALVerbatimDirective::ALVerbatimDirective(const ALDirective *parent, CFDictionaryRef properties, CFStringRef content) : super(parent, properties) {
  _content = (content != NULL) ? (CFStringRef)CFRetain(content) : NULL;
}

/**
 * Clean up
 */
ALVerbatimDirective::~ALVerbatimDirective() {
  if(_content) CFRelease(_content);
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
ALVerbatimDirective * ALVerbatimDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALVerbatimDirective *directive = NULL;
  CFStringRef content = NULL;
  
  if(el->getType() != ALElementTypeContent){
    KSLogError("Invalid node type for verbatim directive");
    vstatus = KSStatusError;
    goto error;
  }
  
  if((content = el->getContent()) == NULL){
    KSLogError("Verbatim directive requires content");
    vstatus = KSStatusError;
    goto error;
  }
  
  directive = new ALVerbatimDirective(parent, properties, content);
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
CFStringRef ALVerbatimDirective::getName(void) const {
  return CFSTR("Verbatim Directive");
}

/**
 * Obtain the verbatim content
 * 
 * @return content
 */
CFStringRef ALVerbatimDirective::getContent(void) const {
  return _content;
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALVerbatimDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  CFStringRef processed = NULL;
  CFStringRef content;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
  }
  
  if((content = getContent()) == NULL){
    KSLogError("Verbatim content is null");
    status = KSStatusError;
    goto error;
  }
  
  if(options->isDebugging()){
    KSLog("Emitting directive: verbatim (%d characters)", CFStringGetLength(content));
  }
  
  if(filter->getOptionsModel()->getCollapseWhitespace()){
    processed = CFStringCreateCondensedCopy(content, CFStringCondensationOptionIgnoreBreaks, 0);
  }else{
    processed = (CFStringRef)CFRetain(content);
  }
  
  if((status = outs->writeCFString(processed, CFRangeMake(0, CFStringGetLength(processed)), NULL)) != KSStatusOk){
    KSLogError("Unable to write verbtaim content: %@", KSStatusGetMessage(status));
    goto error;
  }
  
error:
  if(processed) CFRelease(processed);
  
  return status;
}

