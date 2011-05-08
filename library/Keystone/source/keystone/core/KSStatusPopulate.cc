// WARNING: THIS FILE IS GENERATED.  DO NOT MAKE MANUAL CHANGES; THEY WILL BE
// OVERWRITTEN THE NEXT TIME THE PROJECT IS BUILT.
// 
// $Id$
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

#include <CoreFoundation/CoreFoundation.h>

CFMutableDictionaryRef __KSStatusPopulateMessageMap(CFMutableDictionaryRef map) {
  CFNumberRef keyVal;
  SInt32 value;
  
  // KSStatusOk (0)
  // 
  value = 0;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("No Error"));
  CFRelease(keyVal);

  // KSStatusError (-1)
  // 
  value = -1;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Undefined error"));
  CFRelease(keyVal);

  // KSStatusPermissionDenied (-2)
  // 
  value = -2;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Permission denied"));
  CFRelease(keyVal);

  // KSStatusQuotaExceeded (-3)
  // 
  value = -3;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("File system quota exceeded"));
  CFRelease(keyVal);

  // KSStatusResourceExists (-4)
  // 
  value = -4;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("File or resource already exists"));
  CFRelease(keyVal);

  // KSStatusInterrupted (-5)
  // 
  value = -5;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Operation was interrupted by a signal"));
  CFRelease(keyVal);

  // KSStatusInvalidOperand (-6)
  // 
  value = -6;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Invalid operand(s)"));
  CFRelease(keyVal);

  // KSStatusIOError (-7)
  // 
  value = -7;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("An I/O error occured"));
  CFRelease(keyVal);

  // KSStatusInvalidResource (-8)
  // 
  value = -8;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Operation is attempting to operate on an invalid resource type"));
  CFRelease(keyVal);

  // KSStatusTooManyLinks (-9)
  // 
  value = -9;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Too many symbolic links"));
  CFRelease(keyVal);

  // KSStatusTooManyOpenFiles (-10)
  // 
  value = -10;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Too many open files"));
  CFRelease(keyVal);

  // KSStatusPathTooLong (-11)
  // 
  value = -11;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("The path (or one of its components) is too long"));
  CFRelease(keyVal);

  // KSStatusFileTableFull (-12)
  // 
  value = -12;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("The system file table is full"));
  CFRelease(keyVal);

  // KSStatusNoSuchResource (-13)
  // 
  value = -13;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("No such file or resource"));
  CFRelease(keyVal);

  // KSStatusBadFileDescriptor (-14)
  // 
  value = -14;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Bad file descriptor"));
  CFRelease(keyVal);

  // KSStatusStreamClosed (-15)
  // 
  value = -15;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Stream already closed"));
  CFRelease(keyVal);

  // KSStatusNoSuchDevice (-16)
  // 
  value = -16;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("No such device"));
  CFRelease(keyVal);

  // KSStatusConnectionReset (-17)
  // 
  value = -17;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Socket connection reset by peer"));
  CFRelease(keyVal);

  // KSStatusNotConnected (-18)
  // 
  value = -18;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Socket is not connected"));
  CFRelease(keyVal);

  // KSStatusTimedOut (-19)
  // 
  value = -19;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Socket transmission timed out"));
  CFRelease(keyVal);

  // KSStatusNodeResolutionFailed (-20)
  // 
  value = -20;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Node resolution failed"));
  CFRelease(keyVal);

  // KSStatusInvalidState (-21)
  // 
  value = -21;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Current state is invalid"));
  CFRelease(keyVal);

  // KSStatusWouldBlock (-22)
  // 
  value = -22;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Would block"));
  CFRelease(keyVal);

  // KSStatusBusy (-23)
  // 
  value = -23;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Operation cannot be completed because resource is busy"));
  CFRelease(keyVal);

  // KSStatusInfiniteLoop (-24)
  // 
  value = -24;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("An infinite loop was detected"));
  CFRelease(keyVal);

  // KSStatusConnectionRefused (-25)
  // 
  value = -25;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("The remote host refused the connection"));
  CFRelease(keyVal);

  // KSStatusProtocolError (-300)
  // 
  value = -300;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Protocol error or unexpected reply from remote host"));
  CFRelease(keyVal);

  // KSStatusTrustNotAvailable (-400)
  // 
  value = -400;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Trust not available"));
  CFRelease(keyVal);

  // KSStatusCertificateChainInvalid (-401)
  // 
  value = -401;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Invalid certificate chain"));
  CFRelease(keyVal);

  // KSStatusCertificateExpired (-402)
  // 
  value = -402;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Certificate expired"));
  CFRelease(keyVal);

  // KSStatusUnimplemented (-500)
  // 
  value = -500;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Not implemented / unsupported"));
  CFRelease(keyVal);

  // KSStatusInvalidParameter (-501)
  // 
  value = -501;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Invalid parameter"));
  CFRelease(keyVal);

  // KSStatusTypeError (-502)
  // 
  value = -502;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Invalid type for operation"));
  CFRelease(keyVal);

  // KSStatusNullPointer (-503)
  // 
  value = -503;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Invalid null pointer encountered"));
  CFRelease(keyVal);

  // KSStatusNoSuchMethod (-504)
  // 
  value = -504;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("No such method"));
  CFRelease(keyVal);

  // KSStatusRuntimeError (-505)
  // 
  value = -505;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("Runtime error"));
  CFRelease(keyVal);

  // KSStatusNoSuchOption (-1000)
  // 
  value = -1000;
  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);
  CFDictionaryAddValue(map, keyVal, CFSTR("No such option"));
  CFRelease(keyVal);

  
  return map;
}
