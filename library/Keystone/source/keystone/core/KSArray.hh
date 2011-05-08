// 
// $Id: KSArray.hh 24 2008-12-20 21:15:53Z brian $
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

#if !defined(__KEYSTONE_KSARRAY__)
#define __KEYSTONE_KSARRAY__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSObject.hh"
#include "KSCoreFoundationWrapper.hh"
#include "KSTypes.hh"

#define KEYSTONE_CLASS_KSArray "KSArray"

/**
 * An array.  A Keystone wrapper around the CFArray type.
 * 
 * @author Brian William Wolter &lt;<a href="mailto:brian@woltergroup.net">brian@woltergroup.net</a>&gt;
 */
class KSArray : public KSObject, KSCoreFoundationWrapper {
protected:
  CFMutableArrayRef _array;
  
public:
  KSArray();
  KSArray(CFArrayRef array);
  KSArray(KSArray *array);
  virtual ~KSArray();
  
  static KSArray * createWithObjects(KSObject *o1, ...);
  
  virtual unsigned long getTypeID() const { return KSArrayTypeID; }
  
  KSObject * getObjectAtIndex(KSIndex i) const;
  KSObject * getFirstObject(void) const;
  KSObject * getLastObject(void) const;
  KSLength getCount(void) const;
  
  KSArray * createByAppendingObject(KSObject *o) const;
  KSArray * copy(void) const;
  
  virtual CFTypeRef getCFTypeRef(void) const;
  
  virtual bool isEqual(KSArray *o) const;
  virtual CFHashCode getHashCode(void) const;
  virtual CFStringRef copyDescription(void) const;
  
};

extern const KSArray *kEmptyKSArray;

#endif __KEYSTONE_KSARRAY__


