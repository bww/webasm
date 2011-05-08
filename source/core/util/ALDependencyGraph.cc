// 
// $Id: ALDependencyGraph.cc 162 2011-05-08 22:01:47Z brian $
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

#include "ALDependencyGraph.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSUtility.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/CFStringAdditions.hh>

#include <libxml/tree.h>
#include <libxml/parser.h>

#define super KSObject

ALDependencyGraph::ALDependencyGraph(CFStringRef resource, CFSetRef dependencies) {
  _resource = CFRetainSafe(resource);
  _dependencies = (dependencies != NULL) ? CFSetCreateMutableCopy(NULL, 0, dependencies) : CFSetCreateMutable(NULL, 0, &kCFTypeSetCallBacks);
}

ALDependencyGraph::~ALDependencyGraph() {
  CFReleaseSafe(_resource);
  CFReleaseSafe(_dependencies);
}

/**
 * Copy the path to the standard dependency cache path for the specified
 * resource path.
 * 
 * @param path resource path
 * @return standard dependency cache path
 */
CFStringRef ALDependencyGraph::copyStandardDependencyCachePath(CFStringRef path) {
  CFStringRef name = CFStringCopyLastPathComponent(path);
  CFStringRef base = CFStringCopyPathDirectory(path);
  CFStringRef depend = CFStringCreateWithFormat(NULL, 0, CFSTR("%@/" kALDependencyGraphCachePrefix "%@"), base, name);
  CFRelease(name);
  CFRelease(base);
  return depend;
}

KSStatus ALDependencyGraph::init(KSError **error) {
  KSStatus status = KSStatusOk;
  CFStringRef path = NULL;
  
  if(_resource != NULL && (path = copyStandardDependencyCachePath(_resource)) != NULL){
    status = initWithContentsOfFile(path, error);
  }else{
    status = KSStatusError;
    if(error) *error = new KSError(status, "No resource defined");
  }
  
  CFReleaseSafe(path);
  return status;
}

KSStatus ALDependencyGraph::initWithContentsOfFile(CFStringRef path, KSError **error) {
  KSStatus status = KSStatusOk;
  CFMutableSetRef depends = NULL;
  KSFile *file = NULL;
  xmlDocPtr document = NULL;
  xmlNodePtr el = NULL;
  char *cpath = NULL;
  
  if(path == NULL){
    status = KSStatusInvalidParameter;
    if(error) *error = new KSError(status, "Path to file is null");
    goto error;
  }
  
  file = new KSFile(path);
  
  if(file->exists()){
    cpath = CFStringCopyCString(path, kDefaultStringEncoding);
    
    if((document = xmlReadFile(cpath, "UTF-8", XML_PARSE_NOCDATA)) == NULL){
      status = KSStatusInvalidResource;
      if(error) *error = new KSError(status, "Unable to read dependency graph from file: %@", path);
      goto error;
    }
    
    if((el = xmlDocGetRootElement(document)) != NULL){
      depends = CFSetCreateMutable(NULL, 0, &kCFTypeSetCallBacks);
      
      for(el = el->children; el; el = el->next){
        if(el->type != XML_ELEMENT_NODE) continue;
        
        if(strcasecmp((char *)el->name, "direct") == 0){
          
          xmlNodePtr edirect;
          for(edirect = el->children; edirect; edirect = edirect->next){
            if(edirect->type != XML_ELEMENT_NODE) continue;
            
            if(strcasecmp((char *)edirect->name, "file") == 0 && edirect->children != NULL && edirect->children->content != NULL){
              CFStringRef dfile = CFStringCreateWithCString(NULL, (char *)edirect->children->content, kDefaultStringEncoding);
              CFSetAddValue(depends, dfile);
              if(dfile) CFRelease(dfile);
            }
            
          }
          
        }
        
      }
      
    }
    
  }
  
  if(_dependencies) CFRelease(_dependencies);
  _dependencies = (depends != NULL) ? depends : CFSetCreateMutable(NULL, 0, &kCFTypeSetCallBacks);
  
error:
  if(document)  xmlFreeDoc(document);
  if(cpath)     free(cpath);
  if(file)      KSRelease(file);
  
  return status;
}

KSStatus ALDependencyGraph::write(KSError **error) const {
  CFStringRef path = NULL;
  KSStatus status = KSStatusOk;
  
  if(_resource != NULL && (path = copyStandardDependencyCachePath(_resource)) != NULL){
    status = writeToFile(path, error);
  }else{
    status = KSStatusError;
    if(error) *error = new KSError(status, "No resource defined");
  }
  
  CFReleaseSafe(path);
  return status;
}

KSStatus ALDependencyGraph::writeToFile(CFStringRef path, KSError **error) const {
  KSStatus status = KSStatusOk;
  xmlDocPtr document = NULL;
  xmlNodePtr root = NULL;
  char *output = NULL;
  FILE *ostream = NULL;
  
  if(path == NULL){
    status = KSStatusInvalidParameter;
    if(error) *error = new KSError(status, "Path to file is null");
    goto error;
  }
  
  output = CFStringCopyCString(path, kDefaultStringEncoding);
  
  document = xmlNewDoc((xmlChar *)"1.0");
  root = xmlNewNode(NULL, (xmlChar *)"dependencies");
  
  xmlDocSetRootElement(document, root);
  
  if(_dependencies != NULL){
    xmlNodePtr edirect = xmlNewNode(NULL, (xmlChar *)"direct");
    
    CFIndex count = CFSetGetCount(_dependencies);
    void **values = (void **)malloc(sizeof(void *) * count);
    
    CFSetGetValues(_dependencies, (const void **)values);
    
    register int i;
    for(i = 0; i < count; i++){
      CFStringRef file = (CFStringRef)values[i];
      char *cfile = CFStringCopyCString(file, kDefaultStringEncoding);
      xmlNodePtr node = xmlNewNode(NULL, (xmlChar *)"file");
      xmlNodePtr text = xmlNewText((xmlChar *)cfile);
      xmlAddChild(node, text);
      xmlAddChild(edirect, node);
      if(cfile) free(cfile);
    }
    
    xmlAddChild(root, edirect);
    
    if(values) free(values);
  }
  
  if((ostream = fopen(output, "w")) == NULL){
    status = KSStatusIOError;
    if(error) *error = new KSError(status, "Unable to open file for writing: %@", output);
    goto error;
  }
  
  if(xmlDocFormatDump(ostream, document, 1) < 0){
    status = KSStatusIOError;
    if(error) *error = new KSError(status, "Unable to write document to file: %@", output);
    goto error;
  }
  
error:
  if(document)  xmlFreeDoc(document);
  if(ostream)   fclose(ostream);
  if(output)    free(output);
  
  return status;
}

CFSetRef ALDependencyGraph::getDependencies(void) const {
  KSLog("Use: ALDependencyGraph::getDirectDependencies(void) instead.");
  return NULL;
}

CFSetRef ALDependencyGraph::getDirectDependencies(void) const {
  return _dependencies;
}

void ALDependencyGraph::addDirectDependency(CFStringRef depends) {
  if(_dependencies != NULL && depends != NULL){
    CFSetAddValue(_dependencies, depends);
  }
}

void ALDependencyGraph::removeDirectDependency(CFStringRef depends) {
  if(_dependencies != NULL && depends != NULL){
    CFSetRemoveValue(_dependencies, depends);
  }
}

void ALDependencyGraph::removeAllDirectDependencies(void) {
  if(_dependencies != NULL){
    CFSetRemoveAllValues(_dependencies);
  }
}

bool ALDependencyGraph::isOutOfDate(void) const {
  return false;
}

CFStringRef ALDependencyGraph::copyDescription(void) const {
  return CFStringCreateWithFormat(NULL, 0, CFSTR("[ALDependencyGraph %@]"), _dependencies);
}

