// 
// $Id: ALElement.cc 57 2009-11-08 22:49:58Z brian $
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

#include "ALElement.hh"
#include <Keystone/KSLog.hh>

/**
 * Construct
 * 
 * @param model options
 */
ALElement::ALElement(CFStringRef name, CFDictionaryRef attributes, CFArrayRef children) {
  _name = (name != NULL) ? (CFStringRef)CFRetain(name) : NULL;
  _attributes = (attributes != NULL) ? CFDictionaryCreateMutableCopy(NULL, 0, attributes) : CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  _children = (children != NULL) ? CFArrayCreateMutableCopy(NULL, 0, children) : CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
  _content = NULL;
  _type = ALElementTypeElement;
}

/**
 * Construct
 * 
 * @param model options
 */
ALElement::ALElement(CFStringRef content) {
  _content = (content != NULL) ? (CFStringRef)CFRetain(content) : NULL;
  _name = NULL;
  _attributes = NULL;
  _children = NULL;
  _type = ALElementTypeContent;
}

/**
 * Clean up
 */
ALElement::~ALElement() {
  if(_name)       CFRelease(_name);
  if(_attributes) CFRelease(_attributes);
  if(_children)   CFRelease(_children);
  if(_content)    CFRelease(_content);
}

/**
 * Determine if the name of this element matches the specified name
 * 
 * @param name a name
 * @return matches this element name or not
 */
bool ALElement::isNamed(CFStringRef name) const {
  return (_name != NULL && name != NULL && CFStringCompare(_name, name, kCFCompareCaseInsensitive) == kCFCompareEqualTo);
}

/**
 * Obtain the element name
 * 
 * @return element name
 */
CFStringRef ALElement::getName(void) const {
  return _name;
}

/**
 * Obtain element attributes
 * 
 * @return element attributes
 */
CFDictionaryRef ALElement::getAttributes(void) const {
  return _attributes;
}

/**
 * Obtain a single attribute value by name.  The first attribute defined with
 * the specified value is returned.
 * 
 * @param name attribute name
 * @return attribute value
 */
CFStringRef ALElement::getAttribute(CFStringRef name) const {
  
  if(_attributes != NULL){
    CFTypeRef attr;
    if((attr = CFDictionaryGetValue(_attributes, name)) != NULL){
      if(CFGetTypeID(attr) == CFStringGetTypeID()){
        return (CFStringRef)attr;
      }else if(CFGetTypeID(attr) == CFArrayGetTypeID() && CFArrayGetCount((CFArrayRef)attr) > 0){
        return (CFStringRef)CFArrayGetValueAtIndex((CFArrayRef)attr, 0);
      }
    }
  }
  
  return NULL;
}

/**
 * Obtain all attributes values for the speicified name.  If the requested
 * attribute has only one value, it is converted to an array.
 * 
 * @param name attribute name
 * @return attribute values
 */
CFArrayRef ALElement::getEveryAttribute(CFStringRef name) const {
  
  if(_attributes != NULL){
    CFTypeRef attr;
    if((attr = CFDictionaryGetValue(_attributes, name)) != NULL){
      if(CFGetTypeID(attr) == CFStringGetTypeID()){
        CFMutableArrayRef array = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
        CFArrayAppendValue((CFMutableArrayRef)array, attr);
        CFDictionarySetValue(_attributes, name, array);
        if(array) CFRelease(array);
        return array;
      }else if(CFGetTypeID(attr) == CFArrayGetTypeID() && CFArrayGetCount((CFArrayRef)attr) > 0){
        return (CFArrayRef)attr;
      }
    }
  }
  
  return NULL;
}

/**
 * Obtain element children
 * 
 * @return element children
 */
CFArrayRef ALElement::getChildren(void) const {
  return _children;
}

/**
 * Obtain element content
 * 
 * @return content
 */
CFStringRef ALElement::getContent(void) const {
  return _content;
}


