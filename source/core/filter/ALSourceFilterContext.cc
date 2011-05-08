// 
// $Id: ALSourceFilterContext.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALSourceFilterContext.hh"
#include <Keystone/KSLog.hh>
#include <Keystone/KSUtility.hh>

#define super KSObject

ALSourceFilterContext::ALSourceFilterContext(CFDictionaryRef paths, ALDependencyGraph *graph) {
  _paths = CFRetainSafe(paths);
  _dependencyGraph = KSRetain(graph);
}

ALSourceFilterContext::~ALSourceFilterContext() {
  CFReleaseSafe(_paths);
  KSRelease(_dependencyGraph);
}

ALDependencyGraph * ALSourceFilterContext::getDependencyGraph(void) const {
  return _dependencyGraph;
}

CFDictionaryRef ALSourceFilterContext::getPaths(void) const {
  return _paths;
}

CFStringRef ALSourceFilterContext::getSourcePath(void) const {
  return (_paths != NULL) ? (CFStringRef)CFDictionaryGetValue(_paths, kALSourceFilterSourcePathKey) : NULL;
}

CFStringRef ALSourceFilterContext::getSourceBasePath(void) const {
  return (_paths != NULL) ? (CFStringRef)CFDictionaryGetValue(_paths, kALSourceFilterSourceBasePathKey) : NULL;
}

CFStringRef ALSourceFilterContext::getDestinationPath(void) const {
  return (_paths != NULL) ? (CFStringRef)CFDictionaryGetValue(_paths, kALSourceFilterDestinationPathKey) : NULL;
}

CFStringRef ALSourceFilterContext::getDestinationBasePath(void) const {
  return (_paths != NULL) ? (CFStringRef)CFDictionaryGetValue(_paths, kALSourceFilterDestinationBasePathKey) : NULL;
}

CFStringRef ALSourceFilterContext::copyDescription(void) const {
  return CFSTR("[ALSourceFilterContext]");
}

