// 
// $Id: KSPathIterator.hh 19 2008-12-15 02:36:18Z brian $
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

#if !defined(__KEYSTONE_KSPATHITERATOR__)
#define __KEYSTONE_KSPATHITERATOR__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSObject.hh"
#include "KSFileFilter.hh"

#define KEYSTONE_CLASS_KSPathIterator "KSPathIterator"

/**
 * A path iterator produces some number of paths with successive calls to
 * copyNextPath().  How or where these paths come from is left to concrete
 * implementations.
 * 
 * @author Brian William Wolter &lt;<a href="mailto:brian@woltergroup.net">brian@woltergroup.net</a>&gt;
 * @see KSDirectoryPathIterator
 */
class KSPathIterator : public KSObject {
protected:
  KSPathIterator() { };
  void advance(void);
  
public:
  virtual ~KSPathIterator() { };
  
  virtual unsigned long getTypeID() const { return KSPathIteratorTypeID; }
  
  virtual bool hasMorePaths(void) const = 0;
  virtual CFStringRef copyNextPath(void) = 0;
  
};

#endif __KEYSTONE_KSPATHITERATOR__

