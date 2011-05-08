// 
// $Id: KSString.cc 82 2010-08-12 21:58:20Z brian $
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

#include "KSString.hh"
#include "KSLog.hh"
#include "CFStringAdditions.hh"

#define kKSStringFormatMarker '%'

static const char kKSStringFormatSpecifiers[] = {
  '%', 'd', 'i', 'o', 'u', 'x', 'c', 'f', 'F', 'e', 'E', 'g', 'G', 'a', 'A', 's', 'p', '@', 'K', 'k',
  NULL
};

/**
 * Create with format
 */
CFStringRef KSStringCreateWithFormatAndArguments(KSStringFormatOptions options, const char *format, va_list ap) {
  CFMutableStringRef string = CFStringCreateMutable(NULL, 0);
  const char *fp;
  size_t buflen = 512;
  char buffer[buflen];
  size_t speclen = 32;
  char spec[speclen];
  CFStringRef display;
  
  for(fp = format; *fp; ){
    UniChar pct = '%';
    UniChar uc;
    register int i;
    char fc = 0;
    char *sp = spec;
    
    switch(*fp){
      case kKSStringFormatMarker:
        *sp++ = kKSStringFormatMarker;
        
        for(i = 1; i < speclen - 1 && *fp != 0; i++){
          char sc = *(++fp);
          char cc;
          
          register int j;
          for(j = 0; (cc = kKSStringFormatSpecifiers[j]) != 0; j++){
            if(cc == sc){
              *sp++ = fc = sc;
              goto done;
            }
          }
          
          *sp++ = sc;
          continue;
          
          done: fp++; break;
        }
        
        *sp++ = 0;
        
        const char *cstring;
        CFTypeRef cftype;
        KSObject *ksobject;
        
        switch(fc){
          case '%':
            CFStringAppendCharacters(string, (const UniChar *)&pct, 1);
            break;
          case 'd':
          case 'i':
          case 'c':
            snprintf(buffer, buflen - 1, spec, va_arg(ap, int));
            CFStringAppendCString(string, buffer, kDefaultStringEncoding);
            break;
          case 'o':
          case 'u':
          case 'x':
          case 'X':
            snprintf(buffer, buflen - 1, spec, va_arg(ap, unsigned int));
            CFStringAppendCString(string, buffer, kDefaultStringEncoding);
            break;
          case 'f':
          case 'F':
          case 'e':
          case 'E':
          case 'g':
          case 'G':
          case 'a':
          case 'A':
            snprintf(buffer, buflen - 1, spec, va_arg(ap, double));
            CFStringAppendCString(string, buffer, kDefaultStringEncoding);
            break;
          case 's':
            if((cstring = va_arg(ap, const char *)) != NULL)
              CFStringAppendCString(string, cstring, kDefaultStringEncoding);
            else
              CFStringAppend(string, CFSTR("<null>"));
            break;
          case 'p':
            snprintf(buffer, buflen - 1, spec, va_arg(ap, void *));
            CFStringAppendCString(string, buffer, kDefaultStringEncoding);
            break;
          case '@':
            if((cftype = va_arg(ap, CFTypeRef)) != NULL){
              if(CFGetTypeID(cftype) == CFStringGetTypeID()){
                CFStringAppend(string, (CFStringRef)cftype);
              }else{
                CFStringAppendFormat(string, NULL, CFSTR("%@"), cftype);
              }
            }else{
              CFStringAppend(string, CFSTR("<null>"));
            }
            break;
          case 'k':
          case 'K':
            if((ksobject = va_arg(ap, KSObject *)) != NULL){
              display = ksobject->copyDescription();
              CFStringAppend(string, display);
              if(display) CFRelease(display);
            }else{
              CFStringAppend(string, CFSTR("<null>"));
            }
            break;
        }
        break;
      default:
        uc = (UniChar)*fp++;
        CFStringAppendCharacters(string, (const UniChar *)&uc, 1);
        break;
    }
    
  }
  
  return string;
}

KSString * KSStringGetConstant(const char *string) {
  static CFMutableDictionaryRef __cstrings = NULL;
  if(string == NULL) return NULL;
  KSString *cstring = NULL;
  
  if(__cstrings == NULL){
    __cstrings = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kKSObjectCFDictionaryValueCallBacks);
  }
  
  CFStringRef key = CFStringCreateWithCString(NULL, string, kDefaultStringEncoding);
  SInt64 hvalue = (SInt64)CFHash(key);
  if(key) CFRelease(key);
  
  CFNumberRef hash = CFNumberCreate(NULL, kCFNumberSInt64Type, &hvalue);
  if((cstring = (KSString *)CFDictionaryGetValue(__cstrings, hash)) == NULL){
    cstring = new KSString(string);
    CFDictionarySetValue(__cstrings, hash, cstring);
    cstring->release(); // don't use KSRelease (which will null the pointer)
  }
  if(hash) CFRelease(hash);
  
  return cstring;
}

/**
 * Construct
 */
KSString::KSString() {
  _string = CFSTR("");
}

/**
 * Construct
 * 
 * @param string C string
 */
KSString::KSString(const char *string) {
  _string = (string != NULL) ? CFStringCreateWithCString(NULL, string, kDefaultStringEncoding) : CFSTR("");
}

/**
 * Construct with format
 * 
 * @param options format options
 * @param format format
 * @param ... arguments
 */
KSString::KSString(KSStringFormatOptions options, const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  _string = KSStringCreateWithFormatAndArguments(options, format, ap);
  va_end(ap);
}

/**
 * Construct
 * 
 * @param options format options
 * @param format format
 * @param ap argument list
 */
KSString::KSString(KSStringFormatOptions options, const char *format, va_list ap) {
  _string = KSStringCreateWithFormatAndArguments(options, format, ap);
}

/**
 * Construct
 * 
 * @param string a string
 */
KSString::KSString(CFStringRef string) {
  _string = (string != NULL) ? CFStringCreateCopy(NULL, string) : CFSTR("");
}

/**
 * Clean up
 */
KSString::~KSString() {
  if(_string) CFRelease(_string);
}

/**
 * Obtain the string character length
 * 
 * @return character length
 */
KSLength KSString::getLength(void) const {
  return CFStringGetLength(_string);
}

/**
 * Obtain the character at a specified index.  If no such index exists
 * behavior is undefined.
 * 
 * @param i index
 * @return character
 */
UniChar KSString::getCharacterAtIndex(KSIndex i) const {
  return CFStringGetCharacterAtIndex(_string, i);
}

/**
 * Parse an integer value from the string
 * 
 * @return integer value
 */
int KSString::getIntegerValue(void) const {
  return CFStringGetIntValue(_string);
}

/**
 * Parse a double value from the string
 * 
 * @return double value
 */
double KSString::getDoubleValue(void) const {
  return CFStringGetDoubleValue(_string);
}

/**
 * Create a new string by appending the parameter string's value to this
 * string.
 * 
 * @param string string to append
 * @return the new string
 */
KSString * KSString::createByAppendingString(KSString *string) {
  if(string == NULL) return NULL;
  CFMutableStringRef buffer = CFStringCreateMutable(NULL, 0);
  CFStringAppend(buffer, _string);
  CFStringAppend(buffer, string->_string);
  return new KSString(buffer);
}

/**
 * Create a copy of this string
 * 
 * @return copy
 */
KSString * KSString::copy(void) {
  CFStringRef cstr = CFStringCreateCopy(NULL, _string);
  KSString *kstr = new KSString(cstr);
  if(cstr) CFRelease(cstr);
  return kstr;
}

/**
 * Obtain the underlying CFType.  This type is owned by the wrapper class.
 * 
 * @return CFType
 */
CFTypeRef KSString::getCFTypeRef(void) const {
  return _string;
}

/**
 * Compare strings
 * 
 * @param s string
 * @return comparison result
 */
KSComparisonResult KSString::compare(KSString *s) const {
  return (KSComparisonResult)CFStringCompare(_string, s->_string, 0);
}

/**
 * Compare strings
 * 
 * @param s string
 * @param range range of characters to compare
 * @return comparison result
 */
KSComparisonResult KSString::compare(KSString *s, KSRange range) const {
  return (KSComparisonResult)CFStringCompareWithOptions(_string, s->_string, KSRangeToCFRange(range), 0);
}

/**
 * Determine semantic equality
 * 
 * @param o string
 * @return equal or not
 */
bool KSString::isEqual(KSObject *o) const {
  if(o == NULL || o->getTypeID() != KSStringTypeID || ((KSString *)o)->_string == NULL) return false;
  return (CFStringCompare(_string, ((KSString *)o)->_string, 0) == kCFCompareEqualTo);
}

/**
 * Obtain this object's hash code
 * 
 * @return hash code
 */
CFHashCode KSString::getHashCode(void) const {
  return CFHash(_string);
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSString::copyDescription(void) const {
  return CFStringCreateCopy(NULL, _string);
}


