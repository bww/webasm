// 
// $Id: CFStringAdditions.hh 83 2010-08-13 17:18:02Z brian $
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

#if !defined(__KEYSTONE_CFSTRINGADDITIONS__)
#define __KEYSTONE_CFSTRINGADDITIONS__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSRuntime.hh"

#define kDefaultStringEncoding        kCFStringEncodingUTF8
#define kPathComponentDelimiter       '/'
#define kPathComponentDelimiterString CFSTR("/")
#define kWhitespaceCharacters         CFSTR(" \t\v\n\r")

typedef enum CFStringCondensationOptionsMask_ {
  CFStringCondensationOptionNone          = 0,
  CFStringCondensationOptionIgnoreBreaks  = 1 << 0
} CFStringCondensationOptionsMask;

typedef SInt32 CFStringCondensationOptions;
typedef SInt32 CFStringPathOptions;

char *      CFStringCopyCString(CFStringRef string, CFStringEncoding encoding);
CFDataRef   CFStringCopyBytes(CFStringRef string, CFStringEncoding encoding);
CFArrayRef  CFStringCreateArrayFromDelimitedList(CFStringRef string, CFStringRef delimiter);
CFArrayRef  CFStringCreateArrayFromComponentList(CFStringRef string, CFCharacterSetRef delimiter, CFCharacterSetRef escape);

CFStringRef CFStringCreateDelimitedListFromArray(CFArrayRef items, CFStringRef delimiter);
CFStringRef CFStringCreateEscapedString(CFStringRef string, CFCharacterSetRef escape, UniChar prefix);
CFStringRef CFStringCreateTrimmedString(CFStringRef string);
CFStringRef CFStringCopyTemporaryDirectory(void);
CFStringRef CFStringCreateTemporaryFilePath(CFStringRef name);
CFStringRef CFStringCopyUserHomeDirectory(void);
CFStringRef CFStringCreateUserHomeFilePath(CFStringRef relative);
CFStringRef CFStringCreateCanonicalPath(CFStringRef base, CFStringRef path);
CFStringRef CFStringCreateRelativePath(CFStringRef base, CFStringRef path);
CFStringRef CFStringCreateRelativePathWithOptions(CFStringRef base, CFStringRef path, CFStringPathOptions options, CFIndex *diverge, CFIndex *ascents, CFIndex *trailing);
CFStringRef CFStringCreateByRelocatingPath(CFStringRef path, CFStringRef newbase, Boolean relative);
CFStringRef CFStringCreateByMovingLastPathComponent(CFStringRef path, CFStringRef newbase, Boolean relative);
CFArrayRef  CFStringCreateArrayWithPathComponents(CFStringRef path);
CFStringRef CFStringCopyPathDirectory(CFStringRef path);
CFStringRef CFStringCopyFileNameExtension(CFStringRef path);
CFStringRef CFStringCopyLastPathComponent(CFStringRef path);

CFStringRef CFStringCreateCondensedCopy(CFStringRef string, CFStringCondensationOptions options, CFIndex maxlen);
CFStringRef CFStringCopyMangledName(CFStringRef string, UniChar replacement);
CFStringRef CFStringCopyMangledNameForCharacterSet(CFStringRef string, CFCharacterSetRef allowed, UniChar replacement);

void        CFStringAppendSubstring(CFMutableStringRef destination, CFStringRef string, CFRange range);

CFStringRef CFStringCreateHexStringWithData(CFAllocatorRef a, CFDataRef d);
CFStringRef CFStringCreateHexStringWithBytes(CFAllocatorRef a, const void *b, size_t n, char s);
CFStringRef CFStringCreateBaseNStringWithData(CFAllocatorRef a, int base, CFDataRef d);
CFStringRef CFStringCreateBaseNStringWithBytes(CFAllocatorRef a, int base, const void *b, size_t n, char s);
CFDataRef   CFStringCreateDataWithBaseNString(CFAllocatorRef a, int base, CFStringRef string);
void        CFStringCopyBytesFromBaseNString(CFAllocatorRef a, int base, const void *buffer, size_t capacity, size_t *length, CFStringRef string);

/** Deprecated Functions **/

CFStringRef CFStringCreateByAppendingPath(CFStringRef base, CFStringRef path) KEYSTONE_DEPRECATED_WITH_COMMENT("Use CFStringCreateCanonicalPath() instead");
CFStringRef CFStringCreateBySwappingBasePaths(CFStringRef path, CFStringRef base, CFStringRef newbase) KEYSTONE_DEPRECATED_WITH_COMMENT("Use CFStringCreateByRelocatingPath() instead");

#endif __KEYSTONE_CFSTRINGADDITIONS__



