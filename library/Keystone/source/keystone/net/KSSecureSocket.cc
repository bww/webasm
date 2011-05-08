// 
// $Id: KSSecureSocket.cc 73 2010-01-28 13:43:50Z brian $
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

#include "KSSecureSocket.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "CFStringAdditions.hh"

#include <CoreServices/CoreServices.h>

#define super KSSocket

/**
 * Secure read callback
 * 
 * @param context pointer to the socket file descriptor
 * @param data a buffer into which data is read
 * @param length on input: number of bytes to read; on output: actual number of
 * bytes read.
 * @return status
 */
OSStatus KSSecureSocketReadCallBack(const void *context, void *data, size_t *length) {
  OSStatus osstat = noErr;
  KSLength w = *length;
  KSLength z;
  
  if(context == NULL || length == NULL || data == NULL){
    KSLogError("Invalid parameter to TLS read callback");
    return paramErr;
  }
  
  KSStatus status;
  if((status = ((KSSocket *)context)->__read((KSByte *)data, w, &z)) != KSStatusOk || z < w){
    switch(status){
      case KSStatusOk:
      case KSStatusWouldBlock:
        osstat = errSSLWouldBlock;
        break;
      default:
        osstat = readErr;
        break;
    }
  }
  
  *length = (size_t)z;
  
#if defined(KEYSTONE_DEBUG_SOCKET_TLS_IO)
  KSLog("R: %d/%d (%d)", z, w, (w - z));
#endif
  
  return osstat;
}

/**
 * Secure write callback
 * 
 * @param context pointer to the socket file descriptor
 * @param data a buffer of data to write
 * @param length on input: number of bytes to write; on output: actual number of
 * bytes written.
 * @return status
 */
OSStatus KSSecureSocketWriteCallBack(const void *context, const void *data, size_t *length) {
  OSStatus osstat = noErr;
  KSLength w = *length;
  KSLength z;
  
  if(context == NULL || length == NULL || data == NULL){
    KSLogError("Invalid parameter to TLS write callback");
    return paramErr;
  }
  
  KSStatus status;
  if((status = ((KSSocket *)context)->__write((KSByte *)data, w, &z)) != KSStatusOk || z < w){
    switch(status){
      case KSStatusOk:
      case KSStatusWouldBlock:
        osstat = errSSLWouldBlock;
        break;
      default:
        osstat = writErr;
        break;
    }
  }
  
  *length = (size_t)z;
  
#if defined(KEYSTONE_DEBUG_SOCKET_TLS_IO)
  KSLog("W: %d/%d (%d)", z, w, (w - z));
#endif
  
  return osstat;
}

/**
 * Construct an unconnected socket
 * 
 * @param address remote address
 */
KSSecureSocket::KSSecureSocket(KSAddress *address) : super(address) {
  _secContext = NULL;
  _reqSecureChannel = false;
}

/**
 * Construct a connected socket
 * 
 * @param sockfd socket file descriptor
 */
KSSecureSocket::KSSecureSocket(int sockfd) : super(sockfd) {
  _secContext = NULL;
  _reqSecureChannel = false;
}

/**
 * Clean up
 */
KSSecureSocket::~KSSecureSocket() {
  if(_secContext) SSLDisposeContext(_secContext);
}

/**
 * Initialize
 * 
 * @param server use server role or not
 * @param error on output, if status is other than KSStatusOk: an
 * error describing the problem
 * @return status
 */
KSStatus KSSecureSocket::init(bool server, KSError **error) {
  KSStatus status = KSStatusOk;
  OSStatus osstat = noErr;
  
  if((osstat = SSLNewContext(server, &_secContext)) != noErr){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Unable to create TLS security context");
    goto error;
  }
  
  if((osstat = SSLSetIOFuncs(_secContext, KSSecureSocketReadCallBack, KSSecureSocketWriteCallBack)) != noErr){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Unable to set TLS I/O functions: %d", osstat);
    goto error;
  }
  
  if((osstat = SSLSetClientSideAuthenticate(_secContext, kTryAuthenticate)) != noErr){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Unable to set TLS client side authentication policy: %d", osstat);
    goto error;
  }
  
  if((osstat = SSLSetConnection(_secContext, this)) != noErr){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Unable to set TLS socket connection: %d", osstat);
    goto error;
  }
  
  SSLSetProtocolVersionEnabled(_secContext, kSSLProtocol2, FALSE);
  SSLSetProtocolVersionEnabled(_secContext, kSSLProtocol3, TRUE);
  SSLSetProtocolVersionEnabled(_secContext, kTLSProtocol1, TRUE);
  
  SSLSetEnableCertVerify(_secContext, FALSE);
  SSLSetAllowsAnyRoot(_secContext, TRUE);
  SSLSetAllowsExpiredRoots(_secContext, FALSE);
  SSLSetAllowsExpiredCerts(_secContext, FALSE);
  
error:
  return status;
}

/**
 * TLS handshate
 * 
 * @param error on output, if status is other than KSStatusOk: an
 * error describing the problem
 * @return status
 */
KSStatus KSSecureSocket::handshake(KSError **error) {
  KSStatus status = KSStatusOk;
  OSStatus osstat = noErr;
  
  if((osstat = SSLHandshake(_secContext)) != noErr){
    
    switch(osstat){
      case errSSLWouldBlock:
        status = KSStatusWouldBlock;
        break;
      case errSSLProtocol:
        status = KSStatusProtocolError;
        break;
      case errSSLUnknownRootCert:
        status = KSStatusTrustNotAvailable;
        break;
      case errSSLNoRootCert:
        status = KSStatusCertificateChainInvalid;
        break;
      case errSSLCertExpired:
        status = KSStatusCertificateExpired;
        break;
      case errSSLXCertChainInvalid:
        status = KSStatusCertificateChainInvalid;
        break;
      default:
        status = KSStatusError;
        break;
    }
    
    if(error) *error = new KSError(status, "Unable to complete TLS handshake: %d", osstat);
  }
  
  return status;
}

/**
 * Start TLS
 * 
 * @param role which end of the connection this socket represents
 * @param error on output, if status is other than KSStatusOk: an
 * error describing the problem
 * @return status
 */
KSStatus KSSecureSocket::startSecureTransport(KSSecureTransportRole role, KSError **error) {
  KSStatus status = KSStatusOk;
  OSStatus osstat = noErr;
  SSLSessionState state = kSSLIdle;
  
  _reqSecureChannel = true;
  
  if((status = init((role == kKSSecureTransportRoleServer), error)) != KSStatusOk) goto error;
  
  do {
    
    // Handshake / negitiation
    if((status = handshake(error)) != KSStatusOk && status != KSStatusWouldBlock) goto error;
    
    // Check status; continue so long as we're still in the handshake state
    if((osstat = SSLGetSessionState(_secContext, &state)) != noErr){
      status = KSStatusError;
      if(error) *error = new KSError(status, "Unable to obtain TLS session state");
      goto error;
    }
    
  } while(state == kSSLHandshake);
  
error:
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
KSStatus KSSecureSocket::read(KSByte *buffer, KSLength count, KSLength *actual) {
  KSStatus status = KSStatusOk;
  
  if(_reqSecureChannel && _secContext == NULL){
    status = KSStatusInvalidState;
  }else if(_secContext != NULL){
    OSStatus osstat;
    size_t z;
    
    if((osstat = SSLRead(_secContext, buffer, count, &z)) != noErr){
      switch(osstat){
        case errSSLWouldBlock:
          status = KSStatusWouldBlock;
          break;
        default:
          status = KSStatusError;
          break;
      }
    }
    
    if(actual) *actual = z;
  }else{
    status = __read(buffer, count, actual);
  }
  
  return status;
}

/**
 * Write (internal)
 * 
 * @param buffer buffer
 * @param count bytes to write
 * @param actual on output: actual byte count written
 * @return status
 */
KSStatus KSSecureSocket::write(KSByte *buffer, KSLength count, KSLength *actual) {
  KSStatus status = KSStatusOk;
  
  if(_reqSecureChannel && _secContext == NULL){
    status = KSStatusInvalidState;
  }else if(_secContext != NULL){
    OSStatus osstat;
    size_t z;
    
    if((osstat = SSLWrite(_secContext, buffer, count, &z)) != noErr){
      switch(osstat){
        case errSSLWouldBlock:
          status = KSStatusWouldBlock;
          break;
        default:
          status = KSStatusError;
          break;
      }
    }
    
    if(actual) *actual = z;
  }else{
    status = __write(buffer, count, actual);
  }
  
  return status;
}

/**
 * Close the underlying socket file descriptor
 * 
 * @return status
 */
KSStatus KSSecureSocket::close(void) {
  KSStatus status = KSStatusOk;
  
  if(_secContext != NULL){
    OSStatus osstat = noErr;
    
    if((osstat = SSLClose(_secContext)) != noErr){
      switch(osstat){
        case errSSLWouldBlock:
          status = KSStatusWouldBlock;
          break;
        default:
          status = KSStatusError;
          break;
      }
    }
    
  }else{
    status = super::close();
  }
  
  return status;
}

/**
 * Obtain a string description
 * 
 * @retrun description
 */
CFStringRef KSSecureSocket::copyDescription(void) const {
  CFStringRef descr = NULL;
  KSAddress *address;
  
  if((address = getRemoteAddress()) != NULL){
    CFStringRef addr = address->copyDescription();
    descr = CFStringCreateWithFormat(NULL, 0, CFSTR("<KSSecureSocket %@>"), addr);
    CFReleaseSafe(addr);
  }else{
    descr = CFSTR("<KSSecureSocket>");
  }
  
  return descr;
}


