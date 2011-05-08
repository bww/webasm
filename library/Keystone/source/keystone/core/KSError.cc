// 
// $Id: KSError.cc 82 2010-08-12 21:58:20Z brian $
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

#include "KSError.hh"

/**
 * Obtain an error message from an error that may or may not be null.
 * 
 * @param error an error or NULL
 * @return the error message
 */
KSString * KSErrorGetMessage(KSError *error) {
  KSString *message;
  return ((error != NULL) && ((message = error->getMessage()) != NULL)) ? message : KSSTR("Unknown error");
}

/**
 * Internal Constructor
 */
KSError::KSError() {
  _status = KSStatusOk;
  _message = NULL;
}

/**
 * Construct
 * 
 * @param status status
 * @param message error message
 */
KSError::KSError(KSStatus status, KSString *message) {
  _status = status;
  _message = (message != NULL) ? (KSString *)KSRetain(message) : new KSString(KSStatusGetMessage(status));
}

/**
 * Construct
 * 
 * @param status status
 * @param message error message format
 * @param ... format arguments
 */
KSError::KSError(KSStatus status, const char *message, ...) {
  _status = status;
  
  if(message != NULL){
    va_list ap;
    va_start(ap, message);
    _message = new KSString(0, message, ap);
    va_end(ap);
  }else{
    _message = new KSString(KSStatusGetMessage(status));
  }
  
}

/**
 * Clean up
 */
KSError::~KSError() {
  KSRelease(_message);
}

/**
 * Obtain the status
 * 
 * @return status
 */
KSStatus KSError::getStatus(void) const {
  return _status;
}

/**
 * Obtain the error message
 * 
 * @return message
 */
KSString * KSError::getMessage(void) const {
  return _message;
}

/**
 * Determine equality
 * 
 * @param o candidate
 * @return equal or not
 */
bool KSError::isEqual(KSObject *o) const {
  if(o == NULL || o->getTypeID() != KSErrorTypeID) return false;
  return (((KSError *)o)->_status == _status && _message->isEqual(((KSError *)o)->_message));
}

/**
 * Obtain a hash code
 * 
 * @return hash code
 */
CFHashCode KSError::getHashCode(void) const {
  return _message->getHashCode() + _status;
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSError::copyDescription(void) const {
  return CFStringCreateWithFormat(NULL, NULL, CFSTR("[%d] %@"), _status, _message->getCFTypeRef());
}


