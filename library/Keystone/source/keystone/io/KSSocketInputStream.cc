// 
// $Id: KSSocketInputStream.cc 53 2010-01-20 23:59:08Z brian $
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

#include "KSSocketInputStream.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "CFStringAdditions.hh"

/**
 * Construct
 * 
 * @param socket a connected socket
 */
KSSocketInputStream::KSSocketInputStream(KSSocket *socket) {
  _socket = KSRetain(socket);
}

/**
 * Clean up
 */
KSSocketInputStream::~KSSocketInputStream() {
  KSRelease(_socket);
}

/**
 * Obtain the socket from which this stream reads.
 * 
 * @return socket
 */
KSSocket * KSSocketInputStream::getSocket(void) const {
  return _socket;
}

/**
 * Read bytes from the underlying stream
 * 
 * @param buffer buffer
 * @param count bytes to read
 * @param actual on output: actual byte count read
 * @return status
 */
KSStatus KSSocketInputStream::read(KSByte *buffer, KSLength count, KSLength *actual) {
  return (_socket != NULL) ? _socket->read(buffer, count, actual) : KSStatusInvalidState;
}

/**
 * Close the underlying stream
 * 
 * @return status
 */
KSStatus KSSocketInputStream::close(void) {
  return (_socket != NULL) ? _socket->close() : KSStatusInvalidState;
}

