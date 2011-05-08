// 
// $Id: KSDataBufferInputStream.cc 42 2010-01-18 00:57:23Z brian $
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

#include "KSDataBufferInputStream.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "CFStringAdditions.hh"

/**
 * Construct
 * 
 * @param buffer buffer to use for output
 */
KSDataBufferInputStream::KSDataBufferInputStream(CFDataRef buffer) {
  _buffer = (buffer != NULL) ? (CFDataRef)CFRetain(buffer) : NULL;
  _cursor = 0;
  _open = true;
}

/**
 * Clean up
 */
KSDataBufferInputStream::~KSDataBufferInputStream() {
  if(_buffer) CFRelease(_buffer);
}

/**
 * Obtain the underlying buffer
 * 
 * @return buffer
 */
CFDataRef KSDataBufferInputStream::getBuffer(void) const {
  return _buffer;
}

/**
 * Read from the underlying buffer.
 * 
 * @param bytes buffer into which bytes should be copied
 * @param count number of bytes to copy
 * @param actual on output: actual number of bytes copied
 * @return status
 */
KSStatus KSDataBufferInputStream::read(KSByte *bytes, KSLength count, KSLength *actual) {
  
  if(!_open){
    if(actual) *actual = 0;
    return KSStatusStreamClosed;
  }
  
  if(_buffer == NULL){
    if(actual) *actual = 0;
    return KSStatusIOError;
  }
  
  CFRange range = CFRangeMake(_cursor, KSMin(count, CFDataGetLength(_buffer) - _cursor));
  CFDataGetBytes(_buffer, range, bytes);
  
  _cursor = _cursor + range.length;
  if(actual) *actual = range.length;
  
  return KSStatusOk;
}

/**
 * Close this stream.  No further reads are permitted after this method is
 * invoked.  This method always succeeds.
 * 
 * @return status
 */
KSStatus KSDataBufferInputStream::close(void) {
  _open = false;
  return KSStatusOk;
}


