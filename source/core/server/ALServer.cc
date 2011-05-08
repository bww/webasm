// 
// $Id: ALServer.cc 159 2010-12-06 14:49:05Z brian $
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

#include "ALServer.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSUtility.hh>

#define super KSObject

ALServer::ALServer(CFStringRef name, CFURLRef base, bool defaultServer) {
  _name = CFRetainSafe(name);
  _base = CFRetainSafe(base);
  _defaultServer = defaultServer;
}

ALServer::~ALServer() {
  if(_name) CFRelease(_name);
  if(_base) CFRelease(_base);
}

CFStringRef ALServer::getName(void) const {
  return _name;
}

CFURLRef ALServer::getOutputBaseURL(void) const {
  return _base;
}

bool ALServer::isDefaultServer(void) const {
  return _defaultServer;
}

CFStringRef ALServer::copyDescription(void) const {
  CFStringRef absoluteBase = (_base != NULL) ? CFURLGetString(_base) : CFSTR("<no base>");
  return CFStringCreateWithFormat(NULL, 0, CFSTR("[ALServer \"%@\" %@]"), (_name != NULL) ? _name : CFSTR("<unnamed>"), absoluteBase);
}

