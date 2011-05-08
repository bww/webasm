// 
// $Id: KSOutputStream.cc 28 2009-07-28 00:01:59Z brian $
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

#include "KSOutputStream.hh"
#include "KSLog.hh"
#include "CFStringAdditions.hh"

/**
 * Construct
 */
KSOutputStream::KSOutputStream() {
}

/**
 * Clean up
 */
KSOutputStream::~KSOutputStream() {
}

/**
 * @brief Write a Core Foundation string
 * 
 * This is a convenience method which converts the string to bytes using the
 * default text encoding and invokes <code>write</code> with the result.
 * 
 * @param string the string to write
 * @param range character range to write
 * @param actual on output: the actual number of bytes written
 * @return status
 */
KSStatus KSOutputStream::writeCFString(CFStringRef string, CFRange range, KSLength *actual) {
  KSStatus status = KSStatusOk;
  
  KSLength sblen;
  KSLength swlen;
  KSByte *textbuffer = NULL;
  
  if(actual != NULL) *actual = 0;
  
  if(CFStringGetBytes(string, range, kDefaultStringEncoding, '?', false, NULL, 0, &sblen) < 1){
    KSLogError("Unable to convert output bytes");
    status = KSStatusError;
    goto error;
  }
  
  if((textbuffer = (KSByte *)malloc(sblen)) == NULL){
    KSLogError("Unable to allocate for text buffer");
    status = KSStatusError;
    goto error;
  }
  
  if(CFStringGetBytes(string, range, kDefaultStringEncoding, '?', false, textbuffer, sblen, &sblen) < 1){
    KSLogError("Unable to convert output bytes");
    status = KSStatusError;
    goto error;
  }
  
  if((status = this->write(textbuffer, sblen, &swlen)) != KSStatusOk || sblen != swlen){
    KSLogError("Error occured while writing output bytes (partial write)");
    goto error;
  }
  
  if(actual != NULL) *actual = swlen;
  
error:
  free(textbuffer);
  
  return status;
}


