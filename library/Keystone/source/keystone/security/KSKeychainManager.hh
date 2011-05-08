// 
// $Id: KSKeychainManager.hh 59 2010-01-23 05:44:48Z brian $
// Keystone Framework
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

#if !defined(__KEYSTONE_KSKEYCHAINMANAGER__)
#define __KEYSTONE_KSKEYCHAINMANAGER__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <Security/Security.h>

#include "KSObject.hh"
#include "KSTypes.hh"
#include "KSStatus.hh"
#include "KSError.hh"

#define KEYSTONE_CLASS_KSKeychainManager "KSKeychainManager"

#define gKeychainManager (KSKeychainManager::getShared())

#define kKSKeychainAttributeProtocolKey         CFSTR("KSKeychainAttributeProtocolKey")
#define kKSKeychainAttributeAccountNameKey      CFSTR("KSKeychainAttributeAccountNameKey")
#define kKSKeychainAttributeServerNameKey       CFSTR("KSKeychainAttributeServerNameKey")
#define kKSKeychainAttributeSecurityDomainKey   CFSTR("KSKeychainAttributeSecurityDomainKey")
#define kKSKeychainAttributePortKey             CFSTR("KSKeychainAttributePortKey")
#define kKSKeychainAttributePathKey             CFSTR("KSKeychainAttributePathKey")
#define kKSKeychainAttributeAuthenticationType  CFSTR("KSKeychainAttributeAuthenticationType")

#define kKSKeychainManagerPromptPrefix          "\x1b\x5b 1;31m[\u2192]\x1b\x5b m "

/**
 * Keychain manager
 * 
 * @author Brian William Wolter &lt;<a href="mailto:brian@woltergroup.net">brian@woltergroup.net</a>&gt;
 */
class KSKeychainManager : public KSObject {
private:
  KSKeychainManager();
  
public:
  virtual ~KSKeychainManager();
  
  static KSKeychainManager * getShared(void);
  
  static SecProtocolType getProtocolForURLScheme(CFStringRef scheme);
  
  SecKeychainItemRef copyInternetPassword(CFStringRef account, CFURLRef url);
  SecKeychainItemRef copyInternetPassword(SecProtocolType protocol, CFStringRef account, CFStringRef host, CFStringRef path = NULL, SecAuthenticationType authtype = kSecAuthenticationTypeAny);
  SecKeychainItemRef copyInternetPassword(CFDictionaryRef attributes, char **password = NULL, UInt32 *pwlength = NULL);
  
  KSStatus unlockKeychainInteractive(void) const;
  KSStatus collectPasswordInteractive(CFStringRef prompt, char **password = NULL, UInt32 *pwlength = NULL);
  KSStatus grantApplicationAccess(SecKeychainItemRef item, CFStringRef path, CFStringRef label);
  KSStatus copyInternetPasswordInteractive(CFDictionaryRef attributes, char **password, UInt32 *pwlength, bool askIfMissing = true, bool storeIfMissing = true, CFStringRef prompt = NULL);
  
  virtual CFStringRef copyDescription(void) const;
  
};

#endif __KEYSTONE_KSKEYCHAINMANAGER__

