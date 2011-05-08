// 
// $Id: ALDependencyGraph.hh 162 2011-05-08 22:01:47Z brian $
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

#if !defined(__ALDEPENDENCYGRAPH__)
#define __ALDEPENDENCYGRAPH__ 1

#include <CoreFoundation/CoreFoundation.h>

#include <Keystone/KSObject.hh>
#include <Keystone/KSStatus.hh>
#include <Keystone/KSError.hh>
#include <Keystone/KSInputStream.hh>
#include <Keystone/KSOutputStream.hh>

#define kALDependencyGraphCachePrefix   ".webasm_depend_"

class ALDependencyGraph : public KSObject {
private:
  CFStringRef     _resource;
  CFMutableSetRef _dependencies;
  
public:
  ALDependencyGraph(CFStringRef resource, CFSetRef dependencies = NULL);
  virtual ~ALDependencyGraph();
  
  static CFStringRef copyStandardDependencyCachePath(CFStringRef path);
  
  KSStatus init(KSError **error);
  KSStatus initWithContentsOfFile(CFStringRef path, KSError **error);
  KSStatus write(KSError **error) const;
  KSStatus writeToFile(CFStringRef path, KSError **error) const;
  
  CFSetRef getDependencies(void) const;
  CFSetRef getDirectDependencies(void) const;
  void addDirectDependency(CFStringRef depends);
  void removeDirectDependency(CFStringRef depends);
  void removeAllDirectDependencies(void);
  
  bool isOutOfDate(void) const;
  
  virtual CFStringRef copyDescription(void) const;
  
};

#endif __ALDEPENDENCYGRAPH__

