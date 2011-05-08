// 
// $Id: KSFileOutputStream.cc 73 2010-01-28 13:43:50Z brian $
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

#include "KSFileOutputStream.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "CFStringAdditions.hh"

#include <unistd.h>

/**
 * Write to a file descriptor
 * 
 * @param fd file descriptor
 * @param buffer bytes to write
 * @param count number of bytes to write from the buffer
 * @param actual on output: the actual number of bytes written
 * @return status
 */
KSStatus KSFileOutputStreamWrite(int fd, KSByte *buffer, KSLength count, KSLength *actual) {
  KSStatus status = KSStatusOk;
  
  int n;
  if((n = ::write(fd, buffer, count)) < 0){
    
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
  KSLogError("W: [%d] <%@>", n, display);
  CFReleaseSafe(display);
#endif
  
  return status;
}

/**
 * Construct
 * 
 * @param fd file descriptor
 */
KSFileOutputStream::KSFileOutputStream(int fd) {
  _fd = fd;
}

/**
 * Construct
 * 
 * @param stream output stream
 */
KSFileOutputStream::KSFileOutputStream(FILE *stream) {
  _fd = fileno(stream);
}

/**
 * Clean up
 */
KSFileOutputStream::~KSFileOutputStream() {
}

/**
 * Obtain the file descriptor to which this stream writes.
 * 
 * @return file descriptor
 */
int KSFileOutputStream::getFileDescriptor(void) const {
  return _fd;
}

/**
 * Write bytes to the underlying stream.
 * 
 * @param buffer bytes to write
 * @param count number of bytes to write from the buffer
 * @param actual on output: the actual number of bytes written
 * @return status
 */
KSStatus KSFileOutputStream::write(KSByte *buffer, KSLength count, KSLength *actual) {
  return KSFileOutputStreamWrite(getFileDescriptor(), buffer, count, actual);
}

/**
 * Flush the underlying output stream to disk.
 * 
 * @return status
 */
KSStatus KSFileOutputStream::flush(void) {
  // KSFileOutputStream doesn't do any buffering
  return KSStatusOk;
}

/**
 * Close the underlying stream
 * 
 * @return status
 */
KSStatus KSFileOutputStream::close(void) {
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

