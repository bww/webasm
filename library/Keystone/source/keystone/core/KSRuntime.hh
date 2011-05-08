// 
// $Id: KSRuntime.hh 74 2010-01-29 14:19:03Z brian $
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

#if !defined(__KEYSTONE_KSRUNTIME__)
#define __KEYSTONE_KSRUNTIME__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSObjectTypes.hh"
#include "KSTypes.hh"

#if defined(__GNUC__) && (__GNUC__ >= 3) && (__GNUC_MINOR__ >= 1)
#define KEYSTONE_DEPRECATED __attribute__((deprecated))
#define KEYSTONE_DEPRECATED_WITH_COMMENT(c) __attribute__((deprecated))
#else
#define KEYSTONE_DEPRECATED
#endif /* __GNUC__ */

#if defined(__GNUC__) && (__GNUC__ >= 3) && (__GNUC_MINOR__ >= 1)
#define KEYSTONE_WEAK_IMPORT __attribute__((weak_import))
#else
#define KEYSTONE_WEAK_IMPORT
#endif

#if defined(__GNUC__) && (__GNUC__ >= 3) && (__GNUC_MINOR__ >= 1)
#define KEYSTONE_UNAVAILABLE __attribute__((unavailable))
#else
#define KEYSTONE_UNAVAILABLE
#endif

class __KSRuntime;
class   KSRuntimeType;
class   KSObject;

typedef KSObject * (*KSObjectCreateInstance)(void);

__KSRuntime * __KSRuntimeGetShared(void);

void __KSRuntimeRegisterType(unsigned long id, CFStringRef name, KSObjectCreateInstance creator);
void __KSRuntimeRegisterTypeSpecific(__KSRuntime *runtime, unsigned long id, CFStringRef name, KSObjectCreateInstance creator);

KSObject * __KSRuntimeCreateInstanceWithTypeName(CFStringRef name);
KSObject * __KSRuntimeCreateInstanceWithTypeID(KSTypeID id);

#define KSRuntimeRegisterType(tid, tname)   __KSRuntimeRegisterType(tid, CFSTR(#tname), tname::newInstance)
#define KSTypeCreateWithName(name)          __KSRuntimeCreateInstanceWithTypeName(name)
#define KSTypeCreateWithTypeID(tid)         __KSRuntimeCreateInstanceWithTypeID(tid)
#define KSRuntime                           __KSRuntimeGetShared()

#endif __KEYSTONE_KSRUNTIME__

