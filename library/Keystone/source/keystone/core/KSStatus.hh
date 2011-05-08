// 
// $Id: KSStatus.hh 73 2010-01-28 13:43:50Z brian $
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

#if !defined(__KEYSTONE_KSSTATUS__)
#define __KEYSTONE_KSSTATUS__ 1

#include <CoreFoundation/CoreFoundation.h>

#include <stdint.h>

typedef int32_t KSStatus;

enum {
  
  KSStatusOk                      =  0,     ///< No Error
  KSStatusError                   = -1,     ///< Undefined error
  KSStatusPermissionDenied        = -2,     ///< Permission denied
  KSStatusQuotaExceeded           = -3,     ///< File system quota exceeded
  KSStatusResourceExists          = -4,     ///< File or resource already exists
  KSStatusInterrupted             = -5,     ///< Operation was interrupted by a signal
  KSStatusInvalidOperand          = -6,     ///< Invalid operand(s)
  KSStatusIOError                 = -7,     ///< An I/O error occured
  KSStatusInvalidResource         = -8,     ///< Operation is attempting to operate on an invalid resource type
  KSStatusTooManyLinks            = -9,     ///< Too many symbolic links
  KSStatusTooManyOpenFiles        = -10,    ///< Too many open files
  KSStatusPathTooLong             = -11,    ///< The path (or one of its components) is too long
  KSStatusFileTableFull           = -12,    ///< The system file table is full
  KSStatusNoSuchResource          = -13,    ///< No such file or resource
  KSStatusBadFileDescriptor       = -14,    ///< Bad file descriptor
  KSStatusStreamClosed            = -15,    ///< Stream already closed
  KSStatusNoSuchDevice            = -16,    ///< No such device
  KSStatusConnectionReset         = -17,    ///< Socket connection reset by peer
  KSStatusNotConnected            = -18,    ///< Socket is not connected
  KSStatusTimedOut                = -19,    ///< Socket transmission timed out
  KSStatusNodeResolutionFailed    = -20,    ///< Node resolution failed
  KSStatusInvalidState            = -21,    ///< Current state is invalid
  KSStatusWouldBlock              = -22,    ///< Would block
  KSStatusBusy                    = -23,    ///< Operation cannot be completed because resource is busy
  KSStatusInfiniteLoop            = -24,    ///< An infinite loop was detected
  KSStatusConnectionRefused       = -25,    ///< The remote host refused the connection
  
};

enum {
  
  KSStatusProtocolError           = -300,   ///< Protocol error or unexpected reply from remote host
  
};

enum {
  
  KSStatusTrustNotAvailable       = -400,   ///< Trust not available
  KSStatusCertificateChainInvalid = -401,   ///< Invalid certificate chain
  KSStatusCertificateExpired      = -402,   ///< Certificate expired
  
};

enum {
  
  KSStatusUnimplemented           = -500,   ///< Not implemented / unsupported
  KSStatusInvalidParameter        = -501,   ///< Invalid parameter
  KSStatusTypeError               = -502,   ///< Invalid type for operation
  KSStatusNullPointer             = -503,   ///< Invalid null pointer encountered
  KSStatusNoSuchMethod            = -504,   ///< No such method
  KSStatusRuntimeError            = -505,   ///< Runtime error
  
};

enum {
  
  KSStatusNoSuchOption            = -1000,  ///< No such option
  
};

CFStringRef KSStatusGetMessage(KSStatus status);

#endif __KEYSTONE_KSSTATUS__



