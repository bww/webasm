// 
// $Id: CFStringAdditions.cc 79 2010-07-22 01:11:36Z brian $
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

#include "CFStringAdditions.hh"
#include "KSTypes.hh"
#include "KSLog.hh"
#include "KSUtility.hh"

#include <unistd.h>
#include <limits.h>

typedef struct CFStringKaleidoscopeBaseNCharacterOffset_ {
  CFRange   range;
  CFIndex   offset;
} CFStringKaleidoscopeBaseNCharacterOffset;

const char kCFStringKaleidoscopeBaseNCharacters[] = {
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '$', '.'
};

const CFIndex kCFStringKaleidoscopeBaseNCharacterOffsetCount = 5;
const CFStringKaleidoscopeBaseNCharacterOffset kCFStringKaleidoscopeBaseNCharacterOffsets[] = {
  { { 36,  1 }, 62 },
  { { 46,  1 }, 63 },
  { { 48, 10 },  0 },
  { { 65, 26 }, 10 },
  { { 97, 26 }, 36 }
};

int CFStringBaseNStringGetPowerOfTwo(int n);

/**
 * Allocate for and make a string copy
 * 
 * @param string the string top copy to a cstring
 * @param encoding character encoding
 * @return cstring
 */
char * CFStringCopyCString(CFStringRef string, CFStringEncoding encoding) {
  CFIndex slen;
  char *cstring;
  
  slen = (CFStringGetLength(string) * 2) + 1;
  cstring = (char *)malloc(slen);
  
  if(!CFStringGetCString(string, cstring, slen, encoding)){
    free(cstring);
    cstring = NULL;
  }
  
  return cstring;
}

/**
 * Allocate for and make a copy of a string's bytes in the specified encoding.
 * 
 * @param string the string
 * @param encoding character encoding
 * @return string bytes
 */
CFDataRef CFStringCopyBytes(CFStringRef string, CFStringEncoding encoding) {
  CFIndex length;
  KSByte *bytes = NULL;
  CFDataRef data = NULL;
  
  if(string == NULL)
    return NULL;
  
  // get the length in bytes
  CFStringGetBytes(string, CFRangeMake(0, CFStringGetLength(string)), encoding, '?', false, NULL, 0, &length);
  
  if((bytes = (KSByte *)malloc(length)) == NULL){
    KSLogError("Unable to allocate for buffer");
    goto error;
  }
  
  // make the conversion
  CFStringGetBytes(string, CFRangeMake(0, CFStringGetLength(string)), encoding, '?', false, bytes, length, &length);
  
  if((data = CFDataCreate(NULL, bytes, length)) == NULL){
    KSLogError("Unable to create data");
    goto error;
  }
  
error:
  if(bytes != NULL) free(bytes);
  
  return data;
}

/**
 * Copy a delimited list expressed as a string.  The string is separated into
 * components, which are returned.
 * 
 * @param string the list
 * @param delimiter list delimiter
 * @return list components
 */
CFArrayRef CFStringCreateArrayFromDelimitedList(CFStringRef string, CFStringRef delimiter) {
  CFMutableArrayRef results = NULL;
  
  if(string == NULL || delimiter == NULL)
    return NULL;
  
  CFArrayRef parts;
  if((parts = CFStringCreateArrayBySeparatingStrings(NULL, string, delimiter)) == NULL)
    return CFArrayCreate(NULL, (const void **)&string, 1, &kCFTypeArrayCallBacks);
  
  results = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
  
  register int i;
  for(i = 0; i < CFArrayGetCount(parts); i++){
    CFStringRef part = (CFStringRef)CFArrayGetValueAtIndex(parts, i);
    CFStringRef trimmed = CFStringCreateTrimmedString(part);
    CFArrayAppendValue(results, trimmed);
    CFRelease(trimmed);
  }
  
  CFRelease(parts);
  return results;
}

/**
 * Create an array of strings separated by the specified characters.
 * 
 * @param string a component string
 * @param delimiter component delimiter characters
 * @param escape when not null, escape characters used to escape delimiter
 * characters which should not be treated as delimiters in the string. The
 * escape characters also escape themselves (non-literalized escape characters
 * are removed.)
 * @return components
 */
CFArrayRef CFStringCreateArrayFromComponentList(CFStringRef string, CFCharacterSetRef delimiter, CFCharacterSetRef escape) {
  CFMutableArrayRef results = NULL;
  CFMutableStringRef buffer = NULL;
  int esc = 0;
  
  if(string == NULL || delimiter == NULL)
    return NULL;
  
  results = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
  
  register int i;
  for(i = 0; i < CFStringGetLength(string); i++){
    UniChar c = CFStringGetCharacterAtIndex(string, i);
    
    if(CFCharacterSetIsCharacterMember(delimiter, c)){
      if((esc % 2) == 0 && buffer != NULL){
        CFArrayAppendValue(results, buffer);
        CFRelease(buffer);
        buffer = NULL;
      }
    }else if(escape != NULL && CFCharacterSetIsCharacterMember(escape, c)){
      if((esc % 2) != 0 && buffer != NULL){
        CFStringAppendCharacters(buffer, &c, 1);
      }
      esc++;
    }else{
      if(buffer == NULL) buffer = CFStringCreateMutable(NULL, 0);
      CFStringAppendCharacters(buffer, &c, 1);
      esc = 0;
    }
    
  }
  
  if(buffer != NULL){
    CFArrayAppendValue(results, buffer);
    CFRelease(buffer);
  }
  
  return results;
}

/**
 * Create a delimited list from the specified array of values.
 * 
 * @param items values
 * @param delimiter delimiter
 * @return delimited list
 */
CFStringRef CFStringCreateDelimitedListFromArray(CFArrayRef items, CFStringRef delimiter) {
  CFMutableStringRef list = CFStringCreateMutable(NULL, 0);
  
  register int i;
  for(i = 0; i < CFArrayGetCount(items); i++){
    if(i > 0) CFStringAppend(list, delimiter);
    CFStringAppend(list, (CFStringRef)CFArrayGetValueAtIndex(items, i));
  }
  
  return list;
}

/**
 * Escape a string by prefixing every instance of certain characters with
 * another character.  The obvious application of this is to escape quotation
 * marks by prefixing a '\' character.  To prevent double-escapes, the escape
 * character is checked to make sure it is balanced.  For example, escaping "'"
 * with "\":
 * 
 * "I'm a string" becomes "I\'m a string",
 * "I\'m a string" becomes "I\'m a string",
 * "I\\'m a string" becomes "I\\\'m a string",
 * and so fourth.
 * 
 * @param string a string
 * @param escape characters to escape
 * @param prefix escape prefix (e.g., '\')
 * @return escaped string
 */
CFStringRef CFStringCreateEscapedString(CFStringRef string, CFCharacterSetRef escape, UniChar prefix) {
  CFMutableStringRef buffer = NULL;
  int esc = 0;
  
  if(string == NULL || escape == NULL){
    return NULL;
  }
  
  buffer = CFStringCreateMutable(NULL, 0);
  
  register int i;
  for(i = 0; i < CFStringGetLength(string); i++){
    UniChar v = CFStringGetCharacterAtIndex(string, i);
    
    if(CFCharacterSetIsCharacterMember(escape, v)){
      if((esc % 2) == 0) CFStringAppendCharacters(buffer, &prefix, 1);
      esc = 0;
    }else if(v == prefix){
      esc++;
    }else{
      esc = 0;
    }
    
    CFStringAppendCharacters(buffer, &v, 1);
  }
  
  return buffer;
}

/**
 * Create a string from the proided string with whitespace removed
 * from both ends.
 * 
 * @param string the string
 * @return trimmed string
 */
CFStringRef CFStringCreateTrimmedString(CFStringRef string) {
  int lb = 0, ub = 0;
  size_t length = CFStringGetLength(string);
  CFCharacterSetRef white = CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline);
  
  for(lb = 0; lb < length; lb++){
    UniChar uc = CFStringGetCharacterAtIndex(string, lb);
    if(!CFCharacterSetIsCharacterMember(white, uc))
      break;
  }
  
  for(ub = length - 1; ub >= 0; ub--){
    UniChar uc = CFStringGetCharacterAtIndex(string, ub);
    if(!CFCharacterSetIsCharacterMember(white, uc))
      break;
  }
  
  // if the upper and lower bounds are the same, there's no string left
  if(lb == 0 && ub == length - 1){
    return CFStringCreateCopy(NULL, string);
  }else{
    return CFStringCreateWithSubstring(NULL, string, CFRangeMake(lb, (ub - lb) + 1));
  }
  
}

/**
 * Copy the default temporary directory path.
 * 
 * @return default temporary directory path
 */
CFStringRef CFStringCopyTemporaryDirectory(void) {
  int  blen = 1024;
  char buffer[blen];
  
  size_t len;
  if((len = confstr(_CS_DARWIN_USER_TEMP_DIR, buffer, blen - 1)) < 0){
    KSLogError("Unable to obtain temporary directory via _CS_DARWIN_USER_TEMP_DIR: %s", strerror(errno));
    return NULL;
  }
  
  return CFStringCreateWithCString(NULL, buffer, kDefaultStringEncoding);
}

/**
 * Create a temporary file path with the specified file name under
 * the default temporary directory.
 * 
 * @param name file name
 * @return file path
 */
CFStringRef CFStringCreateTemporaryFilePath(CFStringRef name) {
  CFStringRef path = NULL;
  
  CFStringRef tempdir;
  if((tempdir = CFStringCopyTemporaryDirectory()) != NULL){
    path = CFStringCreateCanonicalPath(tempdir, name);
    CFReleaseSafe(tempdir);
  }
  
  return path;
}

/**
 * Copy the current user's home directory
 * 
 * @return home directory
 */
CFStringRef CFStringCopyUserHomeDirectory(void) {
  CFStringRef path = NULL;
  
  char *home;
  if((home = getenv("HOME")) != NULL){
    path = CFStringCreateWithCString(NULL, home, kDefaultStringEncoding);
  }
  
  return path;
}

/**
 * Create a path under the current user's home directory
 * 
 * @param relative relative path under the user home root
 * @return file path
 */
CFStringRef CFStringCreateUserHomeFilePath(CFStringRef relative) {
  CFStringRef path = NULL;
  
  CFStringRef home;
  if((home = CFStringCopyUserHomeDirectory()) != NULL){
    path = CFStringCreateCanonicalPath(home, relative);
    CFReleaseSafe(home);
  }
  
  return path;
}

/**
 * Create a canonical (absolute) path by resolving the specified path
 * against the provided base.  If the path is already absolute, a copy
 * of the path is simply returned.  If for some reason the path is not
 * valid, NULL is returned.
 * 
 * Note that relative components in the base path are treated as literal
 * components.  That is, ".." in "/Users/ethan/.." is treated as a regular
 * directory with the literal name "..", not "ascend".
 * 
 * For example:
 * "/",             "some/file.html"    becomes "/some/file.html"
 * "/",             "./some/file.html"  becomes "/some/file.html"
 * "/Users/ethan",  "some/file.html"    becomes "/Users/ethan/some/file.html"
 * "/Users/ethan",  "../../file.html"   becomes "/file.html"
 * "/Users/ethan",  "/some/file.html"   becomes "/some/file.html"
 * "/"              "../../file.html"   becomes NULL
 * 
 * @param base the base path
 * @param path the path
 * @return canonical path
 */
CFStringRef CFStringCreateCanonicalPath(CFStringRef base, CFStringRef path) {
  CFMutableStringRef buffer = NULL;
  CFMutableArrayRef absparts = NULL;
  CFCharacterSetRef delimiter = NULL;
  CFCharacterSetRef escape = NULL;
  register int i;
  
  if(path == NULL || base == NULL)
    return NULL;
  
  // - if the path is empty, return the base
  // - if the path absolute, return the path
  
  if(CFStringGetLength(path) < 1){
    return CFStringCreateCopy(NULL, base);
  }else if(CFStringGetCharacterAtIndex(path, 0) == kPathComponentDelimiter){
    return CFStringCreateCopy(NULL, path);
  }
  
  delimiter = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("/"));
  escape = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("\\"));
  
  CFArrayRef bparts = CFStringCreateArrayFromComponentList(base, delimiter, escape);
  CFArrayRef pparts = CFStringCreateArrayFromComponentList(path, delimiter, escape);
  CFIndex pcount = CFArrayGetCount(pparts);
  
  absparts = CFArrayCreateMutableCopy(NULL, 0, bparts);
  
  for(i = 0; i < pcount; i++){
    CFStringRef ppart = (CFStringRef)CFArrayGetValueAtIndex(pparts, i);
    
    if(CFStringCompare(ppart, CFSTR("."), 0) == kCFCompareEqualTo){
      // Ignore '.'
    }else if(CFStringCompare(ppart, CFSTR(".."), 0) == kCFCompareEqualTo){
      int abscount = CFArrayGetCount(absparts);
      
      // Make sure we aren't attempting to acend further than root.
      // If so, the path is invalid; clean up and return NULL.
      if(abscount < 1) goto error;
      
      // Otherwise, remove the component we're acending out of
      CFArrayRemoveValueAtIndex(absparts, abscount - 1);
      
    }else{
      CFArrayAppendValue(absparts, ppart);
    }
    
  }
  
  // Build the path
  
  buffer = CFStringCreateMutable(NULL, 0);
  
  for(i = 0; i < CFArrayGetCount(absparts); i++){
    CFStringAppend(buffer, kPathComponentDelimiterString);
    CFStringAppend(buffer, (CFStringRef)CFArrayGetValueAtIndex(absparts, i));
  }
  
error:
  if(absparts)  CFRelease(absparts);
  if(delimiter) CFRelease(delimiter);
  if(escape)    CFRelease(escape);
  if(bparts)    CFRelease(bparts);
  if(pparts)    CFRelease(pparts);
  
  return buffer;
}

/**
 * Create a relative path from a canonical path in relation to the specified
 * base path.
 * 
 * @param base canonical base path
 * @param path canonical path
 * @return relative path
 */
CFStringRef CFStringCreateRelativePath(CFStringRef base, CFStringRef path) {
  return CFStringCreateRelativePathWithOptions(base, path, 0, NULL, NULL, NULL);
}

/**
 * Create a relative path from a canonical path in relation to the specified
 * base path.
 * 
 * @param base canonical base path
 * @param path canonical path
 * @param options path options
 * @param diverge divergence component index
 * @param ascent count
 * @param trailing trailing component count (following ascents)
 * @return relative path
 */
CFStringRef CFStringCreateRelativePathWithOptions(CFStringRef base, CFStringRef path, CFStringPathOptions options, CFIndex *diverge, CFIndex *ascents, CFIndex *trailing) {
  CFMutableStringRef buffer = NULL;
  CFCharacterSetRef delimiter = NULL;
  CFCharacterSetRef escape = NULL;
  register int i;
  
  if(path == NULL || base == NULL)
    return NULL;
  
  // - if the path is empty, return the base
  // - if the path is not absolute, return the path
  
  if(CFStringGetLength(path) < 1){
    return CFStringCreateCopy(NULL, base);
  }else if(CFStringGetCharacterAtIndex(path, 0) != kPathComponentDelimiter){
    return CFStringCreateCopy(NULL, path);
  }
  
  delimiter = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("/"));
  escape = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("\\"));
  
  CFArrayRef bparts = CFStringCreateArrayFromComponentList(base, delimiter, escape);
  CFIndex bcount = CFArrayGetCount(bparts);
  CFArrayRef pparts = CFStringCreateArrayFromComponentList(path, delimiter, escape);
  CFIndex pcount = CFArrayGetCount(pparts);
  
  CFIndex d = 0;
  CFIndex a = 0;
  
  // Skip matching components
  
  for(i = 0; i < pcount && i < bcount; i++){
    CFStringRef ppart = (CFStringRef)CFArrayGetValueAtIndex(pparts, i);
    CFStringRef bpart = (CFStringRef)CFArrayGetValueAtIndex(bparts, i);
    if(CFStringCompare(ppart, bpart, 0) != kCFCompareEqualTo) break;
  }
  
  // Note divergence and ascents
  
  d = i;
  a = KSMax(0, (bcount - d));
  
  if(diverge)   *diverge  = d;
  if(ascents)   *ascents  = a;
  if(trailing)  *trailing = (pcount - d);
  
  buffer = CFStringCreateMutable(NULL, 0);
  
  // Add ascents
  
  for(i = 0; i < a; i++){
    if(i > 0) CFStringAppend(buffer, kPathComponentDelimiterString);
    CFStringAppend(buffer, CFSTR(".."));
  }
  
  // Add remaining components
  
  for(i = d; i < CFArrayGetCount(pparts); i++){
    if(i > d || a > 0) CFStringAppend(buffer, kPathComponentDelimiterString);
    CFStringAppend(buffer, (CFStringRef)CFArrayGetValueAtIndex(pparts, i));
  }
  
error:
  if(delimiter) CFRelease(delimiter);
  if(escape)    CFRelease(escape);
  if(bparts)    CFRelease(bparts);
  if(pparts)    CFRelease(pparts);
  
  return buffer;
}

/**
 * Create a path by relocating an absolute path to a new base or root.
 * For example, the path "/path/to/some/file" with a new base path of
 * "/somewhere/else" would result in "/somewhere/else/some/file".
 * 
 * If a relative path is requested, relative path components are used
 * instead, e.g., a path "/path/to/some/file" with the base
 * "/path/to/another" becomes "../another/file" and the same path
 * with the base "/path/to/some/deeper/base" becomes "deeper/base/file".
 * 
 * If the path is relative, it is canonicalized under the new base, e.g.:
 * "some/file" with a new base of "/base" becomes "/base/some/file".
 * 
 * If the base path is relative, the original path is returned, since
 * this is too ambiguous.
 * 
 * Path delimiters (that is, '/') escaped with the '\' character are
 * treated as literal slashes.  A double-backslash is interpreted as
 * a literal backslash.
 * 
 * Because this is primarily a Mac OS implementation, path components
 * are not case sensitive.
 * 
 * @param path a path
 * @param newbase a new base to relocate the path to
 * @param relative create a relocated path relative to the original path
 * @return relocated path
 */
CFStringRef CFStringCreateByRelocatingPath(CFStringRef path, CFStringRef newbase, Boolean relative) {
  CFMutableStringRef buffer = NULL;
  CFCharacterSetRef delimiter = NULL;
  CFCharacterSetRef escape = NULL;
  register int i;
  
  if(path == NULL || newbase == NULL)
    return NULL;
  
  // - if the path is empty, return the new base
  // - if the path is not absolute, just add it to the base
  
  if(CFStringGetLength(path) < 1){
    return CFStringCreateCopy(NULL, newbase);
  }else if(CFStringGetCharacterAtIndex(path, 0) != kPathComponentDelimiter){
    return CFStringCreateCanonicalPath(newbase, path);
  }
  
  delimiter = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("/"));
  escape = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("\\"));
  
  int shared = 0;
  
  CFArrayRef oldparts = CFStringCreateArrayFromComponentList(path, delimiter, escape);
  CFIndex oldcount = CFArrayGetCount(oldparts);
  CFArrayRef newparts = CFStringCreateArrayFromComponentList(newbase, delimiter, escape);
  CFIndex newcount = CFArrayGetCount(newparts);
  
  buffer = CFStringCreateMutable(NULL, 0);
  
  for(i = 0; i < oldcount && i < newcount; i++){
    CFStringRef oldpart = (CFStringRef)CFArrayGetValueAtIndex(oldparts, i);
    CFStringRef newpart = (CFStringRef)CFArrayGetValueAtIndex(newparts, i);
    if(CFStringCompare(oldpart, newpart, kCFCompareCaseInsensitive) == kCFCompareEqualTo){
      shared++;
    }else{
      break;
    }
  }
  
  int oldspill = KSMax(0, oldcount - shared - 1);
  int oldcopy  = KSMax(1, oldcount - newcount);
  int newspill = KSMax(0, newcount - shared);
  int godowns  = KSMax(0, oldspill);
  
  // Do the base
  
  if(relative){
    for(i = 0; i < godowns; i++){
      if(i > 0) CFStringAppend(buffer, kPathComponentDelimiterString);
      CFStringAppend(buffer, CFSTR(".."));
    }
  }else{
    for(i = 0; i < shared; i++){
      CFStringRef newpart = (CFStringRef)CFArrayGetValueAtIndex(newparts, i);
      CFStringAppend(buffer, kPathComponentDelimiterString);
      CFStringAppend(buffer, newpart);
    }
  }
  
  // Do the relocation parts
  
  for(i = 0; i < newspill; i++){
    CFStringRef newpart = (CFStringRef)CFArrayGetValueAtIndex(newparts, shared + i);
    if(i > 0 || !(relative && godowns < 1)) CFStringAppend(buffer, kPathComponentDelimiterString);
    CFStringAppend(buffer, newpart);
  }
  
  // Do original parts
  
  for(i = 0; i < oldcopy; i++){
    CFStringRef oldpart = (CFStringRef)CFArrayGetValueAtIndex(oldparts, oldcount - oldcopy + i);
    if(!relative || oldcopy > 1 || newspill > 0) CFStringAppend(buffer, kPathComponentDelimiterString);
    CFStringAppend(buffer, oldpart);
  }
  
  if(delimiter) CFRelease(delimiter);
  if(escape)    CFRelease(escape);
  
  return buffer;
}

/**
 * Create a path by moving the last component in a path from one
 * base directory to another
 * 
 * @param path a path
 * @param newbase new directory
 * @param relative create a new path relative to the original path
 * @return moved path
 */
CFStringRef CFStringCreateByMovingLastPathComponent(CFStringRef path, CFStringRef newbase, Boolean relative) {
  CFMutableStringRef buffer = NULL;
  CFCharacterSetRef delimiter = NULL;
  CFCharacterSetRef escape = NULL;
  register int i;
  
  if(path == NULL || newbase == NULL)
    return NULL;
  
  // - if the path is empty, return the new base
  // - if the path is not absolute, just add it to the base
  
  if(CFStringGetLength(path) < 1){
    return CFStringCreateCopy(NULL, newbase);
  }else if(CFStringGetCharacterAtIndex(path, 0) != kPathComponentDelimiter){
    return CFStringCreateCanonicalPath(newbase, path);
  }
  
  delimiter = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("/"));
  escape = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("\\"));
  
  int shared = 0;
  
  CFArrayRef oldparts = CFStringCreateArrayFromComponentList(path, delimiter, escape);
  CFIndex oldcount = CFArrayGetCount(oldparts);
  CFArrayRef newparts = CFStringCreateArrayFromComponentList(newbase, delimiter, escape);
  CFIndex newcount = CFArrayGetCount(newparts);
  
  buffer = CFStringCreateMutable(NULL, 0);
  
  for(i = 0; i < oldcount && i < newcount; i++){
    CFStringRef oldpart = (CFStringRef)CFArrayGetValueAtIndex(oldparts, i);
    CFStringRef newpart = (CFStringRef)CFArrayGetValueAtIndex(newparts, i);
    if(CFStringCompare(oldpart, newpart, kCFCompareCaseInsensitive) == kCFCompareEqualTo){
      shared++;
    }else{
      break;
    }
  }
  
  int oldspill = KSMax(0, oldcount - shared - 1);
  int oldcopy  = 1;
  int newspill = KSMax(0, newcount - shared);
  int godowns  = KSMax(0, oldspill);
  
  // Do the base
  
  if(relative){
    for(i = 0; i < godowns; i++){
      if(i > 0) CFStringAppend(buffer, kPathComponentDelimiterString);
      CFStringAppend(buffer, CFSTR(".."));
    }
  }else{
    for(i = 0; i < shared; i++){
      CFStringRef newpart = (CFStringRef)CFArrayGetValueAtIndex(newparts, i);
      CFStringAppend(buffer, kPathComponentDelimiterString);
      CFStringAppend(buffer, newpart);
    }
  }
  
  // Do the relocation parts
  
  for(i = 0; i < newspill; i++){
    CFStringRef newpart = (CFStringRef)CFArrayGetValueAtIndex(newparts, shared + i);
    if(i > 0 || !(relative && godowns < 1)) CFStringAppend(buffer, kPathComponentDelimiterString);
    CFStringAppend(buffer, newpart);
  }
  
  // Do original parts
  
  for(i = 0; i < oldcopy; i++){
    CFStringRef oldpart = (CFStringRef)CFArrayGetValueAtIndex(oldparts, oldcount - oldcopy + i);
    if(!relative || godowns > 0 || oldcopy > 1 || newspill > 0) CFStringAppend(buffer, kPathComponentDelimiterString);
    CFStringAppend(buffer, oldpart);
  }
  
  if(delimiter) CFRelease(delimiter);
  if(escape)    CFRelease(escape);
  
  return buffer;
}

/**
 * Obtain an array of the path components of the specified path
 * 
 * @param path the path
 * @return path components
 */
CFArrayRef CFStringCreateArrayWithPathComponents(CFStringRef path) {
  return CFStringCreateArrayBySeparatingStrings(NULL, path, kPathComponentDelimiterString);
}

/**
 * Copy and return the directory portion of a path.  That is, the entire
 * path up to and including the last path delimiter character.
 * 
 * @param path the path
 * @return path directory
 */
CFStringRef CFStringCopyPathDirectory(CFStringRef path) {
  CFStringRef dir = NULL;
  
  if(!path) return NULL;
  
  CFRange range;
  if(CFStringFindWithOptions(path, kPathComponentDelimiterString, CFRangeMake(0, CFStringGetLength(path) - 1), kCFCompareBackwards, &range) && range.length > 0){
    // path contains multiple components
    dir = CFStringCreateWithSubstring(NULL, path, CFRangeMake(0, range.location));
  }else{
    // path is a single, relative component
    dir = CFStringCreateCopy(NULL, path);
  }
  
  return dir;
}

/**
 * Copy and return the filename extension of the specified path
 * 
 * @param path the path
 * @return filename extension, or NULL if the path has no extension
 */
CFStringRef CFStringCopyFileNameExtension(CFStringRef path) {
  CFStringRef extension = NULL;
  
  if(!path) return NULL;
  
  CFRange range;
  if(CFStringFindWithOptions(path, CFSTR("."), CFRangeMake(1, CFStringGetLength(path) - 1), kCFCompareBackwards, &range)){
    CFIndex location = range.location + range.length;
    extension = CFStringCreateWithSubstring(NULL, path, CFRangeMake(location, CFStringGetLength(path) - location));
  }
  
  return extension;
}

/**
 * Copy and return the last path component (presumably the file name)
 * from the specified path.  For example (the last component is
 * [bracketed]):
 * 
 * /path/to/[file.html]
 * /path/to/[directory]
 * /path/to/directory/[]
 * ../../[file.html]
 * [file.html]
 * [/]
 * 
 * @param path the path
 * @return last path component
 */
CFStringRef CFStringCopyLastPathComponent(CFStringRef path) {
  CFStringRef last = NULL;
  
  if(!path) return NULL;
  
  CFRange range;
  if(CFStringFindWithOptions(path, CFSTR("/"), CFRangeMake(0, CFStringGetLength(path)), kCFCompareBackwards, &range)){
    CFIndex location = range.location + range.length;
    last = CFStringCreateWithSubstring(NULL, path, CFRangeMake(location, CFStringGetLength(path) - location));
  }else{
    last = CFStringCreateCopy(NULL, path);
  }
  
  return last;
}

/**
 * Create a condensed copy of a string which does not exceed a specified length
 * and from which extraneous whitespace is removed.
 * 
 * @param string a string
 * @param options condensation options
 * @param maxlen maximum length of the output string
 * @return condensed string
 */
CFStringRef CFStringCreateCondensedCopy(CFStringRef string, CFStringCondensationOptions options, CFIndex maxlen) {
  CFMutableStringRef buffer = CFStringCreateMutable(NULL, 0);
  bool allwhite = true;
  
  if(string != NULL){
    CFCharacterSetRef condense = CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline);
    CFCharacterSetRef breaks   = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("\r\n"));
    UniChar lastchar = 0;
    CFIndex actual = 0;
    
    register int i;
    for(i = 0; (actual < maxlen || maxlen < 1) && i < CFStringGetLength(string); i++){
      UniChar c = CFStringGetCharacterAtIndex(string, i);
      
      if(!CFCharacterSetIsCharacterMember(condense, c)){
        allwhite = false;
      }
      
      if(CFCharacterSetIsCharacterMember(breaks, c) && !((options & CFStringCondensationOptionIgnoreBreaks) == CFStringCondensationOptionIgnoreBreaks)){
        if(!CFCharacterSetIsCharacterMember(breaks, lastchar) && actual != 0){
          CFStringAppendCharacters(buffer, &c, 1);
          lastchar = c;
          actual++;
        }
      }else{
        if(CFCharacterSetIsCharacterMember(condense, c)) c = ' ';
        if(!(c == ' ' && lastchar == ' ') && !(actual == 0 && c == ' ')){
          CFStringAppendCharacters(buffer, &c, 1);
          lastchar = c;
          actual++;
        }
      }
      
    }
    
    if(allwhite && (actual < maxlen || maxlen < 1)){
      UniChar c = ' ';
      CFStringAppendCharacters(buffer, &c, 1);
    }
    
    if(CFStringGetLength(string) > i) CFStringAppend(buffer, CFSTR("..."));
    
    if(breaks)   CFRelease(breaks);
  }
  
  return buffer;
}

/**
 * Copy a mangled name for the specified string.  The mangled name will replace
 * non-alphanumeric characters with some other character.
 * 
 * @param string name to mangle
 * @param replacement the replacement character
 * @return mangled name
 */
CFStringRef CFStringCopyMangledName(CFStringRef string, UniChar replacement) {
  return CFStringCopyMangledNameForCharacterSet(string, CFCharacterSetGetPredefined(kCFCharacterSetAlphaNumeric), replacement);
}

/**
 * Copy a mangled name for the specified string.  The mangled name will replace
 * characters outside the allowed set with some other character.
 * 
 * @param string name to mangle
 * @param allowed characters allowed in the name
 * @param replacement the replacement character
 * @return mangled name
 */
CFStringRef CFStringCopyMangledNameForCharacterSet(CFStringRef string, CFCharacterSetRef allowed, UniChar replacement) {
  CFMutableStringRef buffer = NULL;
  
  if(string != NULL && allowed != NULL){
    buffer = CFStringCreateMutable(NULL, 0);
    
    register int i;
    for(i = 0; i < CFStringGetLength(string); i++){
      UniChar c = CFStringGetCharacterAtIndex(string, i);
      if(!CFCharacterSetIsCharacterMember(allowed, c)){
        CFStringAppendCharacters(buffer, &replacement, 1);
      }else{
        CFStringAppendCharacters(buffer, &c, 1);
      }
    }
    
  }
  
  return buffer;
}

/**
 * Append a range of characters from one string to another
 * 
 * @param destination the string to append to
 * @param string the source string
 * @param range character range in the source string
 */
void CFStringAppendSubstring(CFMutableStringRef destination, CFStringRef string, CFRange range) {
  
  if(destination == NULL || string == NULL || range.location < 0 || (range.location + range.length) > CFStringGetLength(string)){
    KSLogError("Invalid state; cannot append character range");
    return;
  }
  
  CFStringRef substring;
  if((substring = CFStringCreateWithSubstring(NULL, string, range)) == NULL){
    KSLogError("Unable to create substring; cannot append character range");
  }else{
    CFStringAppend(destination, substring);
    CFRelease(substring);
  }
  
}

/**
 * Create a string which represents, as a string of hex numbers, the
 * arbitrary data provided.
 * 
 * @param a allocator
 * @param d the data
 * @return string representation
 */
CFStringRef CFStringCreateHexStringWithData(CFAllocatorRef a, CFDataRef d) {
  return CFStringCreateHexStringWithBytes(a, CFDataGetBytePtr(d), CFDataGetLength(d), ' ');
}

/**
 * Create a string which represents, as a string of hex numbers, the
 * arbitrary bytes provided.
 * 
 * @param a allocator
 * @param b the bytes
 * @param n number of bytes
 * @param s separator character (use '\0' for no separator)
 * @return string representation
 */
CFStringRef CFStringCreateHexStringWithBytes(CFAllocatorRef a, const void *b, size_t n, char s) {
  return CFStringCreateBaseNStringWithBytes(a, 16, b, n, s);
}

/**
 * Create a string which represents, as a string of base32 numbers, the
 * arbitrary data provided.
 * 
 * @param a allocator
 * @param d the data
 * @return string representation
 */
CFStringRef CFStringCreateBaseNStringWithData(CFAllocatorRef a, int base, CFDataRef d) {
  return CFStringCreateBaseNStringWithBytes(a, base, CFDataGetBytePtr(d), CFDataGetLength(d), ' ');
}

/**
 * Create a string which represents, as a string of base32 numbers, the
 * arbitrary bytes provided.
 * 
 * @param a allocator
 * @param b destination base (must be power of two where  2 >= n <= 64).  If you
 * provide a base that is not a power of two, the next hightest power of two
 * will be used (e.g., base 33 will actually return base 64).
 * @param b the bytes
 * @param n number of bytes
 * @param s byte separator character (use '\0' for no separator).  This is only
 * used if the base can be divided evenly into bytes (e.g., bases 2, 4, 16, but
 * not 8, 32, 64).
 * @return string representation
 */
CFStringRef CFStringCreateBaseNStringWithBytes(CFAllocatorRef a, int base, const void *b, size_t n, char s) {
  
  if(base < 2 || base > 64)
    return NULL;
  
  int zshift = CFStringBaseNStringGetPowerOfTwo(base);
  Boolean useSeparator = (8 % zshift) == 0;
  
  int lbits  = n * 8;
  int cbits  = 0;
  int oshift = 0;
  
  char *dstr = (char *)malloc((sizeof(char) * ((n * ceilf(8.0 / (float)zshift)) + ((useSeparator && s != 0) ? n : 0))) + 1);
  const uint8_t *bytes = (const unsigned char *)b;
  
  register int i, j;
  for(i = 0, j = 0; i < lbits; ){
    int offset = i % 8;
    int remaining = 8 - offset;
    int count = KSMin((zshift - oshift), remaining);
    int mask = (int)pow(2, count) - 1;
    uint8_t byte = *(bytes + (i / 8)); 
    uint8_t bits;
    
    bits = ((byte >> (remaining - count)) & mask) << (zshift - oshift - count);
    cbits |= bits;
    oshift = oshift + count;
    
    if(oshift >= zshift){
      dstr[j++] = kCFStringKaleidoscopeBaseNCharacters[cbits];
      cbits  = 0;
      oshift = 0;
    }
    
    i += count;
    
    if(useSeparator && s != 0 && (i + 1) < lbits && i > 0 && (i % 8) == 0)
      dstr[j++] = s;
    
  }
  
  if(cbits != 0){
    dstr[j++] = kCFStringKaleidoscopeBaseNCharacters[cbits];
  }
  
  dstr[j] = 0;
  
  CFStringRef string = CFStringCreateWithCString(a, dstr, kCFStringEncodingUTF8);
  free(dstr);
  
  return string;
}

/**
 * Decode a base-n string and return the data it represents.  Any characters
 * encountered in the string which are out of the range of the base are
 * discarded.
 * 
 * @param a allocator
 * @param base the base the string uses
 * @param string a base-n string
 * @return represented data
 */
CFDataRef CFStringCreateDataWithBaseNString(CFAllocatorRef a, int base, CFStringRef string) {
  CFDataRef data = NULL;
  
  size_t length = 0;
  CFStringCopyBytesFromBaseNString(NULL, base, NULL, 0, &length, string);
  
  uint8_t *buffer = (uint8_t *)malloc(sizeof(uint8_t) * length);
  CFStringCopyBytesFromBaseNString(NULL, base, buffer, length, &length, string);
  
  if(buffer != NULL){
    data = CFDataCreate(NULL, buffer, length);
    free(buffer);
  }
  
  return data;
}

/**
 * Decode a base-n string and copy the data it represents into the provided
 * buffer.Any characters encountered in the string which are out of the range of
 * the base are discarded.
 * 
 * @param a allocator
 * @param base the base the string uses
 * @param buffer the buffer into which the data will be copied.  To obtain the
 * length of the output data without processing it, pass <code>NULL</code> as
 * the buffer and use a capacity of 0.
 * @param capacity capacity of the buffer.  To obtain the length of the output
 * data without processing it, pass <code>NULL</code> as the buffer and use a
 * capacity of 0.
 * @param length on output: the actual length of the converted data
 * @param string a base-n string
 * @return represented data
 */
void CFStringCopyBytesFromBaseNString(CFAllocatorRef a, int base, const void *buffer, size_t capacity, size_t *length, CFStringRef string) {
  
  if(base < 2 || base > 64)
    return;
  
  int zshift  = CFStringBaseNStringGetPowerOfTwo(base);
  int zlength = CFStringGetLength(string);
  int olength = floorf((float)CFStringGetLength(string) * ((float)zshift / 8.0));
  
  if(buffer == NULL || capacity == 0){
    if(length != NULL) *length = olength;
    return;
  }
  
  UniChar *characters = (UniChar *)malloc(sizeof(UniChar) * zlength);
  CFStringGetCharacters(string, CFRangeMake(0, zlength), characters);
  
  int cbits  = 0;
  int oshift = 0;
  int ocount = 0;
  
  register int i, j;
  for(i = 0, j = 0; i < zlength && j < capacity; ){
    int remaining = (zshift - ocount);
    int count = KSMin((8 - oshift), remaining);
    int mask = (int)pow(2, count) - 1;
    
    Boolean found = FALSE;
    UniChar cval = characters[i];
    uint8_t bits;
    
    register int k;
    for(k = 0; k < kCFStringKaleidoscopeBaseNCharacterOffsetCount; k++){
      CFStringKaleidoscopeBaseNCharacterOffset coffset = kCFStringKaleidoscopeBaseNCharacterOffsets[k];
      if(cval >= coffset.range.location && cval < (coffset.range.location + coffset.range.length)){
        bits = coffset.offset + (cval - coffset.range.location);
        found = TRUE;
        break;
      }
    }
    
    if(!found){
      i++;
      continue;
    }
    
    cbits |= ((bits >> (remaining - count)) & mask) << (8 - oshift - count);
    oshift = oshift + count;
    ocount = ocount + count;
    
    if(oshift >= 8){
      ((uint8_t *)buffer)[j++] = cbits;
      cbits  = 0;
      oshift = 0;
    }
    
    if(ocount >= zshift){
      ocount = 0;
      i++;
    }
    
  }
  
  if(cbits > 0){
    ((uint8_t *)buffer)[j++] = cbits;
  }
  
  if(characters)      free(characters);
  if(length != NULL)  *length = olength;
  
}

/**
 * Obtain the power of two of the specified value.
 * 
 * @param n a power of two value no less than 2
 * @return power of two
 */
int CFStringBaseNStringGetPowerOfTwo(int n) {
  int p = 1;
  while(pow(2, p) < n) p++;
  return p;
}

/********************************************************************************
                             DEPRECATED FUNCTIONS
*********************************************************************************/

/**
 * Create a string by appending a path to a base path
 * 
 * @param base base path string
 * @param path path string
 * @return canonical path string
 */
CFStringRef CFStringCreateByAppendingPath(CFStringRef base, CFStringRef path) {
  CFStringRef rpath;
  
  if(path == NULL)
    return NULL;
  
  if(CFStringGetLength(path) == 1 && CFStringCompare(path, CFSTR("."), 0) == kCFCompareEqualTo)
    return CFStringCreateCopy(NULL, (base != NULL) ? base : path);
  else if(CFStringGetLength(path) > 1 && CFStringFind(path, CFSTR("./"), 0).location == 0)
    rpath = CFStringCreateWithSubstring(NULL, path, CFRangeMake(2, CFStringGetLength(path) - 2));
  else
    rpath = CFStringCreateCopy(NULL, path);
  
  CFMutableStringRef cpath = CFStringCreateMutable(NULL, 0);
  
  if(base == NULL || CFStringGetCharacterAtIndex(rpath, 0) == kPathComponentDelimiter){
    CFStringAppend(cpath, rpath);
  }else{
    CFStringAppend(cpath, base);
    if(CFStringGetCharacterAtIndex(base, CFStringGetLength(base) - 1) != kPathComponentDelimiter){
      CFStringAppend(cpath, kPathComponentDelimiterString);
    }
    CFStringAppend(cpath, rpath);
  }
  
  CFRelease(rpath);
  return cpath;
}

/**
 * Create a new path by swapping the the base path of a canonical path.  For
 * example the arguments: "/path/to/some/file", "/path/to", "/another/path"
 * would result in the output path "/another/path/some/file".
 * 
 * @param path canonical path
 * @param base base of canonical path
 * @param newbase new base
 * @return canonical path under new base
 */
CFStringRef CFStringCreateBySwappingBasePaths(CFStringRef path, CFStringRef base, CFStringRef newbase) {
  
  if(path == NULL || base == NULL || newbase == NULL)
    return NULL;
  if(CFStringGetLength(path) <= CFStringGetLength(base))
    return NULL;
  
  size_t blen = CFStringGetLength(base);
  
  // if the base does not have a trailing path delimiter, but the character
  // in the canonical path which immediately follows the base path length is
  // a delimiter character, that delimiter is stripped.  otherwise when the
  // paths are concatenated, the relative path will be incorrectly interpreted
  // as being absolute and the base will be discarded (e.g., "/path/to/file"
  // with bases "/path", "/somewhere" will result in "/somewhere", "/to/file"
  // where "/to/file" is considered absolute.
  if(CFStringGetCharacterAtIndex(base, blen - 1) != kPathComponentDelimiter && CFStringGetLength(path) > (blen + 1) && CFStringGetCharacterAtIndex(path, blen) == kPathComponentDelimiter)
    blen++;
  
  CFStringRef tail = CFStringCreateWithSubstring(NULL, path, CFRangeMake(blen, CFStringGetLength(path) - blen));
  CFStringRef result = CFStringCreateByAppendingPath(newbase, tail);
  CFRelease(tail);
  
  return result;
}


