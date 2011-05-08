// 
// $Id: KSSocket.cc 73 2010-01-28 13:43:50Z brian $
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

#include "KSSocket.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "KSFileInputStream.hh"
#include "KSFileOutputStream.hh"
#include "CFStringAdditions.hh"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * Construct an unconnected socket
 * 
 * @param address remote address
 */
KSSocket::KSSocket(KSAddress *address) {
  _closeOnDealloc = true;
  _address = (address != NULL) ? (KSAddress *)KSRetain(address) : NULL;
  _sockfd  = -1;
  _state   = kKSSocketStateDisconnected;
  _mode    = kKSSocketModeNonBlocking;
}

/**
 * Construct a connected socket
 * 
 * @param sockfd socket file descriptor
 */
KSSocket::KSSocket(int sockfd) {
  
  _closeOnDealloc = true;
  _state   = kKSSocketStateDisconnected;
  _mode    = kKSSocketModeBlocking;
  _address = NULL;
  
  if((_sockfd = sockfd) > 0){
    struct sockaddr remote;
    socklen_t rlength = sizeof(struct sockaddr);
    if(getpeername(_sockfd, &remote, &rlength) == 0){
      _address = new KSAddress(&remote);
    }
  }
  
}

/**
 * Clean up
 */
KSSocket::~KSSocket() {
  if(_address) KSRelease(_address);
  if(_sockfd > 0 && _closeOnDealloc) ::close(_sockfd);
}

/**
 * Read (internal)
 * 
 * @param buffer buffer
 * @param count bytes to read
 * @param actual on output: actual byte count read
 * @return status
 */
KSStatus KSSocket::__read(KSByte *buffer, KSLength count, KSLength *actual) {
  return KSFileInputStreamRead(_sockfd, buffer, count, actual);
}

/**
 * Write (internal)
 * 
 * @param buffer buffer
 * @param count bytes to write
 * @param actual on output: actual byte count written
 * @return status
 */
KSStatus KSSocket::__write(KSByte *buffer, KSLength count, KSLength *actual) {
  return KSFileOutputStreamWrite(_sockfd, buffer, count, actual);
}

/**
 * Obtain the socket remote (peer) address.
 * 
 * @return remote address
 */
KSAddress * KSSocket::getRemoteAddress(void) const {
  return _address;
}

/**
 * Obtain the underlying socket file descriptor
 * 
 * @return socket
 */
int KSSocket::getFileDescriptor(void) const {
  return _sockfd;
}

/**
 * Connect to the remote host
 * 
 * @param timeout connect timeout, expressed in seconds
 * @param error on output, if status is other than KSStatusOk: an
 * error describing the problem
 * @return status
 */
KSStatus KSSocket::connect(CFTimeInterval timeout, KSError **error) {
  KSStatus status = KSStatusOk;
  fd_set wfdset, rfdset;
  int fcflags = 0;
  int z;
  
  // make sure we're not already connected
  if(_sockfd > 0 && _state == kKSSocketStateConnected){
    return KSStatusOk;
  }
  
  if(_address == NULL){
    status = KSStatusInvalidState;
    if(error) *error = new KSError(status, "Cannot connect: address is null");
    goto error;
  }
  
  if(_sockfd < 1){
    if((_sockfd = ::socket(PF_INET, SOCK_STREAM, 0)) < 0){
      status = KSStatusError;
      if(error) *error = new KSError(status, "Unable to create socket");
      goto error;
    }
  }
  
  if(_mode == kKSSocketModeNonBlocking && timeout > 0){
    
    fcflags = fcntl(_sockfd, F_GETFL, 0);
    
    if(fcntl(_sockfd, F_SETFL, fcflags | O_NONBLOCK) < 0){
      status = KSStatusError;
      if(error) *error = new KSError(status, "Unable to make socket non-blocking: %s", strerror(errno));
      goto error;
    }
    
  }
  
  struct sockaddr addr;
  _address->copyAddress(&addr);
  
  if((z = ::connect(_sockfd, &addr, sizeof(struct sockaddr_in))) < 0){
    if(errno != EINPROGRESS){
      status = KSStatusError;
      if(error) *error = new KSError(status, "Unable to connect to: %K: %s", _address, strerror(errno));
      goto error;
    }
  }
  
  if(z != 0 && _mode == kKSSocketModeNonBlocking && timeout > 0){
    struct timeval tv;
    socklen_t elen = sizeof(int);
    int e = 0;
    
    tv.tv_sec  = (int)timeout;
    tv.tv_usec = (int)((timeout - tv.tv_sec) * 1000000);
    
    FD_ZERO(&wfdset);
    FD_SET(_sockfd, &wfdset);
    FD_ZERO(&rfdset);
    FD_SET(_sockfd, &rfdset);
    
    if((z = select(_sockfd + 1, &rfdset, &wfdset, NULL, &tv)) < 0){
      status = KSStatusError;
      if(error) *error = new KSError(status, "An error occured on select: %s", strerror(errno));
      goto error;
    }else if(z == 0){
      status = KSStatusTimedOut;
      if(error) *error = new KSError(status, "Connection timed out after %.2fs", timeout);
      goto error;
    }
    
    if((z = getsockopt(_sockfd, SOL_SOCKET, SO_ERROR, &e, &elen)) < 0){
      status = KSStatusError;
      if(error) *error = new KSError(status, "An error occured while fetching the connect result: %s", strerror(errno));
      goto error;
    }else if(e != 0){
      status = KSStatusConnectionRefused;
      if(error) *error = new KSError(status, "Unable to connect to: %K: %s", _address, strerror(e));
      goto error;
    }
    
  }
  
error:
  if(_mode == kKSSocketModeNonBlocking && timeout > 0){
    if(fcntl(_sockfd, F_SETFL, fcflags) < 0){
      KSLogError("Warning: Unable to restore socket flags");
    }
  }
  
  if(_sockfd > 0 && status != KSStatusOk){
    ::close(_sockfd);
  }
  
  return status;
}

/**
 * Read
 * 
 * @param buffer buffer
 * @param count bytes to read
 * @param actual on output: actual byte count read
 * @return status
 */
KSStatus KSSocket::read(KSByte *buffer, KSLength count, KSLength *actual) {
  return __read(buffer, count, actual);
}

/**
 * Write (internal)
 * 
 * @param buffer buffer
 * @param count bytes to write
 * @param actual on output: actual byte count written
 * @return status
 */
KSStatus KSSocket::write(KSByte *buffer, KSLength count, KSLength *actual) {
  return __write(buffer, count, actual);
}

/**
 * Close the underlying socket file descriptor
 * 
 * @return status
 */
KSStatus KSSocket::close(void) {
  if(_sockfd > 0) ::close(_sockfd);
  _sockfd = -1;
  return KSStatusOk;
}

/**
 * Determine equality
 * 
 * @param o candidate
 * @return equal or not
 */
bool KSSocket::isEqual(KSObject *o) const {
  return (o == this);
}

/**
 * Obtain a string description
 * 
 * @retrun description
 */
CFStringRef KSSocket::copyDescription(void) const {
  CFStringRef descr = NULL;
  
  if(_address != NULL){
    CFStringRef addr = _address->copyDescription();
    descr = CFStringCreateWithFormat(NULL, 0, CFSTR("<KSSocket %@>"), addr);
    CFReleaseSafe(addr);
  }else{
    descr = CFSTR("<KSSocket>");
  }
  
  return descr;
}

