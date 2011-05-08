// 
// $Id: KSAddress.cc 54 2010-01-21 23:16:48Z brian $
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

#include "KSAddress.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "CFStringAdditions.hh"

#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <limits.h>

/**
 * Construct
 * 
 * @param ip an IP address expressed in dot-notation
 * @param port a port number
 */
KSAddress::KSAddress(const char *ip, short port) {
  _address  = NULL;
  _hostname = NULL;
  if(ip != NULL) init(inet_addr(ip), port);
}

/**
 * Construct
 * 
 * @param ip an IP address expressed in dot-notation
 * @param port a port number
 */
KSAddress::KSAddress(CFStringRef ip, short port) {
  _address  = NULL;
  _hostname = NULL;
  
  if(ip != NULL){
    char *ips = CFStringCopyCString(ip, kDefaultStringEncoding);
    init(inet_addr(ips), port);
    if(ips) free(ips);
  }
  
}

/**
 * Construct
 * 
 * @param address an address
 */
KSAddress::KSAddress(struct sockaddr *address) {
  _address  = NULL;
  _hostname = NULL;
  
  if(address != NULL){
    _address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr));
    memcpy(_address, address, sizeof(struct sockaddr_in));
  }
  
}

/**
 * Clean up
 */
KSAddress::~KSAddress() {
  if(_address)  free(_address);
  if(_hostname) CFRelease(_hostname);
}

/**
 * Initialize with an address and port
 * 
 * @param address the address
 * @param port the port
 */
void KSAddress::init(in_addr_t address, short port) {
  _address = (struct sockaddr_in *)malloc(sizeof(in_addr_t));
  _address->sin_family = PF_INET;
  _address->sin_port = htons(port);
  _address->sin_addr.s_addr = address;
}

/**
 * Create a string containing the local host name
 * 
 * @return local host name
 */
CFStringRef KSAddress::copyLocalHostName(void) {
  CFStringRef localhost = NULL;
  
  char hostname[_POSIX_HOST_NAME_MAX + 1];
  if(gethostname(hostname, _POSIX_HOST_NAME_MAX) == 0){
    localhost = CFStringCreateWithCString(NULL, hostname, kDefaultStringEncoding);
  }
  
  return localhost;
}

/**
 * Copy addresses for local network interfaces.
 * 
 * @return local interface addresses
 */
CFArrayRef KSAddress::copyLocalAddresses(void) {
  CFMutableArrayRef addresses = CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
  struct ifaddrs *locals = NULL;
  struct ifaddrs *current;
  
  int z;
  if((z = getifaddrs(&locals)) != 0){
    KSLogError("Unable to get local addresses: %s", strerror(errno));
    goto error;
  }
  
  for(current = locals; current != NULL; current = current->ifa_next){
    if(current->ifa_addr->sa_family == AF_INET){
      KSAddress *address = new KSAddress(current->ifa_addr);
      CFArrayAppendValue(addresses, address);
      KSRelease(address);
    }
  }
  
error:
  if(locals) freeifaddrs(locals);
  
  return addresses;
}

/**
 * Create an address by resolving the host and service from the specified URL
 * 
 * @param url URL
 * @param error on output, if the returned address is NULL: an error describing
 * the reason the hostname could not be resolved
 * @return resolved address
 */
KSAddress * KSAddress::createAddressWithURL(CFURLRef url, KSError **error) {
  
  if(url == NULL){
    if(error) *error = new KSError(KSStatusInvalidParameter, "No URL provided");
    return NULL;
  }
  
  CFStringRef host = CFURLCopyHostName(url);
  CFStringRef scheme = CFURLCopyScheme(url);
  
  KSAddress *address = createAddressWithHostName(host, scheme, error);
  
  if(host)    CFRelease(host);
  if(scheme)  CFRelease(scheme);
  
  return address;
}

/**
 * Create an address by resolving the specified hostname
 * 
 * @param host hostname
 * @param service service (e.g., "80" or "http")
 * @param error on output, if the returned address is NULL: an error describing
 * the reason the hostname could not be resolved
 * @return resolved address
 */
KSAddress * KSAddress::createAddressWithHostName(CFStringRef host, CFStringRef service, KSError **error) {
  struct addrinfo *info = NULL;
  struct addrinfo hints;
  KSAddress *address = NULL;
  
  if(host == NULL){
    if(error) *error = new KSError(KSStatusInvalidParameter, "No hostname provided");
    return NULL;
  }
  
  if(service == NULL){
    if(error) *error = new KSError(KSStatusInvalidParameter, "No service provided");
    return NULL;
  }
  
  char *shost = CFStringCopyCString(host, kDefaultStringEncoding);
  char *sservice = CFStringCopyCString(service, kDefaultStringEncoding);
  
  bzero(&hints, sizeof(struct addrinfo));
  
  hints.ai_flags    = AI_CANONNAME;
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  int z;
  if((z = getaddrinfo(shost, sservice, &hints, &info)) < 0){
    if(error) *error = new KSError(KSStatusNodeResolutionFailed, "Could not resolve host: %@ (%@)", host, service);
    goto error;
  }
  
  // just take the first one
  if(info != NULL){
    address = new KSAddress(info->ai_addr);
    address->_hostname = CFStringCreateWithCString(NULL, info->ai_canonname, kDefaultStringEncoding);
  }
  
error:
  if(shost)     free(shost);
  if(sservice)  free(sservice);
  if(info)      freeaddrinfo(info);
  
  return address;
}

/**
 * Create every address resolved by a hostname.  This method is similar to
 * createAddressWithHostName, except that instead of returning only the first
 * resolved address, every address is returned.
 * 
 * @param host hostname
 * @param service service (e.g., "80" or "http")
 * @param error on output, if the returned address is NULL: an error describing
 * the reason the hostname could not be resolved
 * @return resolved addresses
 */
CFArrayRef KSAddress::createEveryAddressWithHostName(CFStringRef host, CFStringRef service, KSError **error) {
  struct addrinfo *info = NULL;
  struct addrinfo *p = NULL;
  struct addrinfo hints;
  CFMutableArrayRef addresses = NULL;
  
  if(host == NULL){
    if(error) *error = new KSError(KSStatusInvalidParameter, "No hostname provided");
    return NULL;
  }
  
  if(service == NULL){
    if(error) *error = new KSError(KSStatusInvalidParameter, "No service provided");
    return NULL;
  }
  
  char *shost = CFStringCopyCString(host, kDefaultStringEncoding);
  char *sservice = CFStringCopyCString(service, kDefaultStringEncoding);
  
  bzero(&hints, sizeof(struct addrinfo));
  
  hints.ai_flags    = AI_CANONNAME;
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  int z;
  if((z = getaddrinfo(shost, sservice, &hints, &info)) < 0){
    if(error) *error = new KSError(KSStatusNodeResolutionFailed, "Could not resolve host: %@ (%@)", host, service);
    goto error;
  }
  
  addresses = CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
  
  for(p = info; p != NULL; p = p->ai_next){
    KSAddress *address = new KSAddress(p->ai_addr);
    address->_hostname = CFStringCreateWithCString(NULL, info->ai_canonname, kDefaultStringEncoding);
    CFArrayAppendValue(addresses, address);
    KSRelease(address);
  }
  
error:
  if(shost)     free(shost);
  if(sservice)  free(sservice);
  if(info)      freeaddrinfo(info);
  
  return addresses;
}

/**
 * Obtain the internal address
 * 
 * @return address
 */
struct sockaddr * KSAddress::getAddress(void) const {
  return (struct sockaddr *)_address;
}

/**
 * Copy the internal address into the provided structure.
 * 
 * @param address on output: the address
 */
void KSAddress::copyAddress(struct sockaddr *address) const {
  if(address != NULL && _address != NULL){
    memcpy(address, _address, sizeof(struct sockaddr_in));
  }
}

/**
 * Obtain the canonical hostname, if available.  The hostname is only available
 * if the address was resolved via createAddressWithHostName or createAddressWithURL.
 * 
 * @return hostname
 */
CFStringRef KSAddress::getHostName(void) const {
  return _hostname;
}

/**
 * Determine semantic equality
 * 
 * @param o string
 * @return equal or not
 */
bool KSAddress::isEqual(KSObject *o) const {
  if(o == NULL || o->getTypeID() != KSAddressTypeID) return false;
  KSAddress *c = (KSAddress *)o;
  return (c != NULL && c->_address != NULL && _address != NULL && c->_address->sin_family == _address->sin_family && c->_address->sin_port == _address->sin_port && c->_address->sin_addr.s_addr == _address->sin_addr.s_addr);
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSAddress::copyDescription(void) const {
  if(_address != NULL){
    if(_hostname != NULL){
      return CFStringCreateWithFormat(NULL, NULL, CFSTR("%@ <%s:%d>"), _hostname, inet_ntoa(_address->sin_addr), ntohs(_address->sin_port));
    }else{
      return CFStringCreateWithFormat(NULL, NULL, CFSTR("<%s:%d>"), inet_ntoa(_address->sin_addr), ntohs(_address->sin_port));
    }
  }else{
    return CFSTR("<unknown>");
  }
}


