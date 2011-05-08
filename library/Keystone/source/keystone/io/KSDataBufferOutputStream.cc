// 
// $Id: KSDataBufferOutputStream.cc 42 2010-01-18 00:57:23Z brian $
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

#include "KSDataBufferOutputStream.hh"
#include "KSLog.hh"
#include "CFStringAdditions.hh"

/**
 * Construct
 */
KSDataBufferOutputStream::KSDataBufferOutputStream() {
  _buffer = CFDataCreateMutable(NULL, 0);
  _open = true;
}

/**
 * Construct
 * 
 * @param buffer buffer to use for output
 */
KSDataBufferOutputStream::KSDataBufferOutputStream(CFMutableDataRef buffer) {
  _buffer = (buffer != NULL) ? (CFMutableDataRef)CFRetain(buffer) : CFDataCreateMutable(NULL, 0);
  _open = true;
}

/**
 * Clean up
 */
KSDataBufferOutputStream::~KSDataBufferOutputStream() {
  if(_buffer) CFRelease(_buffer);
}

/**
 * Obtain the underlying buffer
 * 
 * @return buffer
 */
CFDataRef KSDataBufferOutputStream::getBuffer(void) const {
  return _buffer;
}

/**
 * Write to the buffer
 * 
 * @param bytes bytes to write
 * @param count number of bytes
 * @param on output: actual number of bytes written
 * @return status
 */
KSStatus KSDataBufferOutputStream::write(KSByte *bytes, KSLength count, KSLength *actual) {
  
  // make sure the stream hasn't been closed
  if(!_open){
    if(actual) *actual = 0;
    return KSStatusStreamClosed;
  }
  
  CFDataAppendBytes(_buffer, bytes, count);
  if(actual) *actual = count;
  
  return KSStatusOk;
}

/**
 * Flush the stream.  This method has no effect and always succeeds.
 * 
 * @return status
 */
KSStatus KSDataBufferOutputStream::flush(void) {
  // by definition this makes no sense here...
  return KSStatusOk;
}

/**
 * Close the stream.  No further writes are permitted after this method is
 * invoked.  This method always succeeds.
 * 
 * @return status
 */
KSStatus KSDataBufferOutputStream::close(void) {
  _open = false;
  return KSStatusOk;
}

