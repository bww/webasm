// 
// $Id: KSFileInputStream.cc 73 2010-01-28 13:43:50Z brian $
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

#include "KSFileInputStream.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "CFStringAdditions.hh"

#include <unistd.h>

/**
 * Read from a file descriptor
 * 
 * @param fd file descriptor
 * @param buffer buffer
 * @param count bytes to read
 * @param actual on output: actual byte count read
 * @return status
 */
KSStatus KSFileInputStreamRead(int fd, KSByte *buffer, KSLength count, KSLength *actual) {
  KSStatus status = KSStatusOk;
  
  int n;
  if((n = ::read(fd, buffer, count)) < 0){
    
    switch(errno){
      case EAGAIN:        status = KSStatusWouldBlock;          break;
      case EBADF:         status = KSStatusBadFileDescriptor;   break;
      case ENXIO:         status = KSStatusNoSuchDevice;        break;
      case EACCES:        status = KSStatusPermissionDenied;    break;
      case EDQUOT:        status = KSStatusQuotaExceeded;       break;
      case EEXIST:        status = KSStatusResourceExists;      break;
      case EINTR:         status = KSStatusInterrupted;         break;
      case EINVAL:        status = KSStatusInvalidOperand;      break;
      case EISDIR:        status = KSStatusInvalidResource;     break;
      case EIO:           status = KSStatusIOError;             break;
      case ELOOP:         status = KSStatusTooManyLinks;        break;
      case EMFILE:        status = KSStatusTooManyOpenFiles;    break;
      case ENFILE:        status = KSStatusFileTableFull;       break;
      case ENAMETOOLONG:  status = KSStatusPathTooLong;         break;
      case ENOENT:        status = KSStatusNoSuchResource;      break;
      case ECONNRESET:    status = KSStatusConnectionReset;     break;
      case ENOTCONN:      status = KSStatusNotConnected;        break;
      case ETIMEDOUT:     status = KSStatusTimedOut;            break;
      default:            status = KSStatusError;               break;
    }
    
    if(actual) *actual = 0;
    goto error;
  }
  
  if(actual) *actual = n;
  
error:
#if defined(KEYSTONE_DEBUG_DUMP_STREAMS)
  CFStringRef display = CFStringCreateHexStringWithBytes(NULL, buffer, n, ' ');
  KSLogError("R: [%d] <%@>", n, display);
  CFReleaseSafe(display);
#endif
  
  return status;
}

/**
 * Construct
 * 
 * @param fd file descriptor
 */
KSFileInputStream::KSFileInputStream(int fd) {
  _fd = fd;
}

/**
 * Construct
 * 
 * @param stream input stream
 */
KSFileInputStream::KSFileInputStream(FILE *stream) {
  _fd = fileno(stream);
}

/**
 * Clean up
 */
KSFileInputStream::~KSFileInputStream() {
}

/**
 * Obtain the file descriptor from which this stream reads.
 * 
 * @return file descriptor
 */
int KSFileInputStream::getFileDescriptor(void) const {
  return _fd;
}

/**
 * Read bytes from the underlying stream
 * 
 * @param buffer buffer
 * @param count bytes to read
 * @param actual on output: actual byte count read
 * @return status
 */
KSStatus KSFileInputStream::read(KSByte *buffer, KSLength count, KSLength *actual) {
  return KSFileInputStreamRead(getFileDescriptor(), buffer, count, actual);
}

/**
 * Close the underlying stream
 * 
 * @return status
 */
KSStatus KSFileInputStream::close(void) {
  KSStatus status = KSStatusOk;
  
  if(::close(getFileDescriptor()) < 0){
    switch(errno){
      case EBADF:         status = KSStatusBadFileDescriptor;   break;
      case EINTR:         status = KSStatusInterrupted;         break;
      case EIO:           status = KSStatusIOError;             break;
      default:            status = KSStatusError;               break;
    }
  }
  
  return status;
}

