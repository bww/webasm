// 
// $Id: ALDirective.hh 160 2010-12-06 22:07:33Z brian $
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

#if !defined(__ALDIRECTIVE__)
#define __ALDIRECTIVE__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <JavaScriptCore/JavaScriptCore.h>

#include <Keystone/KSObject.hh>
#include <Keystone/KSOutputStream.hh>
#include <Keystone/KSStatus.hh>

#include "ALElement.hh"
#include "ALSourceFilterContext.hh"
#include "ALOptionsModel.hh"

#include <libxml/tree.h>

class ALSourceFilter;

class ALDirective : public KSObject {
private:
  const ALDirective       *_parent;
  CFMutableDictionaryRef  _properties;
  
protected:
  ALDirective(const ALDirective *parent = NULL, CFDictionaryRef properties = NULL);
  
  KSStatus pushJSContext(JSContextRef jsContext, JSObjectRef *peer) const;
  KSStatus restoreJSContext(JSContextRef jsContext, JSObjectRef peer) const;
  
  void setProperties(CFDictionaryRef properties);
  void setProperty(CFStringRef name, CFTypeRef value);
  void appendAllPropertyNames(CFMutableArrayRef names) const;
  
  virtual KSStatus processWAMLChildren(const ALOptionsModel *options, ALElement *el);
  virtual KSStatus declareScriptProperties(JSContextRef jsContext) const;
  
public:
  virtual ~ALDirective();
  
  static ALDirective * createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status = NULL);
  
  virtual CFStringRef getName(void) const;
  const ALDirective * getParent(void) const;
  CFDictionaryRef getProperties(void) const;
  CFArrayRef copyAllPropertyNames(void) const;
  CFTypeRef getProperty(CFStringRef name) const;
  
  virtual inline bool isVerbatim(void) const { return false; }
  
  virtual inline bool hasChildren(void) const { return false; }
  virtual CFArrayRef getDirectives(void) const;
  virtual ALDirective * getPreviousSibling(ALDirective *directive) const { return NULL; }
  virtual ALDirective * getNextSibling(ALDirective *directive) const { return NULL; }
  
  virtual CFTypeRef getEvaulationResult(void) const { return NULL; }
  
  virtual KSStatus emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const = 0;
  
  virtual CFStringRef copyDescription(void) const;
  
};

#endif __ALDIRECTIVE__

