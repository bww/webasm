// 
// $Id: KSRuntimePrivate.hh 14 2008-11-27 04:59:35Z brian $
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

#if !defined(__KEYSTONE_KSRUNTIME_PRIVATE__)
#define __KEYSTONE_KSRUNTIME_PRIVATE__ 1

#include <CoreFoundation/CoreFoundation.h>
#include "KSRuntime.hh"

/**
 * The Keystone runtime.  Only one singleton instance of this class may exist
 * at any time.  This instnace is retrieved via the getShared() method, or as
 * a convenience, the KSRuntime macro, which evaluates to the same.
 * 
 * @author Brian William Wolter &lt;<a href="mailto:brian@woltergroup.net">brian@woltergroup.net</a>&gt;
 */
class __KSRuntime {
private:
  CFMutableDictionaryRef  _ntypes;
  CFMutableDictionaryRef  _vtypes;
  
protected:
  __KSRuntime();
  
public:
  virtual ~__KSRuntime();
  
  static __KSRuntime * getShared(void);
  
  void registerType(unsigned long id, CFStringRef name, KSObjectCreateInstance creator);
  KSRuntimeType * getRegisteredType(CFStringRef name) const;
  KSRuntimeType * getRegisteredType(KSTypeID id) const;
  
};

#endif __KEYSTONE_KSRUNTIME_PRIVATE__

