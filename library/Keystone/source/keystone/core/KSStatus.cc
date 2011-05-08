// 
// $Id: KSStatus.cc 4 2008-11-24 08:03:36Z brian $
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

#include "KSStatus.hh"
#include "KSStatusPrivate.hh"
#include "KSLog.hh"

CFMutableDictionaryRef __KSStatusPopulateMessageMap(CFMutableDictionaryRef map);

/**
 * Obtain the error message maps.  The maps are checked sequentially for a message
 * which describes a given error code.  The first map is always the Keystone map.
 * 
 * @return error message maps
 */
CFMutableArrayRef KSStatusGetMessageMaps(void) {
  static CFMutableArrayRef __KSStatusErrorMessageMaps = NULL;
  
  if(!__KSStatusErrorMessageMaps){
    CFMutableDictionaryRef vsmap;
    
    __KSStatusErrorMessageMaps = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    
    vsmap = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    __KSStatusPopulateMessageMap(vsmap);
    
    CFArrayAppendValue(__KSStatusErrorMessageMaps, vsmap);
    
  }
  
  return __KSStatusErrorMessageMaps;
}

/**
 * Add a message map to the status system.
 * 
 * @param m message map
 */
void KSStatusAddMessageMap(CFDictionaryRef m) {
  CFMutableArrayRef emaps = NULL;
  
  if(!(emaps = KSStatusGetMessageMaps())){
    KSLogError("Cannot add message map; Status not initialized");
    return;
  }
  
  CFArrayAppendValue(emaps, m);
  
}

/**
 * Obtain a status message for the specified error code
 * 
 * @param err error code
 * @return error message
 */
CFStringRef KSStatusGetMessage(KSStatus status) {
  CFArrayRef emaps = NULL;
  
  if(!(emaps = KSStatusGetMessageMaps()))
    return CFSTR("Unknown; Status not initilaized");
  
  return KSStatusGetMessageFromMaps(status, emaps);
}

/**
 * Obtain a status message from the specified message maps
 * 
 * @param err error code
 * @param maps message maps
 * @return error message
 */
CFStringRef KSStatusGetMessageFromMaps(KSStatus status, CFArrayRef emaps) {
  CFStringRef message = NULL;
  
  CFNumberRef keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &status);
  
  register int i;
  for(i = 0; i < CFArrayGetCount(emaps); i++){
    CFDictionaryRef emap = (CFDictionaryRef)CFArrayGetValueAtIndex(emaps, i);
    if(CFDictionaryGetValueIfPresent(emap, keyVal, (const void **)&message))
      return message;
  }
  
  return CFSTR("Unknown status code");
}

