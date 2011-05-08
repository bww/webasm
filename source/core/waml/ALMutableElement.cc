// 
// $Id: ALMutableElement.cc 57 2009-11-08 22:49:58Z brian $
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

#include "ALMutableElement.hh"
#include <Keystone/KSLog.hh>

#define super ALElement

/**
 * Construct
 * 
 * @param model options
 */
ALMutableElement::ALMutableElement(CFStringRef name, CFDictionaryRef attributes, CFArrayRef children) : super(name, attributes, children) {
}

/**
 * Construct
 * 
 * @param model options
 */
ALMutableElement::ALMutableElement(CFStringRef content) : super(content) {
}

/**
 * Clean up
 */
ALMutableElement::~ALMutableElement() {
}

/**
 * Set the element name
 * 
 * @param name element name
 */
void ALMutableElement::setName(CFStringRef name) {
  if(_name) CFRelease(_name);
  _name = (name != NULL) ? (CFStringRef)CFRetain(name) : NULL;
}

/**
 * Set element attributes
 * 
 * @param attributes element attributes
 */
void ALMutableElement::setAttributes(CFDictionaryRef attributes) {
  if(_attributes) CFRelease(_attributes);
  _attributes = (attributes != NULL) ? CFDictionaryCreateMutableCopy(NULL, 0, attributes) : CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
}

/**
 * Set a single attribute
 * 
 * @param name attribute name
 * @param value attribute value
 */
void ALMutableElement::putAttribute(CFStringRef name, CFStringRef value) {
  if(_attributes != NULL){
    CFTypeRef attr;
    if((attr = CFDictionaryGetValue(_attributes, name)) != NULL){
      if(CFGetTypeID(attr) == CFArrayGetTypeID()){
        CFArrayAppendValue((CFMutableArrayRef)attr, value);
      }else if(CFGetTypeID(attr) == CFStringGetTypeID()){
        CFMutableArrayRef array = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
        CFArrayAppendValue(array, attr);
        CFArrayAppendValue(array, value);
        CFDictionarySetValue(_attributes, name, array);
        if(array) CFRelease(array);
      }
    }else{
      CFDictionarySetValue(_attributes, name, value);
    }
  }
}

/**
 * Remove an attribute
 * 
 * @param name attribute name
 */
void ALMutableElement::removeAttribute(CFStringRef name) {
  if(_attributes != NULL){
    CFDictionaryRemoveValue(_attributes, name);
  }
}

/**
 * Set element children
 * 
 * @param children element children
 */
void ALMutableElement::setChildren(CFArrayRef children) {
  if(_children) CFRelease(_children);
  _children = (children != NULL) ? CFArrayCreateMutableCopy(NULL, 0, children) : CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
}

/**
 * Add a child to this element
 * 
 * @param child the child
 */
void ALMutableElement::addChild(ALElement *child) {
  if(_children != NULL){
    CFArrayAppendValue(_children, child);
  }
}

/**
 * Remove a child from this element
 * 
 * @param child the child
 */
void ALMutableElement::removeChild(ALElement *child) {
  if(_children != NULL){
    register int i;
    for(i = 0; i < CFArrayGetCount(_children); i++){
      ALElement *element = (ALElement *)CFArrayGetValueAtIndex(_children, i);
      if(element->isEqual(child)){
        CFArrayRemoveValueAtIndex(_children, i);
        break;
      }
    }
  }
}


