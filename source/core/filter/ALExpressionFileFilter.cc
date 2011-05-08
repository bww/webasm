// 
// $Id: ALExpressionFileFilter.cc 161 2010-12-06 23:21:36Z brian $
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

#include "ALExpressionFileFilter.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSStatus.hh>
#include <Keystone/KSUtility.hh>
#include <Keystone/CFStringAdditions.hh>

/**
 * Contruct
 * 
 * @param extensions allowed extensions
 * @param excluded excluded paths
 * @param hidden allow hidden files or not
 */
ALExpressionFileFilter::ALExpressionFileFilter(CFStringRef expression, CFArrayRef excluded, CFArrayRef excludedPrefixes, bool hidden) {
  
  _expression       = CFRetainSafe(expression);
  _excluded         = CFRetainSafe(excluded);
  _excludedPrefixes = CFRetainSafe(excludedPrefixes);
  _allowHidden      = hidden;
  _pattern          = NULL;
  
  if(_expression != NULL){
    char *cexpr = CFStringCopyCString(_expression, kDefaultStringEncoding);
    const char *error = NULL;
    int erroffset;
    if((_pattern = pcre_compile(cexpr, PCRE_UTF8, &error, &erroffset, NULL)) == NULL){
      KSLogError("Unable to compile expression: \"%@\": %s (character %d)", _expression, error, erroffset);
    }
    if(cexpr) free(cexpr);
  }
  
}

/**
 * Clean up
 */
ALExpressionFileFilter::~ALExpressionFileFilter() {
  CFReleaseSafe(_expression);
  CFReleaseSafe(_excluded);
  CFReleaseSafe(_excludedPrefixes);
  if(_pattern) pcre_free(_pattern);
}

/**
 * Obtain expression which matches file names
 * 
 * @return expression
 */
CFStringRef ALExpressionFileFilter::getExpression(void) const {
  return _expression;
}

/**
 * Obtain excluded paths
 * 
 * @return excluded paths
 */
CFArrayRef ALExpressionFileFilter::getExcludedPaths(void) const {
  return _excluded;
}

/**
 * Determine whether hidden files are permitted or not
 * 
 * @return allow hiden or not
 */
bool ALExpressionFileFilter::getAllowHidden(void) const {
  return _allowHidden;
}

/**
 * Determine if the file at the specified path is permitted.
 * 
 * @param path file path
 * @return included or not
 */
bool ALExpressionFileFilter::isFileAtPathIncluded(CFStringRef path, bool directory) {
  CFStringRef name = NULL;
  bool result = false;
  
  if(path == NULL)
    return false;
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
  
  // Allow directories after they have been checked for exclusion
  
  if(directory){
    result = true;
    goto error;
  }
  
  if(_expression != NULL){
    char *cpath = CFStringCopyCString(path, kDefaultStringEncoding);
    int subpat[10];
    
    int z;
    if((z = pcre_exec(_pattern, NULL, cpath, strlen(cpath), 0, PCRE_PARTIAL_SOFT, subpat, 10)) > 0){
      result = true;
    }
    
    if(cpath) free(cpath);
  }
  
error:
  if(name) CFRelease(name);
  
  return result;
}

