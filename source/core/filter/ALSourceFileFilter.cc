// 
// $Id: ALSourceFileFilter.cc 161 2010-12-06 23:21:36Z brian $
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

#include "ALSourceFileFilter.hh"

#include <Keystone/KSUtility.hh>
#include <Keystone/CFStringAdditions.hh>

/**
 * Contruct
 * 
 * @param extensions allowed extensions
 * @param excluded excluded paths
 * @param excludePrefixes excluded file name prefixes
 * @param hidden allow hidden files or not
 */
ALSourceFileFilter::ALSourceFileFilter(CFArrayRef extensions, CFArrayRef excluded, CFArrayRef excludedPrefixes, bool hidden) {
  _extensions = (extensions != NULL) ? (CFArrayRef)CFRetain(extensions) : NULL;
  _excluded = (excluded != NULL) ? (CFArrayRef)CFRetain(excluded) : NULL;
  _excludedPrefixes = (excludedPrefixes != NULL) ? (CFArrayRef)CFRetain(excludedPrefixes) : NULL;
  _allowHidden = hidden;
}

/**
 * Clean up
 */
ALSourceFileFilter::~ALSourceFileFilter() {
  CFReleaseSafe(_extensions);
  CFReleaseSafe(_excluded);
  CFReleaseSafe(_excludedPrefixes);
}

/**
 * Obtain the permitted file extensions
 * 
 * @return allowed extensions
 */
CFArrayRef ALSourceFileFilter::getAllowedExtensions(void) const {
  return _extensions;
}

/**
 * Obtain excluded paths
 * 
 * @return excluded paths
 */
CFArrayRef ALSourceFileFilter::getExcludedPaths(void) const {
  return _excluded;
}

/**
 * Determine whether hidden files are permitted or not
 * 
 * @return allow hiden or not
 */
bool ALSourceFileFilter::getAllowHidden(void) const {
  return _allowHidden;
}

/**
 * Determine if the file at the specified path is permitted.
 * 
 * @param path file path
 * @return included or not
 */
bool ALSourceFileFilter::isFileAtPathIncluded(CFStringRef path, bool directory) {
  CFStringRef name = NULL;
  bool result = true;
  
  if(path == NULL)
    return false;
  if(directory)
    return true;
  if((name = CFStringCopyLastPathComponent(path)) == NULL)
    return false;
  
  if(!_allowHidden && CFStringGetCharacterAtIndex(name, 0) == '.'){
    result = false;
    goto error;
  }
  
  if(_excluded != NULL){
    register int i;
    
    for(i = 0; i < CFArrayGetCount(_excluded); i++){
      CFStringRef excl = (CFStringRef)CFArrayGetValueAtIndex(_excluded, i);
      if(CFStringCompare(excl, path, kCFCompareCaseInsensitive) == kCFCompareEqualTo){
        result = false;
        goto error;
      }
    }
    
  }
  
  if(_excludedPrefixes != NULL){
    register int i;
    
    for(i = 0; i < CFArrayGetCount(_excludedPrefixes); i++){
      CFStringRef excl = (CFStringRef)CFArrayGetValueAtIndex(_excludedPrefixes, i);
      if(CFStringFindWithOptions(name, excl, CFRangeMake(0, KSMin(CFStringGetLength(excl), CFStringGetLength(name))), kCFCompareCaseInsensitive, NULL)){
        result = false;
        goto error;
      }
    }
    
  }
  
  if(_extensions != NULL){
    CFStringRef ext;
    bool found = false;
    
    if((ext = CFStringCopyFileNameExtension(path)) != NULL){
      register int i;
      
      for(i = 0; i < CFArrayGetCount(_extensions); i++){
        CFStringRef vext = (CFStringRef)CFArrayGetValueAtIndex(_extensions, i);
        if(CFStringCompare(vext, ext, kCFCompareCaseInsensitive) == kCFCompareEqualTo){
          found = true;
          break;
        }
      }
      
    }
    
    if(ext) CFRelease(ext);
    if(!found){
      result = false;
      goto error;
    }
  }
  
error:
  if(name) CFRelease(name);
  
  return result;
}

