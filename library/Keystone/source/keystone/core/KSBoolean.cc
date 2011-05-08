// 
// $Id: KSBoolean.cc 21 2008-12-15 03:45:09Z brian $
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

#include "KSBoolean.hh"

extern const KSBoolean *kKSTrue   = (KSBoolean *)KSMakeConstant(new KSBoolean(true));
extern const KSBoolean *kKSFalse  = (KSBoolean *)KSMakeConstant(new KSBoolean(false));

/**
 * Construct
 * 
 * @param b boolean value
 */
KSBoolean::KSBoolean(bool b) {
  _boolean = b;
}

/**
 * Clean up
 */
KSBoolean::~KSBoolean() {
}

/**
 * Obtain the boolean value
 * 
 * @return boolean value
 */
bool KSBoolean::getBooleanValue(void) const {
  return _boolean;
}

/**
 * Determine if the value is true
 * 
 * @return true or not
 */
bool KSBoolean::isTrue(void) const {
  return _boolean;
}

/**
 * Determine semantic equality
 * 
 * @param o string
 * @return equal or not
 */
bool KSBoolean::isEqual(KSObject *o) const {
  if(o == NULL || o->getTypeID() != KSBooleanTypeID) return false;
  return (((KSBoolean *)o)->_boolean == _boolean);
}

/**
 * Obtain this object's hash code
 * 
 * @return hash code
 */
CFHashCode KSBoolean::getHashCode(void) const {
  return (int)_boolean;
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSBoolean::copyDescription(void) const {
  return (_boolean) ? CFSTR("true") : CFSTR("false");
}


