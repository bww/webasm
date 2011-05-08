// 
// $Id: KSNumber.cc 21 2008-12-15 03:45:09Z brian $
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

#include "KSNumber.hh"
#include "CFNumberAdditions.hh"

/**
 * Construct
 */
KSNumber::KSNumber() {
  int number = 0;
  _number = CFNumberCreate(NULL, kCFNumberSInt32Type, &number);
}

/**
 * Construct
 * 
 * @param number the number
 */
KSNumber::KSNumber(int number) {
  _number = CFNumberCreate(NULL, kCFNumberSInt32Type, &number);
}

/**
 * Construct
 * 
 * @param number the number
 */
KSNumber::KSNumber(float number) {
  _number = CFNumberCreate(NULL, kCFNumberFloatType, &number);
}

/**
 * Construct
 * 
 * @param number the number
 */
KSNumber::KSNumber(long number) {
  _number = CFNumberCreate(NULL, kCFNumberSInt64Type, &number);
}

/**
 * Construct
 * 
 * @param number the number
 */
KSNumber::KSNumber(double number) {
  _number = CFNumberCreate(NULL, kCFNumberDoubleType, &number);
}

/**
 * Construct
 * 
 * @param number the number
 */
KSNumber::KSNumber(CFNumberRef number) {
  _number = CFNumberCreateCopy(number);
}

/**
 * Clean up
 */
KSNumber::~KSNumber() {
  if(_number) CFRelease(_number);
}

/**
 * Obtain this number's integer value
 * 
 * @return int value
 */
int KSNumber::getIntValue(void) const {
  
  if(CFNumberIsFloatType(_number)){
    double value;
    CFNumberGetValue(_number, kCFNumberDoubleType, &value);
    return (int)value;
  }else{
    long value;
    CFNumberGetValue(_number, kCFNumberLongType, &value);
    return (int)value;
  }
  
}

/**
 * Obtain this number's float value
 * 
 * @return float value
 */
float KSNumber::getFloatValue(void) const {
  
  if(CFNumberIsFloatType(_number)){
    double value;
    CFNumberGetValue(_number, kCFNumberDoubleType, &value);
    return (float)value;
  }else{
    long value;
    CFNumberGetValue(_number, kCFNumberLongType, &value);
    return (float)value;
  }
  
}

/**
 * Obtain this number's long value
 * 
 * @return long value
 */
long KSNumber::getLongValue(void) const {
  
  if(CFNumberIsFloatType(_number)){
    double value;
    CFNumberGetValue(_number, kCFNumberDoubleType, &value);
    return (long)value;
  }else{
    long value;
    CFNumberGetValue(_number, kCFNumberLongType, &value);
    return value;
  }
  
}

/**
 * Obtain this number's double value
 * 
 * @return double value
 */
double KSNumber::getDoubleValue(void) const {
  
  if(CFNumberIsFloatType(_number)){
    double value;
    CFNumberGetValue(_number, kCFNumberDoubleType, &value);
    return value;
  }else{
    long value;
    CFNumberGetValue(_number, kCFNumberLongType, &value);
    return (double)value;
  }
  
}

/**
 * Determine if this number represents a floating-point value.
 * 
 * @return floating point or not
 */
bool KSNumber::isFloatingPoint(void) const {
  return CFNumberIsFloatType(_number);
}

/**
 * Obtain the underlying number type
 * 
 * @return number type
 */
CFNumberType KSNumber::getNumberType(void) const {
  return CFNumberGetType(_number);
}

/**
 * Obtain the number of bytes required to store this number
 * 
 * @return storage size in bytes
 */
KSLength KSNumber::getByteSize(void) const {
  return CFNumberGetByteSize(_number);
}

/**
 * Obtain the underlying CFType.  This type is owned by the wrapper class.
 * 
 * @return CFType
 */
CFTypeRef KSNumber::getCFTypeRef(void) const {
  return _number;
}

/**
 * Compare numbers
 * 
 * @param n number
 * @return comparison result
 */
CFComparisonResult KSNumber::compare(KSNumber *n) const {
  return CFNumberCompare(_number, n->_number, NULL);
}

/**
 * Determine semantic equality
 * 
 * @param o string
 * @return equal or not
 */
bool KSNumber::isEqual(KSObject *o) const {
  if(o == NULL || o->getTypeID() != KSNumberTypeID || ((KSNumber *)o)->_number == NULL) return false;
  return (CFNumberCompare(_number, ((KSNumber *)o)->_number, 0) == kCFCompareEqualTo);
}

/**
 * Obtain this object's hash code
 * 
 * @return hash code
 */
CFHashCode KSNumber::getHashCode(void) const {
  return CFHash(_number);
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSNumber::copyDescription(void) const {
  return CFStringCreateWithFormat(NULL, NULL, CFSTR("%@"), _number);
}


