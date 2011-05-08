// 
// $Id: KSDirectoryPathIterator.cc 72 2010-01-28 08:42:45Z brian $
// XHTML Assembler
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

#include "KSDirectoryPathIterator.hh"
#include "KSLog.hh"
#include "KSPointer.hh"
#include "CFStringAdditions.hh"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

#define super KSPathIterator

/**
 * Construct
 * 
 * @param path directory path
 * @param recurse process recursively or not
 * @param filter file filter
 */
KSDirectoryPathIterator::KSDirectoryPathIterator(CFStringRef path, bool recurse, KSFileFilter *filter) {
  
  _base = (path != NULL) ? (CFStringRef)CFRetain(path) : NULL;
  _filter = (filter != NULL) ? (KSFileFilter *)KSRetain(filter) : NULL;
  _dstack = CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
  _nstack = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
  _current = NULL;
  
  if(_base != NULL){
    DIR *dir;
    char *ppath = CFStringCopyCString(_base, kDefaultStringEncoding);
    
    if(!(dir = opendir(ppath))){
      KSLog("Unable to open: %s (%s)\n", ppath, strerror(errno));
      return;
    }
    
    KSPointer *pdir = new KSPointer(dir);
    CFArrayAppendValue(_dstack, pdir);
    CFArrayAppendValue(_nstack, _base);
    KSRelease(pdir);
    
    advance();
  }
  
}

/**
 * Clean up
 */
KSDirectoryPathIterator::~KSDirectoryPathIterator() {
  
  if(_dstack){
    register int i;
    
    for(i = CFArrayGetCount(_dstack) - 1; i >= 0; i--){
      KSPointer *dp = (KSPointer *)CFArrayGetValueAtIndex(_dstack, i);
      closedir((DIR *)dp->getPointer());
      CFArrayRemoveValueAtIndex(_dstack, i);
    }
    
    CFRelease(_dstack);
  }
  
  if(_nstack) CFRelease(_nstack);
  if(_base) CFRelease(_base);
  if(_current) CFRelease(_current);
  if(_filter) KSRelease(_filter);
  
}

/**
 * Obtain the directory path
 * 
 * @return base path
 */
CFStringRef KSDirectoryPathIterator::getDirectoryPath(void) const {
  return _base;
}

/**
 * Obtain the file filter
 * 
 * @return file filter
 */
KSFileFilter * KSDirectoryPathIterator::getFileFilter(void) const {
  return _filter;
}

/**
 * Advance
 */
void KSDirectoryPathIterator::advance(void) {
  struct dirent *dp;
  DIR *dir;
  
  if(_current != NULL) CFRelease(_current);
  _current = NULL;
  
  CFIndex dcount;
  if((dcount = CFArrayGetCount(_dstack)) < 1)
    return;
  
  // take the top directory on the stack
  dir = (DIR *)((KSPointer *)CFArrayGetValueAtIndex(_dstack, (dcount - 1)))->getPointer();
  
  CFStringRef rpath = NULL;
  CFStringRef bpath = NULL;
  CFStringRef vpath = NULL;
  
  while(1){
    
    // if we're at the end of this directory listing, pop it off the stack and
    // recurse to process the next directory down (if any)
    if((dp = readdir(dir)) == NULL){
      closedir(dir);
      CFArrayRemoveValueAtIndex(_dstack, (dcount - 1));
      CFArrayRemoveValueAtIndex(_nstack, (dcount - 1));
      advance();
      return;
    }
    
    // skip dot and dot-dot
    if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
      continue;
    
    rpath = CFStringCreateWithCString(NULL, dp->d_name, kDefaultStringEncoding);
    bpath = CFStringCreateDelimitedListFromArray(_nstack, kPathComponentDelimiterString);
    vpath = CFStringCreateCanonicalPath(bpath, rpath);
    
    // skip excluded files
    if(_filter != NULL && !_filter->isFileAtPathIncluded(vpath, (dp->d_type & DT_DIR) == DT_DIR))
      continue;
    
    // break if we're here
    break;
  }
  
  if(dp->d_type & DT_DIR){
    
    // push the new directory on the stack and recurse
    DIR *subdir;
    char *sdpath = CFStringCopyCString(vpath, kDefaultStringEncoding);
    
    if((subdir = opendir(sdpath)) == NULL){
      KSLogError("Unable to open directory: %@", vpath);
      if(sdpath) free(sdpath);
      goto error;
    }
    
    KSPointer *psubdir = new KSPointer(subdir);
    CFArrayAppendValue(_dstack, psubdir);
    CFArrayAppendValue(_nstack, rpath);
    KSRelease(psubdir);
    
    if(sdpath) free(sdpath);
    advance();
    
  }else{
    _current = (CFStringRef)CFRetain(vpath);
  }
  
error:
  if(rpath) CFRelease(rpath);
  if(bpath) CFRelease(bpath);
  if(vpath) CFRelease(vpath);
  
}

/**
 * Determine if this source collection contains any more sources.
 * 
 * @return contains more sources or not
 */
bool KSDirectoryPathIterator::hasMorePaths(void) const {
  return (_current != NULL);
}

/**
 * Obtain the next source path.
 * 
 * @return next source path
 */
CFStringRef KSDirectoryPathIterator::copyNextPath(void) {
  if(_current != NULL){
    CFStringRef cur = CFStringCreateCopy(NULL, _current);
    advance();
    return cur;
  }else{
    return NULL;
  }
}

