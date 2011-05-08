// 
// $Id: KSString.hh 44 2010-01-19 04:43:28Z brian $
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

#if !defined(__KEYSTONE_KSSTRING__)
#define __KEYSTONE_KSSTRING__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSObject.hh"
#include "KSCoreFoundationWrapper.hh"
#include "KSTypes.hh"

#include <stdarg.h>

#define KEYSTONE_CLASS_KSString "KSString"

#define KSSTR(s) (KSStringGetConstant(s))

typedef enum KSStringFormatOptionMask_ {
  KSStringFormatOptionNone    = 0
} KSStringFormatOptionMask;

typedef int KSStringFormatOptions;

/**
 * A string.  A Keystone wrapper around the CFString type.
 * 
 * @author Brian William Wolter &lt;<a href="mailto:brian@woltergroup.net">brian@woltergroup.net</a>&gt;
 */
class KSString : public KSObject, KSCoreFoundationWrapper {
private:
  CFStringRef             _string;
  
public:
  KSString();
  KSString(const char *string);
  KSString(KSStringFormatOptions options, const char *format, ...);
  KSString(KSStringFormatOptions options, const char *format, va_list ap);
  KSString(CFStringRef string);
  virtual ~KSString();
  
  virtual unsigned long getTypeID() const { return KSStringTypeID; }
  
  KSLength getLength(void) const;
  UniChar getCharacterAtIndex(KSIndex i) const;
  
  int getIntegerValue(void) const;
  double getDoubleValue(void) const;
  
  KSString * createByAppendingString(KSString *string);
  KSString * copy(void);
  
  virtual CFTypeRef getCFTypeRef(void) const;
  KSComparisonResult compare(KSString *s) const;
  KSComparisonResult compare(KSString *s, KSRange range) const;
  
  virtual bool isEqual(KSObject *o) const;
  virtual CFHashCode getHashCode(void) const;
  virtual CFStringRef copyDescription(void) const;
  
};

KSString * KSStringGetConstant(const char *string);

#endif __KEYSTONE_KSSTRING__


