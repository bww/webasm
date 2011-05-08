// 
// $Id: ALConditionalDirective.hh 160 2010-12-06 22:07:33Z brian $
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

#if !defined(__ALCONDITIONALDIRECTIVE__)
#define __ALCONDITIONALDIRECTIVE__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <JavaScriptCore/JavaScriptCore.h>

#include <Keystone/KSObject.hh>
#include <Keystone/KSOutputStream.hh>
#include <Keystone/KSStatus.hh>

#include <libxml/tree.h>

#include "ALOptionsModel.hh"
#include "ALDirectiveList.hh"

typedef enum ALConditionalOperation_ {
  ALConditionalIf,
  ALConditionalElseIf,
  ALConditionalElse
} ALConditionalOperation;

class ALConditionalDirective : public ALDirectiveList {
private:
  ALConditionalOperation  _operation;
  CFStringRef             _source;
  JSStringRef             _expression;
  mutable CFNumberRef     _result;
  
public:
  ALConditionalDirective(const ALDirective *parent, CFDictionaryRef properties, ALConditionalOperation op, CFStringRef source);
  virtual ~ALConditionalDirective();
  
  static ALConditionalDirective * createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status = NULL);
  
  virtual CFStringRef getName(void) const;
  CFStringRef getExpressionSource(void) const;
  
  virtual CFTypeRef getEvaulationResult(void) const;
  
  virtual KSStatus emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const;
  
};

#endif __ALCONDITIONALDIRECTIVE__

