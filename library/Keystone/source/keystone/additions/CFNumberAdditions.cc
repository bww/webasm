// 
// $Id: CFNumberAdditions.cc 17 2008-12-08 07:26:24Z brian $
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

#include "CFNumberAdditions.hh"
#include "KSTypes.hh"
#include "KSLog.hh"

/**
 * Create a copy of a number
 * 
 * @param number number to copy
 * @return copy
 */
CFNumberRef CFNumberCreateCopy(CFNumberRef number) {
  if(number == NULL) return NULL;
  
  CFNumberType type = CFNumberGetType(number);
  CFNumberRef duplicate = NULL;
  
  switch(type){
    
    case kCFNumberSInt8Type: {
      SInt8 value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberSInt16Type: {
      SInt16 value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberSInt32Type: {
      SInt32 value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberSInt64Type: {
      SInt64 value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberFloatType:
    case kCFNumberFloat32Type: {
      float value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberDoubleType:
    case kCFNumberFloat64Type: {
      double value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberCharType: {
      char value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberShortType: {
      short value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberIntType: {
      int value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberLongType: {
      long value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberLongLongType: {
      long long value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
    case kCFNumberCFIndexType: {
      CFIndex value;
      CFNumberGetValue(number, type, &value);
      duplicate = CFNumberCreate(CFGetAllocator(number), type, &value);
    }
    break;
    
  }
  
  return duplicate;
}

/**
 * Create a number with a boolean
 * 
 * @param alloc allocator
 * @param b boolean
 * @return number
 */
CFNumberRef CFNumberCreateWithBoolean(CFAllocatorRef alloc, bool b) {
  SInt8 value = (SInt8)b;
  return CFNumberCreate(NULL, kCFNumberSInt8Type, &value);
}

/**
 * Obtain a number value as a boolean
 * 
 * @param number the number
 * @return boolean value
 */
bool CFNumberGetBoolean(CFNumberRef number) {
  SInt8 value;
  CFNumberGetValue(number, kCFNumberSInt8Type, &value);
  return (bool)value;
}

/**
 * Create a number with a character
 * 
 * @param alloc allocator
 * @param c character
 * @return number
 */
CFNumberRef CFNumberCreateWithCharacter(CFAllocatorRef alloc, char c) {
  return CFNumberCreate(NULL, kCFNumberCharType, &c);
}

/**
 * Obtain a number value as a character
 * 
 * @param number the number
 * @return character value
 */
char CFNumberGetCharacter(CFNumberRef number) {
  char value;
  CFNumberGetValue(number, kCFNumberCharType, &value);
  return value;
}


