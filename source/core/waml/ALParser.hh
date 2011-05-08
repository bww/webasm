// 
// $Id: ALParser.hh 39 2009-10-28 23:43:53Z brian $
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

#if !defined(__ALPARSER__)
#define __ALPARSER__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <Keystone/KSObject.hh>
#include <Keystone/KSOutputStream.hh>
#include <Keystone/KSInputStream.hh>
#include <Keystone/KSStatus.hh>
#include <Keystone/KSError.hh>

#include "ALOptionsModel.hh"

class ALParser;
class ALParserDelegate : public KSObject {
public:
  virtual ~ALParserDelegate() { };
  
  virtual KSStatus openElement(const ALParser *parser, CFStringRef name, void *info, KSError **error) { return KSStatusOk; };
  virtual KSStatus closeElement(const ALParser *parser, CFStringRef name, void *info, KSError **error) { return KSStatusOk; };
  virtual KSStatus addElementAttribute(const ALParser *parser, CFStringRef name, CFStringRef value, void *info, KSError **error) { return KSStatusOk; };
  virtual KSStatus addCharacterData(const ALParser *parser, CFStringRef content, void *info, KSError **error) { return KSStatusOk; };
  virtual KSStatus handleParseError(const ALParser *parser, KSStatus error, CFStringRef message, void *info) { return KSStatusOk; };
  
};

class ALParser : public KSObject {
private:
  const ALOptionsModel  *_options;
  
protected:
  ALParserDelegate      *_delegate;
  
  ALParser(const ALOptionsModel *model, ALParserDelegate *delegate);
  
public:
  virtual ~ALParser();
  
  const ALOptionsModel * getOptionsModel(void) const;
  
  ALParserDelegate * getDelegate(void) const;
  void setDelegate(ALParserDelegate *delegate);
  
  virtual KSStatus parse(KSInputStream *ins, KSError **error = NULL, void *info = NULL) const = 0;
  virtual KSStatus parse(CFStringRef fulltext, KSError **error = NULL, void *info = NULL) const = 0;
  
};

#endif __ALPARSER__

