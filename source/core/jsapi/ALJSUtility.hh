// 
// $Id: ALJSUtility.hh 101 2010-01-29 18:10:32Z brian $
// Web Assembler
// 
// Copyright (c) 2010 Wolter Group New York, Inc., All rights reserved.
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

#if !defined(__ALJSUTILITY__)
#define __ALJSUTILITY__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <JavaScriptCore/JavaScriptCore.h>
#include <JavaScriptCore/JSObjectRef.h>

#if !defined(MAC_OS_X_VERSION_10_6)
# define MAC_OS_X_VERSION_10_6 1060
#endif

#if (MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_6)
# define ALJSMakeException(j, v, m, l) {  \
  KSLogError("JavaScript: %@", m);        \
  goto l;                                 \
}
#else
# define ALJSMakeException(j, v, m, l) {                                                                \
  JSStringRef __ALJSMakeException_message__ = JSStringCreateWithCFString(m);                            \
  JSValueRef __ALJSMakeException_vmessage__ = JSValueMakeString(j, __ALJSMakeException_message__);      \
  JSStringRelease(__ALJSMakeException_message__);                                                       \
  JSValueRef __ALJSMakeException_argv__[] = { __ALJSMakeException_vmessage__ };                         \
  JSValueRef __ALJSMakeException_error__;                                                               \
  if((__ALJSMakeException_error__ = JSObjectMakeError(j, 1, __ALJSMakeException_argv__, &v)) == NULL){  \
    goto l;                                                                                             \
  }else{                                                                                                \
    v = __ALJSMakeException_error__;                                                                    \
  }                                                                                                     \
}
#endif

#endif //__ALJSUTILITY__

