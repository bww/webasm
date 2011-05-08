// 
// $Id: ALDirectorySourceCollection.cc 38 2009-10-28 04:09:33Z brian $
// Keystone Framework
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

#include "ALDirectorySourceCollection.hh"
#include <Keystone/KSLog.hh>
#include <Keystone/KSPointer.hh>
#include <Keystone/CFStringAdditions.hh>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

#define super ALSourceCollection

/**
 * Construct
 * 
 * @param model options
 * @param path directory path
 * @param recurse process recursively or not
 * @param filter file filter
 */
ALDirectorySourceCollection::ALDirectorySourceCollection(const ALOptionsModel *model, CFStringRef path, bool recurse, KSFileFilter *filter) : super(model) {
  _walker = new KSDirectoryPathIterator(path, recurse, filter);
}

/**
 * Clean up
 */
ALDirectorySourceCollection::~ALDirectorySourceCollection() {
  if(_walker) KSRelease(_walker);
}

CFStringRef ALDirectorySourceCollection::getDirectoryPath(void) const {
  return _walker->getDirectoryPath();
}

KSFileFilter * ALDirectorySourceCollection::getFileFilter(void) const {
  return _walker->getFileFilter();
}

bool ALDirectorySourceCollection::hasMoreSources(void) const {
  return _walker->hasMorePaths();
}

CFStringRef ALDirectorySourceCollection::copyNextSourcePath(void) {
  return _walker->copyNextPath();
}


