// 
// $Id: KSArray.cc 24 2008-12-20 21:15:53Z brian $
// Keystone Framework
// 
// Copyright (c) 2008 Wolter Group New York, Inc., All rights reserved.
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

#include "KSArray.hh"

extern const KSArray *kEmptyKSArray = (KSArray *)KSMakeConstant(new KSArray());

/**
 * Construct
 */
KSArray::KSArray() {
  _array = CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
}

/**
 * Construct
 * 
 * @param array initial contents
 */
KSArray::KSArray(CFArrayRef array) {
  _array = (array != NULL) ? CFArrayCreateMutableCopy(NULL, 0, array) : CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
}

/**
 * Construct
 * 
 * @param array initial contents
 */
KSArray::KSArray(KSArray *array) {
  _array = (array != NULL) ? CFArrayCreateMutableCopy(NULL, 0, array->_array) : CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
}

/**
 * Clean up
 */
KSArray::~KSArray() {
  if(_array) CFRelease(_array);
}

/**
 * Create with objects
 * 
 * @param o1 first object
 * @param ... remaining objects (NULL-terminated list)
 * @return array
 */
KSArray * KSArray::createWithObjects(KSObject *o1, ...) {
  KSArray *array = new KSArray();
  
  CFArrayAppendValue(array->_array, o1);
  
  va_list ap;
  va_start(ap, o1);
  
  KSObject *on;
  while((on = va_arg(ap, KSObject *)) != NULL){
    CFArrayAppendValue(array->_array, on);
  }
  
  va_end(ap);
  
  return array;
}

/**
 * Obtain an object
 * 
 * @param i index
 * @return object
 */
KSObject * KSArray::getObjectAtIndex(KSIndex i) const {
  return (KSObject *)CFArrayGetValueAtIndex(_array, i);
}

/**
 * Obtain the first object
 * 
 * @return first object
 */
KSObject * KSArray::getFirstObject(void) const {
  return (CFArrayGetCount(_array) > 0) ? (KSObject *)CFArrayGetValueAtIndex(_array, 0) : NULL;
}

/**
 * Obtain the last object
 * 
 * @return last object
 */
KSObject * KSArray::getLastObject(void) const {
  return (CFArrayGetCount(_array) > 0) ? (KSObject *)CFArrayGetValueAtIndex(_array, CFArrayGetCount(_array) - 1) : NULL;
}

/**
 * Obtain the array element count
 * 
 * @return count
 */
KSLength KSArray::getCount(void) const {
  return CFArrayGetCount(_array);
}

/**
 * Create a new array by appending the specified object to the end of this
 * array.
 * 
 * @param o object to append
 * @return new array
 */
KSArray * KSArray::createByAppendingObject(KSObject *o) const {
  KSArray *copy = new KSArray(_array);
  CFArrayAppendValue(copy->_array, o);
  return copy;
}

/**
 * Obtain a copy of this array
 * 
 * @return copy
 */
KSArray * KSArray::copy(void) const {
  return new KSArray(_array);
}

/**
 * Obtain the underlying CFType
 * 
 * @return CFArrayRef
 */
CFTypeRef KSArray::getCFTypeRef(void) const {
  return _array;
}

/**
 * Determine semantic equality
 * 
 * @param o candidate
 * @return equal or not
 */
bool KSArray::isEqual(KSArray *o) const {
  return CFEqual(o->_array, _array);
}

/**
 * Obtain a hash code
 * 
 * @return hash code
 */
CFHashCode KSArray::getHashCode(void) const {
  return CFHash(_array);
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSArray::copyDescription(void) const {
  return CFStringCreateWithFormat(NULL, NULL, CFSTR("[KSArray %d]"), getCount());
}

