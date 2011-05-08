// 
// $Id: KSMutableArray.hh 24 2008-12-20 21:15:53Z brian $
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

#if !defined(__KEYSTONE_KSMUTABLEARRAY__)
#define __KEYSTONE_KSMUTABLEARRAY__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSArray.hh"
#include "KSTypes.hh"

#define KEYSTONE_CLASS_KSMutableArray "KSMutableArray"

/**
 * An mutable array.
 * 
 * @author Brian William Wolter &lt;<a href="mailto:brian@woltergroup.net">brian@woltergroup.net</a>&gt;
 */
class KSMutableArray : public KSArray {
public:
  KSMutableArray();
  KSMutableArray(KSArray *array);
  virtual ~KSMutableArray();
  
  virtual unsigned long getTypeID() const { return KSMutableArrayTypeID; }
  
  void addObject(KSObject *object);
  void addObjectsFromArray(KSArray *array);
  void addObjectAtIndex(KSIndex i, KSObject *object);
  void setObjectAtIndex(KSIndex i, KSObject *object);
  void removeObjectAtIndex(KSIndex i);
  
  virtual CFStringRef copyDescription(void) const;
  
};

#endif __KEYSTONE_KSMUTABLEARRAY__


