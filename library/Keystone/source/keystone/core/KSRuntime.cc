// 
// $Id: KSRuntime.cc 19 2008-12-15 02:36:18Z brian $
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

#include "KSRuntime.hh"
#include "KSRuntimePrivate.hh"
#include "KSRuntimeType.hh"
#include "KSObject.hh"
#include "KSLog.hh"

#define KSRuntimeRegisterTypeSpecific(rt, tid, tname) __KSRuntimeRegisterTypeSpecific(rt, tid, CFSTR(#tname), tname::newInstance)

/**
 * Obtain the shared runtime
 * 
 * @return shared runtime
 */
__KSRuntime * __KSRuntimeGetShared(void) {
  return __KSRuntime::getShared();
}

/**
 * Register a runtime type
 * 
 * @param id type identifier
 * @param name type name
 * @param creator type default creator
 */
void __KSRuntimeRegisterType(unsigned long id, CFStringRef name, KSObjectCreateInstance creator) {
  __KSRuntimeRegisterTypeSpecific(__KSRuntimeGetShared(), id, name, creator);
}

/**
 * Register a runtime type with a specific runtime
 * 
 * @param runtime the runtime
 * @param id type identifier
 * @param name type name
 * @param creator type default creator
 */
void __KSRuntimeRegisterTypeSpecific(__KSRuntime *runtime, unsigned long id, CFStringRef name, KSObjectCreateInstance creator) {
  runtime->registerType(id, name, creator);
}

/**
 * Create a new instance of the specified type.
 * 
 * @param name type name
 * @return new instance
 */
KSObject * __KSRuntimeCreateInstanceWithTypeName(CFStringRef name) {
  KSRuntimeType *type;
  if((type = __KSRuntimeGetShared()->getRegisteredType(name)) != NULL){
    return type->createRuntimeTypeInstance();
  }else{
    return NULL;
  }
}

/**
 * Create a new instance of the specified type.
 * 
 * @param id type identifier
 * @return new instance
 */
KSObject * __KSRuntimeCreateInstanceWithTypeID(KSTypeID id) {
  KSRuntimeType *type;
  if((type = __KSRuntimeGetShared()->getRegisteredType(id)) != NULL){
    return type->createRuntimeTypeInstance();
  }else{
    return NULL;
  }
}

/**
 * Internal constructor
 */
__KSRuntime::__KSRuntime() {
  
  // setup the types map
  _ntypes = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kKSObjectCFDictionaryValueCallBacks);
  _vtypes = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kKSObjectCFDictionaryValueCallBacks);
  
  // initialize types
  KSRuntimeRegisterTypeSpecific(this, KSObjectTypeID, KSObject);
  
}

/**
 * Clean up
 */
__KSRuntime::~__KSRuntime() {
  if(_ntypes) CFRelease(_ntypes);
  if(_vtypes) CFRelease(_vtypes);
}

/**
 * Obtain the shared, singleton runtime
 * 
 * @return shared runtime
 */
__KSRuntime * __KSRuntime::getShared(void) {
  static __KSRuntime *shared = NULL;
  if(shared == NULL) shared = new __KSRuntime();
  return shared;
}

/**
 * Register a runtime type
 * 
 * @param id type identifier
 * @param name type name
 * @param creator type creator factory method
 */
void __KSRuntime::registerType(unsigned long id, CFStringRef name, KSObjectCreateInstance creator) {
  KSRuntimeType *type = new KSRuntimeType(id, name, creator);
  CFNumberRef nid = CFNumberCreate(NULL, kCFNumberLongType, &id);
  
  CFDictionarySetValue(_ntypes, name, type);
  CFDictionarySetValue(_vtypes, nid, type);
  
  CFRelease(nid);
  KSRelease(type);
}

/**
 * Obtain a registered type by name
 * 
 * @param name type name
 * @return type
 */
KSRuntimeType * __KSRuntime::getRegisteredType(CFStringRef name) const {
  return (KSRuntimeType *)CFDictionaryGetValue(_ntypes, name);
}

/**
 * Obtain a registered type by identifier
 * 
 * @param id type identifier
 * @return type
 */
KSRuntimeType * __KSRuntime::getRegisteredType(KSTypeID id) const {
  CFNumberRef nid = CFNumberCreate(NULL, kCFNumberLongType, &id);
  KSRuntimeType *type = (KSRuntimeType *)CFDictionaryGetValue(_vtypes, nid);
  CFRelease(nid);
  return type;
}


