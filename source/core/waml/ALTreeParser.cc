// 
// $Id: ALTreeParser.cc 44 2009-10-29 04:20:54Z brian $
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

#include "ALTreeParser.hh"
#include "ALMutableElement.hh"
#include <Keystone/KSLog.hh>

/**
 * Construct
 * 
 * @param model options
 */
ALTreeParser::ALTreeParser(ALParser *parser) {
  if((_parser = (parser != NULL) ? (ALParser *)KSRetain(parser) : NULL) != NULL){
    _parser->setDelegate(this);
  }
}

/**
 * Clean up
 */
ALTreeParser::~ALTreeParser() {
  if(_parser) KSRelease(_parser);
}

/**
 * Parse a document
 * 
 * @param ins document input stream
 * @param root on output: document root element
 * @param error on output: an error description if the status returned is not KSStatusOk
 * @return status
 */
KSStatus ALTreeParser::parse(KSInputStream *ins, ALElement **root, KSError **error) const {
  KSStatus status = KSStatusOk;
  CFMutableArrayRef stack = CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
  ALMutableElement *element = NULL;
  
  if(root) *root = NULL;
  
  if(_parser == NULL){
    status = KSStatusInvalidParameter;
    if(error) *error = new KSError(status, "Parser is null");
    goto error;
  }
  
  element = new ALMutableElement(CFSTR("<document>"), NULL, NULL);
  
  CFArrayAppendValue(stack, element);
  
  if((status = _parser->parse(ins, error, (void *)stack)) != KSStatusOk){
    KSRelease(element);
    goto error;
  }
  
  if(CFArrayGetCount(stack) < 1){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Elements are unbalanced (empty stack)");
    goto error;
  }else if(CFArrayGetCount(stack) > 1){
    status = KSStatusError;
    CFMutableStringRef buffer = CFStringCreateMutable(NULL, 0);
    
    register int i;
    for(i = 0; i < CFArrayGetCount(stack); i++){
      ALElement *straggler = (ALElement *)CFArrayGetValueAtIndex(stack, i);
      if(i > 0) CFStringAppend(buffer, CFSTR(", "));
      
      CFStringRef name;
      if((name = straggler->getName()) != NULL){
        CFStringAppend(buffer, name);
      }else{
        CFStringAppend(buffer, CFSTR("<text>"));
      }
      
    }
    
    if(error) *error = new KSError(status, "Elements are unbalanced at: %@", buffer);
    if(buffer) CFRelease(buffer);
    
    goto error;
  }
  
  if(root != NULL) *root = element;
  
error:
  if(stack) CFRelease(stack);
  
  return status;
}

/**
 * Parse a document
 * 
 * @param fulltext document text to parse
 * @param root on output: document root element
 * @param error on output: an error description if the status returned is not KSStatusOk
 * @return status
 */
KSStatus ALTreeParser::parse(CFStringRef fulltext, ALElement **root, KSError **error) const {
  return KSStatusUnimplemented;
}

/**
 * Open element
 */
KSStatus ALTreeParser::openElement(const ALParser *parser, CFStringRef name, void *info, KSError **error) {
  CFMutableArrayRef stack = (CFMutableArrayRef)info;
  
  ALElement *element = new ALMutableElement(name, NULL, NULL);
  CFArrayAppendValue(stack, element);
  KSRelease(element);
  
  return KSStatusOk;
}

/**
 * Close element
 */
KSStatus ALTreeParser::closeElement(const ALParser *parser, CFStringRef name, void *info, KSError **error) {
  KSStatus status = KSStatusOk;
  
  CFMutableArrayRef stack = (CFMutableArrayRef)info;
  ALMutableElement *element = NULL;
  ALMutableElement *parent = NULL;
  
  if(CFArrayGetCount(stack) < 1){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Element is mismatched (empty stack): %@", name);
    goto error;
  }
  
  element = (ALMutableElement *)KSRetain(((KSObject *)CFArrayGetValueAtIndex(stack, CFArrayGetCount(stack) - 1)));
  
  if(CFStringCompare(element->getName(), name, kCFCompareCaseInsensitive) != kCFCompareEqualTo){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Element is mismatched (expected %@): %@", element->getName(), name);
    goto error;
  }
  
  CFArrayRemoveValueAtIndex(stack, CFArrayGetCount(stack) - 1);
  
  if(CFArrayGetCount(stack) < 1){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Stack is emtpy: %@", name);
    goto error;
  }
  
  if((parent = (ALMutableElement *)CFArrayGetValueAtIndex(stack, CFArrayGetCount(stack) - 1)) == NULL){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Stack element is null: %@", name);
    goto error;
  }
  
  parent->addChild(element);
  
error:
  if(element) KSRelease(element);
  
  return status;
}

/**
 * Add attributes
 */
KSStatus ALTreeParser::addElementAttribute(const ALParser *parser, CFStringRef name, CFStringRef value, void *info, KSError **error) {
  KSStatus status = KSStatusOk;
  
  CFMutableArrayRef stack = (CFMutableArrayRef)info;
  ALMutableElement *element = NULL;
  
  if(CFArrayGetCount(stack) < 1){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Element is mismatched (empty stack)");
    goto error;
  }
  
  element = (ALMutableElement *)CFArrayGetValueAtIndex(stack, CFArrayGetCount(stack) - 1);
  
  if(element->getType() != ALElementTypeElement){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Attributes cannot be applied to a non-element type");
    goto error;
  }
  
  element->putAttribute(name, value);
  
error:
  return status;
}

/**
 * Add character data
 */
KSStatus ALTreeParser::addCharacterData(const ALParser *parser, CFStringRef content, void *info, KSError **error) {
  KSStatus status = KSStatusOk;
  
  CFMutableArrayRef stack = (CFMutableArrayRef)info;
  ALMutableElement *element = NULL;
  ALMutableElement *parent = NULL;
  
  if(CFArrayGetCount(stack) < 1){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Element is mismatched (empty stack)");
    goto error;
  }
  
  parent = (ALMutableElement *)CFArrayGetValueAtIndex(stack, CFArrayGetCount(stack) - 1);
  
  if(parent->getType() != ALElementTypeElement){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Character data cannot be nested in a non-element type");
    goto error;
  }
  
  element = new ALMutableElement(content);
  parent->addChild(element);
  KSRelease(element);
  
error:
  return status;
}

/**
 * Handle parser error
 */
KSStatus ALTreeParser::handleParseError(const ALParser *parser, KSStatus error, CFStringRef message, void *info) {
  return KSStatusOk;
}


