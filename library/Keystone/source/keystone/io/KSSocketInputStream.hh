// 
// $Id: KSSocketInputStream.hh 53 2010-01-20 23:59:08Z brian $
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

#if !defined(__KEYSTONE_KSSOCKETINPUTSTREAM__)
#define __KEYSTONE_KSSOCKETINPUTSTREAM__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSInputStream.hh"
#include "KSTypes.hh"
#include "KSStatus.hh"
#include "KSSocket.hh"

#define KEYSTONE_CLASS_KSSocketInputStream "KSSocketInputStream"

/**
 * An input stream which uses a socket for underlying input.
 * 
 * @author BWW
 */
class KSSocketInputStream : public KSInputStream {
private:
  KSSocket *_socket;
  
public:
  KSSocketInputStream(KSSocket *socket);
  virtual ~KSSocketInputStream();
  
  virtual unsigned long getTypeID() const { return KSSocketInputStreamTypeID; }
  
  KSSocket * getSocket(void) const;
  
  virtual KSStatus read(KSByte *buffer, KSLength count, KSLength *actual);
  virtual KSStatus close(void);
  
};

#endif __KEYSTONE_KSSOCKETINPUTSTREAM__


