// 
// $Id: KSLog.cc 25 2008-12-23 01:45:48Z brian $
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

#include "KSLog.hh"
#include "KSString.hh"
#include "CFStringAdditions.hh"

static KSLogObserverCallBack __KSLogObserver = NULL;

/**
 * Default log observer (prints to standard error)
 * 
 * @param message log message
 */
void KSDefaultLogObserverCallBack(const char *message) {
  fputs(message, stderr);
}

/**
 * Obtain the log observer
 */
KSLogObserverCallBack KSLogGetObserverCallBack(void) {
  if(__KSLogObserver == NULL) __KSLogObserver = KSDefaultLogObserverCallBack;
  return __KSLogObserver;
}

/**
 * Set the log observer
 */
void KSLogSetObserverCallBack(KSLogObserverCallBack observer) {
  __KSLogObserver = observer;
}

/**
 * Log a formatted message
 * 
 * @param f format
 * @param ... arguments
 */
void KSLog(const char *f, ...) {
  KSLogObserverCallBack observer;
  if(!f) return;
  
  va_list ap;
  va_start(ap, f);
  
  CFStringRef pformat = CFStringCreateWithCString(NULL, f, kCFStringEncodingUTF8);
  CFStringRef format = CFStringCreateWithFormat(NULL, NULL, CFSTR("%@\n"), pformat);
  CFStringRef message = CFStringCreateWithFormatAndArguments(NULL, NULL, format, ap);
  
  char *cstr = CFStringCopyCString(message, kCFStringEncodingUTF8);
  if((observer = KSLogGetObserverCallBack()) != NULL) observer(cstr);
  free(cstr);
  
  CFRelease(pformat);
  CFRelease(format);
  CFRelease(message);
  
  va_end(ap);
  
}

void KSLogK(const char *f, ...) {
  KSLogObserverCallBack observer;
  if(!f) return;
  
  va_list ap;
  va_start(ap, f);
  
  char *format = (char *)malloc(strlen(f) + 2);
  strcpy(format, f);
  strcat(format, "\n");
  
  KSString *message = new KSString(0, format, ap);
  
  char *cstr = CFStringCopyCString((CFStringRef)message->getCFTypeRef(), kCFStringEncodingUTF8);
  if((observer = KSLogGetObserverCallBack()) != NULL) observer(cstr);
  free(cstr);
  
  if(format) free(format);
  KSRelease(message);
  
  va_end(ap);
  
}


