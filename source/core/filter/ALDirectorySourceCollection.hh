// 
// $Id: ALDirectorySourceCollection.hh 38 2009-10-28 04:09:33Z brian $
// Web Assembler
// 
// Copyright (c) 2009 Wolter Group New York, Inc., All rights reserved.
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

#if !defined(__ALDIRECTORYSOURCECOLLECTION__)
#define __ALDIRECTORYSOURCECOLLECTION__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <Keystone/KSObject.hh>
#include <Keystone/KSStatus.hh>
#include <Keystone/KSFileFilter.hh>
#include <Keystone/KSDirectoryPathIterator.hh>

#include "ALOptionsModel.hh"
#include "ALSourceCollection.hh"

class ALDirectorySourceCollection : public ALSourceCollection {
private:
  KSDirectoryPathIterator   *_walker;
  
public:
  ALDirectorySourceCollection(const ALOptionsModel *model, CFStringRef path, bool recurse = true, KSFileFilter *filter = NULL);
  virtual ~ALDirectorySourceCollection();
  
  CFStringRef getDirectoryPath(void) const;
  KSFileFilter * getFileFilter(void) const;
  
  virtual bool hasMoreSources(void) const;
  virtual CFStringRef copyNextSourcePath(void);
  
};

#endif __ALDIRECTORYSOURCECOLLECTION__

