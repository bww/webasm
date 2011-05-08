// 
// $Id: ALDirectiveList.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALDirectiveList.hh"
#include "ALInsertDirective.hh"
#include "ALImportDirective.hh"
#include "ALScriptDirective.hh"
#include "ALPrintDirective.hh"
#include "ALPropertyDirective.hh"
#include "ALConditionalDirective.hh"
#include "ALVerbatimDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"

#include <Keystone/KSLog.hh>

#define super ALDirective

/**
 * Constructor
 * 
 * @param parent parent directive
 * @param properties properties
 */
ALDirectiveList::ALDirectiveList(const ALDirective *parent, CFDictionaryRef properties) : super(parent, properties) {
  _directives = CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
}

/**
 * Clean up
 */
ALDirectiveList::~ALDirectiveList() {
  if(_directives) CFRelease(_directives);
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
ALDirectiveList * ALDirectiveList::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  ALDirectiveList *directive = new ALDirectiveList(parent, properties);
  KSStatus vstatus;
  
  if((vstatus = directive->processWAMLChildren(options, el)) != KSStatusOk){
    KSRelease(directive);
    directive = NULL;
  }
  
  if(status) *status = vstatus;
  return directive;
}

/**
 * Process common children nodes
 * 
 * @param el the element whose children to process
 */
KSStatus ALDirectiveList::processWAMLChildren(const ALOptionsModel *options, ALElement *el) {
  KSStatus status = KSStatusOk;
  
  if(el == NULL)
    return status;
  
  CFArrayRef children;
  if((children = el->getChildren()) != NULL){
    
    register int i;
    for(i = 0; i < CFArrayGetCount(children); i++){
      ALElement *child = (ALElement *)CFArrayGetValueAtIndex(children, i);
      
      ALDirective *subdir;
      if((subdir = ALDirective::createFromWAML(options, this, NULL, child, &status)) != NULL){
        addDirective(subdir);
        KSRelease(subdir);
      }else{
        goto error;
      }
      
    }
    
  }
  
  super::processWAMLChildren(options, el);
  
error:
  return status;
}

/**
 * Obtain this directive's name
 * 
 * @return name
 */
CFStringRef ALDirectiveList::getName(void) const {
  return CFSTR("Directive List");
}

/**
 * Obtain the directives in this list
 * 
 * @return directives
 */
CFArrayRef ALDirectiveList::getDirectives(void) const {
  return _directives;
}

/**
 * Obtain the directive which precedes the specified directive in this list.
 * If there is no preceding sibling, NULL is returned.
 * 
 * @param directive the reference directive
 * @return previous sibling directive
 */
ALDirective * ALDirectiveList::getPreviousSibling(ALDirective *directive) const {
  ALDirective *previous = NULL;
  
  if(directive != NULL && _directives != NULL){
    register int i;
    for(i = 0; i < CFArrayGetCount(_directives); i++){
      ALDirective *check;
      if((check = (ALDirective *)CFArrayGetValueAtIndex(_directives, i)) != NULL && check->isEqual(directive)){
        return previous;
      }else{
        previous = check;
      }
    }
  }
  
  return NULL;
}

/**
 * Obtain the directive which follows the specified directive in this list.
 * If there is no next sibling, NULL is returned.
 * 
 * @param directive the reference directive
 * @return next sibling directive
 */
ALDirective * ALDirectiveList::getNextSibling(ALDirective *directive) const {
  
  if(directive != NULL && _directives != NULL){
    register int i;
    for(i = 0; i < CFArrayGetCount(_directives); i++){
      ALDirective *check;
      if((check = (ALDirective *)CFArrayGetValueAtIndex(_directives, i)) != NULL && check->isEqual(directive)){
        if((i + 1) < CFArrayGetCount(_directives)){
          return (ALDirective *)CFArrayGetValueAtIndex(_directives, (i + 1));
        }
      }
    }
  }
  
  return NULL;
}

/**
 * Set the directives in this list
 * 
 * @param d directives
 */
void ALDirectiveList::setDirectives(CFArrayRef d) {
  CFArrayRemoveAllValues(_directives);
  if(d != NULL) CFArrayAppendArray(_directives, d, CFRangeMake(0, CFArrayGetCount(d)));
}

/**
 * Add a directive to this list
 * 
 * @parma d directive
 */
void ALDirectiveList::addDirective(ALDirective *d) {
  if(d != NULL) CFArrayAppendValue(_directives, d);
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALDirectiveList::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
  }
  
  if(options->isDebugging()){
    CFIndex count = CFArrayGetCount(_directives);
    KSLog("Emitting directive: directive list (%d %@)", count, ((count == 1) ? CFSTR("child") : CFSTR("children")));
  }
  
  register int i;
  for(i = 0; i < CFArrayGetCount(_directives); i++){
    ALDirective *directive = (ALDirective *)CFArrayGetValueAtIndex(_directives, i);
    if((status = directive->emit(filter, outs, jsContext, context)) != KSStatusOk){
      goto error;
    }
  }
  
error:
  return status;
}

