// 
// $Id: KSKeychainManager.cc 59 2010-01-23 05:44:48Z brian $
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

#include "KSKeychainManager.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "CFStringAdditions.hh"

#include <pwd.h>
#include <unistd.h>

/**
 * Internal
 */
KSKeychainManager::KSKeychainManager() {
}

/**
 * Clean up
 */
KSKeychainManager::~KSKeychainManager() {
}

/**
 * Obtain the singleton keychain manager.
 * 
 * @return shared keychain manager
 */
KSKeychainManager * KSKeychainManager::getShared(void) {
  static KSKeychainManager *shared = NULL;
  if(shared == NULL) shared = new KSKeychainManager();
  return shared;
}

/**
 * Obtain a keychain protocol type corresponding to a URL scheme
 * 
 * @param scheme URL scheme
 * @return protocol type
 */
SecProtocolType KSKeychainManager::getProtocolForURLScheme(CFStringRef scheme) {
  SecProtocolType protocol;
  
  if(CFStringCompare(scheme, CFSTR("ftp"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeFTP;
  }else if(CFStringCompare(scheme, CFSTR("ftpa"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeFTPAccount;
  }else if(CFStringCompare(scheme, CFSTR("http"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeHTTP;
  }else if(CFStringCompare(scheme, CFSTR("irc"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeIRC;
  }else if(CFStringCompare(scheme, CFSTR("nntp"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeNNTP;
  }else if(CFStringCompare(scheme, CFSTR("pop3"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypePOP3;
  }else if(CFStringCompare(scheme, CFSTR("smtp"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeSMTP;
  }else if(CFStringCompare(scheme, CFSTR("sox"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeSOCKS;
  }else if(CFStringCompare(scheme, CFSTR("imap"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeIMAP;
  }else if(CFStringCompare(scheme, CFSTR("ldap"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeLDAP;
  }else if(CFStringCompare(scheme, CFSTR("atlk"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeAppleTalk;
  }else if(CFStringCompare(scheme, CFSTR("afp"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeAFP;
  }else if(CFStringCompare(scheme, CFSTR("teln"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeTelnet;
  }else if(CFStringCompare(scheme, CFSTR("ssh"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeSSH;
  }else if(CFStringCompare(scheme, CFSTR("ftps"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeFTPS;
  }else if(CFStringCompare(scheme, CFSTR("https"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeHTTPS;
  }else if(CFStringCompare(scheme, CFSTR("htpx"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeHTTPProxy;
  }else if(CFStringCompare(scheme, CFSTR("htsx"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeHTTPSProxy;
  }else if(CFStringCompare(scheme, CFSTR("ftpx"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeFTPProxy;
  }else if(CFStringCompare(scheme, CFSTR("cifs"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeCIFS;
  }else if(CFStringCompare(scheme, CFSTR("smb"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeSMB;
  }else if(CFStringCompare(scheme, CFSTR("rtsp"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeRTSP;
  }else if(CFStringCompare(scheme, CFSTR("rtsx"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeRTSPProxy;
  }else if(CFStringCompare(scheme, CFSTR("daap"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeDAAP;
  }else if(CFStringCompare(scheme, CFSTR("eppc"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeEPPC;
  }else if(CFStringCompare(scheme, CFSTR("ipp"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeIPP;
  }else if(CFStringCompare(scheme, CFSTR("nntps"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeNNTPS;
  }else if(CFStringCompare(scheme, CFSTR("ldaps"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeLDAPS;
  }else if(CFStringCompare(scheme, CFSTR("tels"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeTelnetS;
  }else if(CFStringCompare(scheme, CFSTR("imps"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeIMAPS;
  }else if(CFStringCompare(scheme, CFSTR("ircs"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeIRCS;
  }else if(CFStringCompare(scheme, CFSTR("pops"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypePOP3S;
  }else if(CFStringCompare(scheme, CFSTR("cvsp"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeCVSpserver;
  }else if(CFStringCompare(scheme, CFSTR("svn"), kCFCompareCaseInsensitive) == kCFCompareEqualTo){
    protocol = kSecProtocolTypeSVN;
  }else{
    protocol = kSecProtocolTypeAny;
  }
  
  return protocol;
}

/**
 * Find an internet password keychain item
 * 
 * @param account account name
 * @param url a URL representing the account context.  The URL scheme is
 * used to determine the service protocol.
 * @return password keychain item or NULL if no such password found
 */
SecKeychainItemRef KSKeychainManager::copyInternetPassword(CFStringRef account, CFURLRef url) {
  CFStringRef scheme = NULL;
  CFStringRef host = NULL;
  CFStringRef path = NULL;
  SecKeychainItemRef item = NULL;
  
  scheme  = CFURLCopyScheme(url);
  host    = CFURLCopyHostName(url);
  path    = CFURLCopyPath(url);
  
  item = copyInternetPassword(getProtocolForURLScheme(scheme), account, host, (path != NULL && CFStringGetLength(path) > 0) ? path : NULL);
  
  CFReleaseSafe(scheme);
  CFReleaseSafe(host);
  CFReleaseSafe(path);
  
  return item;
}

/**
 * Find an internet password keychain item
 * 
 * @param protocol service protocol
 * @param account user account name
 * @param host service host
 * @param path service path
 * @param authtype authentication type
 * @return password keychain item or NULL if no such password found
 */
SecKeychainItemRef KSKeychainManager::copyInternetPassword(SecProtocolType protocol, CFStringRef account, CFStringRef host, CFStringRef path, SecAuthenticationType authtype) {
  CFMutableDictionaryRef attributes = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  SecKeychainItemRef item = NULL;
  
  if(account)   CFDictionarySetValue(attributes, kKSKeychainAttributeAccountNameKey, account);
  if(host)      CFDictionarySetValue(attributes, kKSKeychainAttributeServerNameKey, host);
  if(path)      CFDictionarySetValue(attributes, kKSKeychainAttributePathKey, path);
  
  if(protocol){
    CFNumberRef nprotocol = CFNumberCreate(NULL, kCFNumberSInt32Type, &protocol);
    CFDictionarySetValue(attributes, kKSKeychainAttributeProtocolKey, nprotocol);
    if(nprotocol) CFRelease(nprotocol);
  }
  if(authtype){
    CFNumberRef nauthtype = CFNumberCreate(NULL, kCFNumberSInt32Type, &authtype);
    CFDictionarySetValue(attributes, kKSKeychainAttributeAuthenticationType, nauthtype);
    if(nauthtype) CFRelease(nauthtype);
  }
  
  item = copyInternetPassword(attributes);
  
  CFReleaseSafe(attributes);
  return item;
}

/**
 * Find an internet password keychain item
 * 
 * @param account account name
 * @param attributes a dictionary of attributes describing the account context
 * @param password on output, if present and successful: the password
 * @param pwlength on output, if present and successful: the password pwlength
 * @return password keychain item or NULL if no such password found
 */
SecKeychainItemRef KSKeychainManager::copyInternetPassword(CFDictionaryRef attributes, char **password, UInt32 *pwlength) {
  SecKeychainItemRef item = NULL;
  OSStatus osstatus;
  CFStringRef temp;
  UInt16 port = 0;
  SecProtocolType protocol = kSecProtocolTypeAny;
  SecAuthenticationType authtype = kSecAuthenticationTypeAny;
  
  char *ipasswd = NULL;
  UInt32 ipwlen;
  
  char *server  = ((temp = (CFStringRef)CFDictionaryGetValue(attributes, kKSKeychainAttributeServerNameKey)) != NULL) ? CFStringCopyCString(temp, kDefaultStringEncoding) : NULL;
  char *domain  = ((temp = (CFStringRef)CFDictionaryGetValue(attributes, kKSKeychainAttributeSecurityDomainKey)) != NULL) ? CFStringCopyCString(temp, kDefaultStringEncoding) : NULL;
  char *account = ((temp = (CFStringRef)CFDictionaryGetValue(attributes, kKSKeychainAttributeAccountNameKey)) != NULL) ? CFStringCopyCString(temp, kDefaultStringEncoding) : NULL;
  char *path    = ((temp = (CFStringRef)CFDictionaryGetValue(attributes, kKSKeychainAttributePathKey)) != NULL) ? CFStringCopyCString(temp, kDefaultStringEncoding) : NULL;
  
  CFNumberRef cport;
  if((cport = (CFNumberRef)CFDictionaryGetValue(attributes, kKSKeychainAttributePortKey)) != NULL){
    CFNumberGetValue(cport, kCFNumberSInt16Type, &port);
  }
  
  CFNumberRef cproto;
  if((cproto = (CFNumberRef)CFDictionaryGetValue(attributes, kKSKeychainAttributeProtocolKey)) != NULL){
    CFNumberGetValue(cproto, kCFNumberSInt32Type, &protocol);
  }
  
  CFNumberRef cauth;
  if((cauth = (CFNumberRef)CFDictionaryGetValue(attributes, kKSKeychainAttributeAuthenticationType)) != NULL){
    CFNumberGetValue(cauth, kCFNumberSInt32Type, &authtype);
  }
  
  osstatus = 
    SecKeychainFindInternetPassword(
      NULL,
      (server != NULL) ? strlen(server) : 0,
      server,
      (domain != NULL) ? strlen(domain) : 0,
      domain,
      (account != NULL) ? strlen(account) : 0,
      account,
      (path != NULL) ? strlen(path) : 0,
      path,
      port,
      protocol,
      authtype,
      &ipwlen, (void **)&ipasswd,
      &item
    );
  
  if(osstatus != noErr){
    
    switch(osstatus){
      case errSecItemNotFound:
        /* No notice */
        break;
      case errSecNotAvailable:
        KSLogError("Trust not available");
        break;
      case errSecNoSuchKeychain:
        KSLogError("No such keychain");
        break;
      case errSecInvalidKeychain:
        KSLogError("Invalid keychain");
        break;
      case errSecNoDefaultKeychain:
        KSLogError("No default keychain");
        break;
      case errSecInteractionRequired:
        KSLogError("User interaction required");
        break;
      case errSecAuthFailed:
        KSLogError("Keychain authentication failed");
        break;
      default:
        KSLogError("Undefined keychain error: %d", osstatus);
        break;
    }
    
    goto error;
  }
  
  if(pwlength){
    *pwlength = ipwlen;
  }
  
  if(password){
    *password = (char *)malloc(sizeof(char) * (strlen(ipasswd) + 1));
    strcpy(*password, ipasswd);
  }
  
error:
  if(server)  free(server);
  if(domain)  free(domain);
  if(account) free(account);
  if(path)    free(path);
  if(ipasswd) SecKeychainItemFreeContent(NULL, ipasswd);
  
  return item;
}

/**
 * Interactively unlock the default keychain on the command line by
 * prompting the user for credentials.  If the keychain is alreday
 * unlocked the user is not bothered.
 * 
 * @return status
 */
KSStatus KSKeychainManager::unlockKeychainInteractive(void) const {
  KSStatus status = KSStatusOk;
  OSStatus osstat = noErr;
  
  SecKeychainStatus kcstatus;
  if((osstat = SecKeychainGetStatus(NULL, &kcstatus)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
  if((kcstatus & kSecUnlockStateStatus) != kSecUnlockStateStatus || (kcstatus &  kSecReadPermStatus) != kSecReadPermStatus){
    fprintf(stdout, kKSKeychainManagerPromptPrefix "Process %d wants to unlock your keychain.\n", getpid());
    
    char *password;
    if((password = getpass(kKSKeychainManagerPromptPrefix "Password: ")) == NULL){
      status = KSStatusPermissionDenied;
      goto error;
    }
    
    if((osstat = SecKeychainUnlock(NULL, strlen(password), password, TRUE)) != noErr){
      switch(osstat){
        case errSecAuthFailed:
          status = KSStatusPermissionDenied;
          break;
        default:
          status = KSStatusError;
          break;
      }
      goto error;
    }
    
  }
  
error:
  return status;
}

/**
 * Collect a password from the user.
 * 
 * @param prompt the prompt to display
 * @param password on output, if successful: the password.  The caller must free
 * this data when through with it.
 * @param pwlength on output, if successful: the password length
 * @return status
 */
KSStatus KSKeychainManager::collectPasswordInteractive(CFStringRef prompt, char **password, UInt32 *pwlength) {
  KSStatus status = KSStatusOk;
  char *cprompt = (prompt != NULL) ? CFStringCopyCString(prompt, kDefaultStringEncoding) : NULL;
  
  fprintf(stdout, kKSKeychainManagerPromptPrefix "Process %d wants a password.\n", getpid());
  
  char *ipasswd;
  if((ipasswd = getpass((cprompt != NULL) ? cprompt : kKSKeychainManagerPromptPrefix "Password: ")) == NULL){
    status = KSStatusPermissionDenied;
    goto error;
  }
  
  if(pwlength){
    *pwlength = strlen(ipasswd);
  }
  
  if(password){
    *password = (char *)malloc(sizeof(char) * (strlen(ipasswd) + 1));
    strcpy(*password, ipasswd);
  }
  
error:
  if(cprompt) free(cprompt);
  
  return status;
}

/**
 * Grant an application access to the specified keychain item
 * 
 * @param item keychain item
 * @param path application path
 * @param label application label
 * @return status;
 */
KSStatus KSKeychainManager::grantApplicationAccess(SecKeychainItemRef item, CFStringRef path, CFStringRef label) {
  KSStatus status = KSStatusOk;
  OSStatus osstat = noErr;
  
  SecAccessRef itemAccess = NULL;
  SecACLRef oldACL = NULL;
  SecACLRef newACL = NULL;
  
  CFIndex acount;
  SecTrustedApplicationRef oldTrusted[10];
  CSSM_ACL_KEYCHAIN_PROMPT_SELECTOR pselector;
  CFStringRef description = NULL;
  CFArrayRef newTrusted = NULL;
  
  uint32 tagCount = 20;
  CSSM_ACL_AUTHORIZATION_TAG tags[tagCount];
  
  SecTrustedApplicationRef trustedApp = NULL;
  SecKeychainAttributeList attributes;
  SecKeychainAttribute attribute;
  
  attribute.tag = kSecLabelItemAttr;
  attribute.data = (void *)"KSKC";
  attribute.length = strlen((const char *)attribute.data);
  
  attributes.count = 1;
  attributes.attr  = &attribute;
  
  CFArrayRef acls = NULL;
  CFArrayRef apps = NULL;
  CSSM_ACL_AUTHORIZATION_TAG authtag = CSSM_ACL_AUTHORIZATION_DECRYPT;
  
  char *cpath = (path != NULL) ? CFStringCopyCString(path, kDefaultStringEncoding) : NULL;
  
  // Obtain the access reference object for the keychain item.
  // This returns a SecAccessRef, which we must release when we're finished using it.
  if((osstat = SecKeychainItemCopyAccess(item, &itemAccess)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
  // Obtain an array of ACL entry objects for the access object.
  // Limit the search to ACL entries with the specified authorization tag.
  if((osstat = SecAccessCopySelectedACLList(itemAccess, authtag, &acls)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
  // Create a new trusted application reference object for
  //  the application to be added to the list.
  if((osstat = SecTrustedApplicationCreateFromPath(cpath, &trustedApp)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
  // Extract the ACL entry object from the array of ACL entries,
  //  along with the ACL entry's list of trusted applications,
  //  its description, and its prompt selector flag setting.
  // This returns a SecACLRef and a CFArrayRef, which we must
  //  release we're finished using them.
  if(acls != NULL && (acount = CFArrayGetCount(acls)) > 0){
    
    oldACL = (SecACLRef)CFArrayGetValueAtIndex(acls, 0);
    
    if((osstat = SecACLCopySimpleContents(oldACL, &apps, &description, &pselector)) != noErr){
      status = KSStatusError;
      goto error;
    }
    
    // The application list is a CFArray.  Extract the list of applications from the CFArray.
    CFArrayGetValues(apps, CFRangeMake(0, acount), (const void **)oldTrusted);
    
    // Append the new application to the array and create a new CFArray.
    oldTrusted[acount] = trustedApp;
    newTrusted = CFArrayCreate(NULL, (const void **)oldTrusted, acount + 1, &kCFTypeArrayCallBacks);
    
  }else{
    newTrusted = CFArrayCreate(NULL, (const void **)&trustedApp, 1, &kCFTypeArrayCallBacks);
  }
  
  // Get the authorizations from the old ACL.
  if((osstat = SecACLGetAuthorizations(oldACL, tags, &tagCount)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
  // Delete the old ACL from the access object. The user is
  // prompted for permission to alter the keychain item.
  if((osstat = SecACLRemove(oldACL)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
  // Create a new ACL with the same attributes as the old
  //  one, except use the new CFArray of trusted applications.
  if((osstat = SecACLCreateFromSimpleContents(itemAccess, newTrusted, description, &pselector, &newACL)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
  // Set the authorizations for the new ACL to be the same as
  //  those for the old ACL.
  if((osstat = SecACLSetAuthorizations (newACL, tags, tagCount)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
  // Replace the access object in the keychain item with the
  //  new access object. The user is prompted for permission
  //  to alter the keychain item.
  if((osstat = SecKeychainItemSetAccess (item, itemAccess)) != noErr){
    status = KSStatusError;
    goto error;
  }
  
error:
  CFReleaseSafe(itemAccess);
  CFReleaseSafe(oldACL);
  CFReleaseSafe(newACL);
  CFReleaseSafe(description);
  CFReleaseSafe(newTrusted);
  CFReleaseSafe(trustedApp);
  CFReleaseSafe(acls);
  CFReleaseSafe(apps);
  
  if(cpath) free(cpath);
  
  return status;
}

/**
 * Attempt to copy a password item from the default keychain.  If the password
 * is missing, the user may be prompted for the password and that password may
 * be stored in the keychain for future use.
 * 
 * @param attributes a dictionary of attributes describing the account context
 * @param password on successful output: the account password.  The caller must
 * free this data when through with it.
 * @param pwlength on successful output: the account password length
 * @param askIfMissing ask the user for the password if it is not found in his
 * keychain
 * @param storeIfMissing store the password collected from the user
 * @param prompt password input prompt if missing
 * @return status
 */
KSStatus KSKeychainManager::copyInternetPasswordInteractive(CFDictionaryRef attributes, char **password, UInt32 *pwlength, bool askIfMissing, bool storeIfMissing, CFStringRef prompt) {
  SecKeychainItemRef item = NULL;
  KSStatus status = KSStatusOk;
  
  if((item = copyInternetPassword(attributes, password, pwlength)) != NULL){
    status = KSStatusOk;
  }else if(askIfMissing){
    status = collectPasswordInteractive(prompt, password, pwlength);
  }
  
  return status;
}

/**
 * Obtain string description
 * 
 * @return description
 */
CFStringRef KSKeychainManager::copyDescription(void) const {
  return CFSTR("<KeychainManager>");
}

