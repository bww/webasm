// 
// $Id: ALOptionsModel.cc 162 2011-05-08 22:01:47Z brian $
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

#include "ALOptionsModel.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSUtility.hh>
#include <Keystone/CFStringAdditions.hh>

#include "ALDependencyGraph.hh"

#include <crt_externs.h>

/**
 * Internal constructor
 */
ALOptionsModel::ALOptionsModel() {
  
  _flags = 0;
  _configPath = NULL;
  _projectBasePath = CFSTR(".");
  _outputBasePath = CFSTR("./webasm");
  _runtimeBasePath = NULL;
  _processFileTypes = CFStringCreateArrayFromDelimitedList(CFSTR("html, htm"), CFSTR(","));
  _processFilePattern = NULL;
  _servers = NULL;
  _properties = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  _tags = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  
}

/**
 * Clean up
 */
ALOptionsModel::~ALOptionsModel() {
  /* Singleton, no need ... */
}

/**
 * Obtain the shared instance
 * 
 * @return shared instance
 */
const ALOptionsModel * ALOptionsModel::getShared(void) {
  static ALOptionsModel *shared;
  if(!shared) shared = new ALOptionsModel();
  return (const ALOptionsModel *)shared;
}

/**
 * Load configuration XML from the specified file.
 * 
 * @param path configuration path
 */
KSStatus ALOptionsModel::loadConfigurationFromXML(CFStringRef path) {
  KSStatus status = KSStatusOk;
  CFStringRef bpath = NULL;
  char *cpath;
  xmlDocPtr document;
  
  if(isDebugging()){
    KSLog("Loading Configuration: %@", path);
  }
  
  cpath = CFStringCopyCString(path, kDefaultStringEncoding);
  
  if((document = xmlReadFile(cpath, NULL, XML_PARSE_NOCDATA)) == NULL){
    KSLog("Unable to read configuration: %@", path);
    status = KSStatusError;
    goto error;
  }
  
  bpath = CFStringCopyPathDirectory(path);
  status = loadConfiguration(xmlDocGetRootElement(document), bpath);
  if(bpath) CFRelease(bpath);
  
error:
  if(cpath) free(cpath);
  if(document) xmlFreeDoc(document);
  
  return status;
}

/**
 * Load configuration
 * 
 * @param el root element
 * @param bpath base path
 * @return status
 */
KSStatus ALOptionsModel::loadConfiguration(xmlNodePtr el, CFStringRef bpath) {
  KSStatus status = KSStatusOk;
  
  for(el = el->children; el; el = el->next){
    if(el->type != XML_ELEMENT_NODE) continue;
    
    if(strcasecmp((char *)el->name, "project-base") == 0 && el->children != NULL && el->children->content != NULL){
      
      CFStringRef pbase = CFStringCreateWithCString(NULL, (char *)el->children->content, kDefaultStringEncoding);
      CFStringRef abase = CFStringCreateCanonicalPath(bpath, pbase);
      setProjectBasePath(abase);
      CFRelease(pbase);
      CFRelease(abase);
      
    }else if(strcasecmp((char *)el->name, "output-base") == 0 && el->children != NULL && el->children->content != NULL){
      
      CFStringRef obase = CFStringCreateWithCString(NULL, (char *)el->children->content, kDefaultStringEncoding);
      CFStringRef abase = CFStringCreateCanonicalPath(bpath, obase);
      setOutputBasePath(abase);
      CFRelease(obase);
      CFRelease(abase);
      
    }else if(strcasecmp((char *)el->name, "process-file-types") == 0 && el->children != NULL && el->children->content != NULL){
      
      CFStringRef ptypes = CFStringCreateWithCString(NULL, (char *)el->children->content, kDefaultStringEncoding);
      CFArrayRef types = CFStringCreateArrayFromDelimitedList(ptypes, CFSTR(","));
      setProcessFileTypes(types);
      CFRelease(types);
      CFRelease(ptypes);
      
    }else if(strcasecmp((char *)el->name, "process-file-pattern") == 0 && el->children != NULL && el->children->content != NULL){
      
      CFStringRef pattern = CFStringCreateWithCString(NULL, (char *)el->children->content, kDefaultStringEncoding);
      setProcessFilePattern(pattern);
      CFRelease(pattern);
      
    }else if(strcasecmp((char *)el->name, "servers") == 0){
      CFMutableArrayRef servers = CFArrayCreateMutable(NULL, 0, &kKSObjectCFArrayCallBacks);
      xmlNodePtr eServer;
      
      for(eServer = el->children; eServer; eServer = eServer->next){
        if(eServer->type != XML_ELEMENT_NODE) continue;
        
        if(strcasecmp((char *)eServer->name, "server") == 0){
          CFStringRef name = NULL;
          CFURLRef base = NULL;
          bool defaultServer = false;
          
          char *cname;
          if((cname = (char *)xmlGetProp(eServer, (xmlChar *)"name")) != NULL && strlen(cname) > 0){
            name = CFStringCreateWithCString(NULL, cname, kDefaultStringEncoding);
          }
          
          char *cdefault;
          if((cdefault = (char *)xmlGetProp(eServer, (xmlChar *)"default")) != NULL && (strcasecmp(cdefault, "t") == 0 || strcasecmp(cdefault, "true") == 0 || strcasecmp(cdefault, "y") == 0 || strcasecmp(cdefault, "yes") == 0)){
            defaultServer = true;
          }
          
          xmlNodePtr eServerProp;
          for(eServerProp = eServer->children; eServerProp; eServerProp = eServerProp->next){
            if(eServerProp->type != XML_ELEMENT_NODE) continue;
            
            if(strcasecmp((char *)eServerProp->name, "output-base") == 0){
              if(eServerProp->children != NULL && eServerProp->children->content != NULL){
                CFReleaseSafe(base);
                CFStringRef url = CFStringCreateWithCString(NULL, (char *)eServerProp->children->content, kDefaultStringEncoding);
                base = CFURLCreateWithString(NULL, url, NULL);
                CFReleaseSafe(url);
              }
            }
            
          }
          
          if(base != NULL){
            ALServer *server = new ALServer(name, base, defaultServer);
            CFArrayAppendValue(servers, server);
            KSRelease(server);
          }
          
          CFReleaseSafe(name);
          CFReleaseSafe(base);
        }
        
      }
      
      setServers(servers);
      CFReleaseSafe(servers);
      
    }else if(strcasecmp((char *)el->name, "properties") == 0){
      xmlNodePtr sel;
      
      for(sel = el->children; sel; sel = sel->next){
        if(sel->type != XML_ELEMENT_NODE) continue;
        
        if(strcasecmp((char *)sel->name, "define") == 0){
          
          char *cname;
          if((cname = (char *)xmlGetProp(sel, (xmlChar *)"name")) == NULL || strlen(cname) < 1){
            KSLogError("Property definition requires a name attribute");
            continue;
          }
          
          CFStringRef name = CFStringCreateWithCString(NULL, cname, kDefaultStringEncoding);
          CFStringRef value =
            (sel->children != NULL && sel->children->content != NULL)
            ? CFStringCreateWithCString(NULL, (char *)sel->children->content, kDefaultStringEncoding)
            : NULL;
          
          setProperty(name, value);
          
          if(name) CFRelease(name);
          if(value) CFRelease(value);
          
        }
        
      }
      
    }else if(strcasecmp((char *)el->name, "custom-tags") == 0){
      xmlNodePtr sel;
      
      for(sel = el->children; sel; sel = sel->next){
        if(sel->type != XML_ELEMENT_NODE) continue;
        
        if(strcasecmp((char *)sel->name, "tag") == 0){
          CFStringRef name  = NULL;
          CFStringRef value = NULL;
          CFStringRef absolute = NULL;
          
          char *cname;
          if((cname = (char *)xmlGetProp(sel, (xmlChar *)"name")) == NULL || strlen(cname) < 1){
            KSLogError("Custom tag definition requires a name attribute");
            goto tagerror;
          }
          
          name = CFStringCreateWithCString(NULL, cname, kDefaultStringEncoding);
          value =
            (sel->children != NULL && sel->children->content != NULL)
            ? CFStringCreateWithCString(NULL, (char *)sel->children->content, kDefaultStringEncoding)
            : NULL;
          
          if(value == NULL){
            KSLogError("Custom tag definition requires a command");
            goto tagerror;
          }
          
          absolute = CFStringCreateCanonicalPath(bpath, value);
          setCustomTagCommand(name, absolute);
          
          tagerror:
          if(name)      CFRelease(name);
          if(value)     CFRelease(value);
          if(absolute)  CFRelease(absolute);
        }
        
      }
      
    }
    
  }
  
  return status;
}

/**
 * Obtain the configuration path
 * 
 * @return config path
 */
CFStringRef ALOptionsModel::getConfigPath() const {
  return _configPath;
}

/**
 * Set the configuration path
 * 
 * @param path config path
 */
void ALOptionsModel::setConfigPath(CFStringRef path) {
  if(_configPath) CFRelease(_configPath);
  _configPath = (path != NULL) ? ((_runtimeBasePath != NULL) ? CFStringCreateCanonicalPath(_runtimeBasePath, path) : (CFStringRef)CFRetain(path)) : NULL;
}

/**
 * Obtain the project base path
 * 
 * @return project base path
 */
CFStringRef ALOptionsModel::getProjectBasePath() const {
  return _projectBasePath;
}

/**
 * Set the project base path
 * 
 * @param path project base path
 */
void ALOptionsModel::setProjectBasePath(CFStringRef path) {
  if(_projectBasePath) CFRelease(_projectBasePath);
  _projectBasePath = (path != NULL) ? ((_runtimeBasePath != NULL) ? CFStringCreateCanonicalPath(_runtimeBasePath, path) : (CFStringRef)CFRetain(path)) : NULL;
}

/**
 * Obtain the output base path
 * 
 * @return output base path
 */
CFStringRef ALOptionsModel::getOutputBasePath() const {
  return _outputBasePath;
}

/**
 * Set the output base path
 * 
 * @param path output base path
 */
void ALOptionsModel::setOutputBasePath(CFStringRef path) {
  if(_outputBasePath) CFRelease(_outputBasePath);
  _outputBasePath = (path != NULL) ? ((_runtimeBasePath != NULL) ? CFStringCreateCanonicalPath(_runtimeBasePath, path) : (CFStringRef)CFRetain(path)) : NULL;
}

/**
 * Obtain the runtime base path
 * 
 * @return runtime base path
 */
CFStringRef ALOptionsModel::getRuntimeBasePath() const {
  return _runtimeBasePath;
}

/**
 * Set the runtime base path.  The runtime base must be a
 * canonical path.
 * 
 * @param path runtime base path
 */
void ALOptionsModel::setRuntimeBasePath(CFStringRef path) {
  if(_runtimeBasePath) CFRelease(_runtimeBasePath);
  _runtimeBasePath = (path != NULL) ? (CFStringRef)CFRetain(path) : NULL;
}

/**
 * Copy a canonical path relative to the runtime base
 * 
 * @param path a relative path
 * @return canonical path
 */
CFStringRef ALOptionsModel::copyCanonicalPath(CFStringRef path) const {
  CFStringRef base = getRuntimeBasePath();
  return (base != NULL) ? CFStringCreateCanonicalPath(base, path) : CFStringCreateCopy(NULL, path);
}

/**
 * Obtain file types (extensions) to process
 * 
 * @return process file types
 */
CFArrayRef ALOptionsModel::getProcessFileTypes() const {
  return _processFileTypes;
}

/**
 * Set file types (extensions) to process
 * 
 * @param types process file types
 */
void ALOptionsModel::setProcessFileTypes(CFArrayRef types) {
  if(_processFileTypes) CFRelease(_processFileTypes);
  _processFileTypes = (types != NULL) ? (CFArrayRef)CFRetain(types) : NULL;
}

/**
 * Obtain file pattern to process
 * 
 * @return file pattern
 */
CFStringRef ALOptionsModel::getProcessFilePattern() const {
  return _processFilePattern;
}

/**
 * Set file pattern to process
 * 
 * @param pattern file pattern
 */
void ALOptionsModel::setProcessFilePattern(CFStringRef pattern) {
  if(_processFilePattern) CFRelease(_processFilePattern);
  _processFilePattern = CFRetainSafe(pattern);
}

/**
 * Obtain server definitions
 * 
 * @return server definitions
 */
CFArrayRef ALOptionsModel::getServers() const {
  return _servers;
}

/**
 * Set server definitions
 * 
 * @param servers server definitions
 */
void ALOptionsModel::setServers(CFArrayRef servers) {
  if(_servers) CFRelease(_servers);
  _servers = CFRetainSafe(servers);
}

/**
 * Obtain a server definition by name
 * 
 * @param name server definition name
 * @return server definition
 */
ALServer * ALOptionsModel::getServerByName(CFStringRef name) const {
  
  if(_servers != NULL){
    register int i;
    for(i = 0; i < CFArrayGetCount(_servers); i++){
      ALServer *server = (ALServer *)CFArrayGetValueAtIndex(_servers, i);
      CFStringRef match;
      if((match = server->getName()) != NULL && CFStringCompare(match, name, kCFCompareCaseInsensitive) == kCFCompareEqualTo){
        return server;
      }
    }
  }
  
  return NULL;
}

/**
 * Obtain the default server definition
 * 
 * @return default server definition
 */
ALServer * ALOptionsModel::getDefaultServer(void) const {
  
  if(_servers != NULL){
    
    // if exactly one server is defined, it is always the default server
    if(CFArrayGetCount(_servers) == 1){
      return (ALServer *)CFArrayGetValueAtIndex(_servers, 0);
    }
    
    register int i;
    for(i = 0; i < CFArrayGetCount(_servers); i++){
      ALServer *server = (ALServer *)CFArrayGetValueAtIndex(_servers, i);
      if(server->isDefaultServer()) return server;
    }
    
  }
  
  return NULL;
}

/**
 * Obtain defined properties
 * 
 * @return properties
 */
CFDictionaryRef ALOptionsModel::getProperties() const {
  return _properties;
}

/**
 * Obtain a defined property value for the specified name
 * 
 * @param name property name
 * @return value
 */
CFStringRef ALOptionsModel::getProperty(CFStringRef name) const {
  return (_properties != NULL) ? (CFStringRef)CFDictionaryGetValue(_properties, name) : NULL;
}

/**
 * Set defined properties
 * 
 * @param properties properties
 */
void ALOptionsModel::setProperties(CFDictionaryRef properties) {
  if(_properties) CFRelease(_properties);
  _properties =
    (properties != NULL)
    ? CFDictionaryCreateMutableCopy(NULL, 0, properties)
    : CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
}

/**
 * Add a set of properties to the current set, overriding any current properties
 * with matching names.
 * 
 * @param properties properties to add
 */
void ALOptionsModel::addProperties(CFDictionaryRef properties) {
  if(_properties != NULL){
    CFIndex pcount = CFDictionaryGetCount(properties);
    const void **keys = (const void **)malloc(sizeof(CFTypeRef) * pcount);
    const void **vals = (const void **)malloc(sizeof(CFTypeRef) * pcount);
    CFDictionaryGetKeysAndValues(properties, keys, vals);
    
    register int i;
    for(i = 0; i < pcount; i++){
      setProperty((CFStringRef)keys[i], (CFStringRef)vals[i]);
    }
    
    if(keys) free(keys);
    if(vals) free(vals);
  }
}

/**
 * Set a property
 * 
 * @param name property name
 * @param value property value
 */
void ALOptionsModel::setProperty(CFStringRef name, CFStringRef value) {
  if(name != NULL && value != NULL){
    CFDictionarySetValue(_properties, name, value);
  }
}

/**
 * Load properties from the environment.  Environment properties are formatted
 * as <code>WEBASM_PROPERTY_<em>name</em>=Value</code> where <em>name</em> is
 * the property name that will be defined.
 */
void ALOptionsModel::loadPropertiesFromEnvironment() {
  const char *prefix    = "WEBASM_PROPERTY_";
  const char separator  = '=';
  
  char **env;
  for(env = *(_NSGetEnviron()); *env != NULL; env++){
    const char *item = *env;
    if(strncasecmp(item, prefix, strlen(prefix)) == 0){
      int length = strlen(item);
      int offset = strlen(prefix);
      
      register int i;
      for(i = 0; i < length; i++){
        if(item[i] == separator) break;
      }
      
      char *name = (char *)malloc(sizeof(char) * (i - offset + 1));
      strncpy(name, (item + offset), i - offset);
      name[i - offset] = 0;
      
      char *value = (char *)malloc(sizeof(char) * (length - i));
      strncpy(value, (item + i + 1), length - i - 1);
      value[length - i - 1] = 0;
      
      CFStringRef cname  = CFStringCreateWithCString(NULL, name, kDefaultStringEncoding);
      CFStringRef cvalue = CFStringCreateWithCString(NULL, value, kDefaultStringEncoding);
      setProperty(cname, cvalue);
      if(cname) CFRelease(cname);
      if(cvalue) CFRelease(cvalue);
      
      if(name)  free(name);
      if(value) free(value);
    }
  }
  
}

/**
 * Obtain all custom tags
 * 
 * @return custom tags
 */
CFDictionaryRef ALOptionsModel::getCustomTags() const {
  return _tags;
}

/**
 * Obtain the command for a specified custom tag
 * 
 * @param name custom tag name
 * @return tag command
 */
CFStringRef ALOptionsModel::getCustomTagCommand(CFStringRef name) const {
  return (_tags != NULL) ? (CFStringRef)CFDictionaryGetValue(_tags, name) : NULL;
}

/**
 * Set a custom tag
 * 
 * @param name custom tag name
 * @param command custom tag command
 */
void ALOptionsModel::setCustomTagCommand(CFStringRef name, CFStringRef command) {
  if(_tags != NULL){
    CFDictionarySetValue(_tags, name, command);
  }
}

/**
 * Obtain flags
 * 
 * @return flags
 */
int ALOptionsModel::getFlags() const {
  return _flags;
}

/**
 * Set flags
 * 
 * @param f flags
 */
void ALOptionsModel::setFlags(int f) {
  _flags = f;
}

/**
 * Set a flag
 * 
 * @param f flag
 * @param on on or not
 */
void ALOptionsModel::setFlag(int f, bool on) {
  _flags = (on) ? (_flags | f) : (_flags & ~f);
}

/**
 * Determine if the verbose flag is set
 * 
 * @return verbose or not
 */
bool ALOptionsModel::isVerbose() const {
  return (getFlags() & kOptionVerbose) == kOptionVerbose;
}

/**
 * Determine if the quiet flag is set
 * 
 * @return quiet or not
 */
bool ALOptionsModel::isQuiet() const {
  return (getFlags() & kOptionQuiet) == kOptionQuiet;
}

/**
 * Determine if the debugging flag is set
 * 
 * @return debugging or not
 */
bool ALOptionsModel::isDebugging() const {
  return (getFlags() & kOptionDebug) == kOptionDebug;
}

/**
 * Determine if the force output flag is set
 * 
 * @return force output or not
 */
bool ALOptionsModel::getForce() const {
  return (getFlags() & kOptionForce) == kOptionForce;
}

/**
 * Determine if hidden files should be processed or not
 * 
 * @return process hidden files or not
 */
bool ALOptionsModel::getIncludeHiddenFiles() const {
  return (getFlags() & kOptionIncludeHiddenFiles) == kOptionIncludeHiddenFiles;
}

/**
 * Determine if verbatim whitespace should be collapsed or not
 * 
 * @return collapse verbatim whitespace or not
 */
bool ALOptionsModel::getCollapseWhitespace() const {
  return (getFlags() & kOptionCollapseWhitespace) == kOptionCollapseWhitespace;
}

/**
 * Obtain a set of file name prefixes which should be excluded from processing
 * 
 * @return excluded file name prefixes
 */
CFArrayRef ALOptionsModel::getExcludedFilePrefixes(void) const {
  static CFArrayRef __shared = NULL;
  if(__shared == NULL) {
    const void *values[] = {
      CFSTR(kALDependencyGraphCacheDirectory)
    };
    __shared = CFArrayCreate(NULL, values, 1, &kCFTypeArrayCallBacks);
  }
  return __shared;
}


