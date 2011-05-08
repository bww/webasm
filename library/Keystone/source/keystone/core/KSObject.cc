// 
// $Id: KSObject.cc 19 2008-12-15 02:36:18Z brian $
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

#include "KSObject.hh"
#include "KSLog.hh"

/**
 * Make an object a constant.  Constant objects ignore retain and release
 * calls and therefore are never deallocated (unless the delete operator
 * is explicitly invoked, which it never ever should be).
 * 
 * @param object the object
 * @return the object
 */
KSObject * KSMakeConstant(KSObject *object) {
  if(object != NULL) object->_constant = true;
  return object;
}

/**
 * Construct
 */
KSObject::KSObject() {
  _refcount = 1;
  _constant = false;
}

/**
 * Clean up
 */
KSObject::~KSObject() {
}

/**
 * Retain this object
 * 
 * @return this object
 */
KSObject * KSObject::retain(void) const {
  if(!_constant) _refcount++;
  return (KSObject *)this;
}

/**
 * Release this object
 */
void KSObject::release(void) const {
  if(_constant) return;
  if(--_refcount < 1) delete this;
}

/**
 * Determine equality
 * 
 * @param o candidate object
 * @return equal or not
 */
bool KSObject::isEqual(KSObject *o) const {
  return (o == this);
}

/**
 * Obtain a hash code for this object (pointer value)
 * 
 * @return hash code
 */
CFHashCode KSObject::getHashCode(void) const {
  return (CFHashCode)this;
}

/**
 * Copy a string description of this object
 * 
 * @return string description
 */
CFStringRef KSObject::copyDescription(void) const {
  return CFStringCreateWithFormat(NULL, NULL, CFSTR("[KSObject %p]"), this);
}

// 
// Memory management callbacks
// 

const void * KSObjectDefaultRetainCallBack(CFAllocatorRef a, const void *v) {
  return (const void *)(((KSObject *)v)->retain());
}

void KSObjectDefaultReleaseCallBack(CFAllocatorRef a, const void *v) {
  ((KSObject *)v)->release();
}

CFStringRef KSObjectDefaultCopyDescriptionCallBack(const void *v) {
  return ((KSObject *)v)->copyDescription();
}

Boolean KSObjectDefaultEqualCallBack(const void *a, const void *b) {
  return (Boolean)((KSObject *)a)->isEqual((KSObject *)b);
}

CFHashCode KSObjectDefaultHashCallBack(const void *v) {
  return ((KSObject *)v)->getHashCode();
}

/**
 * KSObject memory management callbacks for CFArray
 */
const CFArrayCallBacks kKSObjectCFArrayCallBacks = {
  0,
  KSObjectDefaultRetainCallBack,
  KSObjectDefaultReleaseCallBack,
  KSObjectDefaultCopyDescriptionCallBack,
  KSObjectDefaultEqualCallBack
};

/**
 * KSObject memory management callbacks for CFDictionary
 */
const CFDictionaryKeyCallBacks kKSObjectCFDictionaryKeyCallBacks = {
  0,
  KSObjectDefaultRetainCallBack,
  KSObjectDefaultReleaseCallBack,
  KSObjectDefaultCopyDescriptionCallBack,
  KSObjectDefaultEqualCallBack,
  KSObjectDefaultHashCallBack
};

const CFDictionaryValueCallBacks kKSObjectCFDictionaryValueCallBacks = {
  0,
  KSObjectDefaultRetainCallBack,
  KSObjectDefaultReleaseCallBack,
  KSObjectDefaultCopyDescriptionCallBack,
  KSObjectDefaultEqualCallBack
};



