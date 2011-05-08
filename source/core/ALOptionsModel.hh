// 
// $Id: ALOptionsModel.hh 161 2010-12-06 23:21:36Z brian $
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

#if !defined(__ALOPTIONSMODEL__)
#define __ALOPTIONSMODEL__ 1

#include <CoreFoundation/CoreFoundation.h>
#include <Keystone/KSObject.hh>
#include <Keystone/KSStatus.hh>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "ALServer.hh"

#define kAssemblerCoreBundleID    CFSTR("net.woltergroup.AssemblerCore")
#define kKeystoneBundleID         CFSTR("net.woltergroup.Keystone")

#define gSharedOptions            (ALOptionsModel::getShared())

const int kOptionQuiet              =  1 << 0;
const int kOptionVerbose            =  1 << 1;
const int kOptionDebug              = (1 << 2) | kOptionVerbose;
const int kOptionForce              =  1 << 3;
const int kOptionIncludeHiddenFiles =  1 << 4;
const int kOptionCollapseWhitespace =  1 << 5;

#define ALOptionsFlagSet(o, f)    ((o->getFlags() & f) == f)

class ALOptionsModel : public KSObject {
private:
  CFStringRef             _configPath;
  CFStringRef             _projectBasePath;
  CFStringRef             _outputBasePath;
  CFStringRef             _runtimeBasePath;
  CFArrayRef              _processFileTypes;
  CFStringRef             _processFilePattern;
  CFArrayRef              _servers;
  CFMutableDictionaryRef  _properties;
  CFMutableDictionaryRef  _tags;
  int                     _flags;
  
protected:
  KSStatus loadConfiguration(xmlNodePtr el, CFStringRef bpath);
  
public:
  ALOptionsModel();
  virtual ~ALOptionsModel();
  
  static const ALOptionsModel * getShared(void);
  
  KSStatus loadConfigurationFromXML(CFStringRef path);
  
  CFStringRef getConfigPath() const;
  void setConfigPath(CFStringRef path);
  CFStringRef getProjectBasePath() const;
  void setProjectBasePath(CFStringRef path);
  CFStringRef getOutputBasePath() const;
  void setOutputBasePath(CFStringRef path);
  CFStringRef getRuntimeBasePath(void) const;
  void setRuntimeBasePath(CFStringRef path);
  CFStringRef copyCanonicalPath(CFStringRef path) const;
  CFArrayRef getProcessFileTypes() const;
  void setProcessFileTypes(CFArrayRef types);
  CFStringRef getProcessFilePattern() const;
  void setProcessFilePattern(CFStringRef pattern);
  
  CFArrayRef getServers() const;
  void setServers(CFArrayRef servers);
  ALServer * getServerByName(CFStringRef name) const;
  ALServer * getDefaultServer(void) const;
  
  CFDictionaryRef getProperties() const;
  CFStringRef getProperty(CFStringRef name) const;
  void setProperties(CFDictionaryRef properties);
  void addProperties(CFDictionaryRef properties);
  void setProperty(CFStringRef name, CFStringRef value);
  void loadPropertiesFromEnvironment();
  
  CFDictionaryRef getCustomTags() const;
  CFStringRef getCustomTagCommand(CFStringRef name) const;
  void setCustomTagCommand(CFStringRef name, CFStringRef command);
  
  int getFlags() const;
  void setFlags(int f);
  void setFlag(int f, bool on);
  
  /* Convenience Flag Methods */
  bool isVerbose() const;
  bool isQuiet() const;
  bool isDebugging() const;
  bool getForce() const;
  bool getIncludeHiddenFiles() const;
  bool getCollapseWhitespace() const;
  
  CFArrayRef getExcludedFilePrefixes(void) const;
  
};

#endif __ALOPTIONSMODEL__

