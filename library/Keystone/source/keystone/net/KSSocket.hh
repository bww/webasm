// 
// $Id: KSSocket.hh 61 2010-01-25 02:55:22Z brian $
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

#if !defined(__KEYSTONE_KSSOCKET__)
#define __KEYSTONE_KSSOCKET__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSObject.hh"
#include "KSTypes.hh"
#include "KSStatus.hh"
#include "KSError.hh"
#include "KSAddress.hh"

#define KEYSTONE_CLASS_KSSocket "KSSocket"

typedef enum KSSocketState_ {
  kKSSocketStateDisconnected    = 0,
  kKSSocketStateConnected       = 1 << 0,
  kKSSocketStateSecure          = 1 << 1
} KSSocketState;

typedef enum KSSocketMode_ {
  kKSSocketModeBlocking         = 0,
  kKSSocketModeNonBlocking      = 1
} KSSocketMode;

/**
 * A socket connection
 * 
 * @author Brian William Wolter &lt;<a href="mailto:brian@woltergroup.net">brian@woltergroup.net</a>&gt;
 */
class KSSocket : public KSObject {
private:
  int           _sockfd;
  KSAddress     *_address;
  KSSocketState _state;
  KSSocketMode  _mode;
  bool          _closeOnDealloc;
  
protected:
  KSStatus __read(KSByte *buffer, KSLength count, KSLength *actual);
  KSStatus __write(KSByte *buffer, KSLength count, KSLength *actual);
  
public:
  KSSocket(KSAddress *address);
  KSSocket(int sockfd);
  virtual ~KSSocket();
  
  virtual unsigned long getTypeID() const { return KSSocketTypeID; }
  
  KSAddress * getRemoteAddress(void) const;
  int getFileDescriptor(void) const;
  
  KSStatus connect(CFTimeInterval timeout = -1.0, KSError **error = NULL);
  virtual KSStatus read(KSByte *buffer, KSLength count, KSLength *actual);
  virtual KSStatus write(KSByte *buffer, KSLength count, KSLength *actual);
  virtual KSStatus close(void);
  
  virtual bool isEqual(KSObject *o) const;
  virtual CFStringRef copyDescription(void) const;
  
  friend OSStatus KSSecureSocketReadCallBack(const void *context, void *data, size_t *length);
  friend OSStatus KSSecureSocketWriteCallBack(const void *context, const void *data, size_t *length);
  
};

#endif __KEYSTONE_KSSOCKET__


