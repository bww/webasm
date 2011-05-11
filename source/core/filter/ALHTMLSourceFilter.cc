// 
// $Id: ALHTMLSourceFilter.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALHTMLSourceFilter.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/KSError.hh>
#include <Keystone/CFStringAdditions.hh>

#include "ALParser.hh"
#include "ALWAMLParser.hh"
#include "ALTreeParser.hh"
#include "ALDirectiveList.hh"
#include "ALVerbatimDirective.hh"
#include "ALOptionsModel.hh"
#include "ALJSDocument.hh"

#define super ALSourceFilter

/**
 * Construct
 * 
 * @param model options
 */
ALHTMLSourceFilter::ALHTMLSourceFilter(const ALOptionsModel *model, const ALSourceFilter *parentFilter, const ALDirective *parentDirective, const ALSourceFilterContext *context, JSContextRef jsContext) : super(model, parentFilter, parentDirective, context) {
  _jsContext = (jsContext != NULL) ? JSGlobalContextRetain((JSGlobalContextRef)jsContext) : JSGlobalContextCreate(NULL);
  _jsPeer    = NULL;
  initJSContext(_jsContext);
}

/**
 * Clean up
 */
ALHTMLSourceFilter::~ALHTMLSourceFilter() {
  if(_jsContext)            JSGlobalContextRelease((JSGlobalContextRef)_jsContext);
  if(_currentOutputStream)  KSRelease(_currentOutputStream);
}

/**
 * Initialize JS context
 * 
 * @param jsContext the context
 */
KSStatus ALHTMLSourceFilter::initJSContext(JSContextRef jsContext) {
  KSStatus status = KSStatusOk;
  JSObjectRef global;
  JSClassRef clazz;
  
  if((global = JSContextGetGlobalObject(jsContext)) == NULL){
    KSLogError("No global object; cannot initialize JavaScript");
    status = KSStatusError;
    goto error;
  }
  
  if((clazz = ALJSDocumentGetClass(jsContext)) != NULL){
    _jsPeer = JSObjectMake(jsContext, clazz, (void *)this);
  }else{
    KSLogError("Unable to create JavaScript peer class");
  }
  
error:
  return status;
}

/**
 * Push our context
 * 
 * @param peer on output: the top peer, if any
 */
KSStatus ALHTMLSourceFilter::pushJSContext(JSObjectRef *peer) const {
  KSStatus status = KSStatusOk;
  JSStringRef document = JSStringCreateWithCFString(CFSTR("document"));
  JSStringRef parent = JSStringCreateWithCFString(CFSTR("parent"));
  JSObjectRef global;
  
  JSObjectRef parentPeer;
  ALHTMLSourceFilter *parentFilter;
  
  if((global = JSContextGetGlobalObject(_jsContext)) == NULL){
    KSLogError("No global object; cannot push JavaScript context");
    status = KSStatusError;
    goto error;
  }
  
  if((parentFilter = (ALHTMLSourceFilter *)getParentFilter()) != NULL && (parentPeer = parentFilter->getJavaScriptPeer()) != NULL){
    JSObjectSetProperty(_jsContext, _jsPeer, parent, parentPeer, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly, NULL);
  }
  
  if(peer) *peer = (JSObjectRef)JSObjectGetProperty(_jsContext, global, document, NULL);
  JSObjectSetProperty(_jsContext, global, document, _jsPeer, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly, NULL);
  
error:
  JSStringRelease(parent);
  JSStringRelease(document);
  
  return status;
}

/**
 * Pop our context
 * 
 * @param peer the peer to restore
 */
KSStatus ALHTMLSourceFilter::restoreJSContext(JSObjectRef peer) const {
  KSStatus status = KSStatusOk;
  JSStringRef document = JSStringCreateWithCFString(CFSTR("document"));
  JSObjectRef global;
  
  if((global = JSContextGetGlobalObject(_jsContext)) == NULL){
    KSLogError("No global object; cannot restore JavaScript context");
    status = KSStatusError;
    goto error;
  }
  
  JSObjectSetProperty(_jsContext, global, document, peer, kJSPropertyAttributeDontDelete | kJSPropertyAttributeReadOnly, NULL);
  
error:
  JSStringRelease(document);
  
  return status;
}

/**
 * Obtain the filter destination path.  This is the path to the processed
 * file.
 * 
 * @return destination path
 */
CFStringRef ALHTMLSourceFilter::getFilterDestinationPath(void) const {
  const ALSourceFilterContext *context = getContextInfo();
  return (context != NULL) ? context->getDestinationPath() : NULL;
}

/**
 * Obtain the filter base path
 * 
 * @return base path
 */
CFStringRef ALHTMLSourceFilter::getFilterSourceBasePath(void) const {
  const ALSourceFilterContext *context = getContextInfo();
  return (context != NULL) ? context->getSourceBasePath() : NULL;
}

/**
 * Obtain this filter's JavaScript peer object
 * 
 * @return JavaScript peer
 */
JSObjectRef ALHTMLSourceFilter::getJavaScriptPeer(void) const {
  return _jsPeer;
}

/**
 * Obtain the current output stream for this filter.
 * 
 * @return output stream
 */
KSOutputStream * ALHTMLSourceFilter::getCurrentOutputStream(void) const {
  return _currentOutputStream;
}

/**
 * Filter
 * 
 * @param ins input stream
 * @param outs output stream
 */
KSStatus ALHTMLSourceFilter::filter(KSInputStream *ins, KSOutputStream *outs) const {
  KSStatus status = KSStatusOk;
  ALDirectiveList *parent = NULL;
  ALDirective *subdir = NULL;
  const ALDirective *enclosing = NULL;
  
  ALWAMLParser *parser = NULL;
  ALTreeParser *tree = NULL;
  ALElement *root = NULL;
  KSError *error = NULL;
  
  JSObjectRef parentPeer = NULL;
  
  // setup the current output stream
  _currentOutputStream = KSRetain(outs);
  
  // check parameters
  if(ins == NULL || outs == NULL){
    KSLogError("Input or output stream is null; cannot filter text");
    status = KSStatusError;
    goto error;
  }
  
  // Omit base (option model) preferences when a parent directive is present.
  // Otherwise, the model may shadow properties defined in the model and
  // overridden in the enclosing directive.  The enclosing directive already
  // has a path to the model at its' root (where it should be).
  
  enclosing = getParentDirective();
  parent = new ALDirectiveList(enclosing, (enclosing == NULL) ? getOptionsModel()->getProperties() : NULL);
  parser = new ALWAMLParser(gSharedOptions);
  tree = new ALTreeParser(parser);
  
  if((status = tree->parse(ins, &root, &error)) != KSStatusOk){
    KSLogError("Unable to parse document: %@", (error != NULL) ? KSUnwrap(error->getMessage()) : KSStatusGetMessage(status));
    goto error;
  }
  
  CFArrayRef  children;
  CFStringRef content;
  
  if(root != NULL && (children = root->getChildren()) != NULL){
    
    register int i;
    for(i = 0; i < CFArrayGetCount(children); i++){
      ALElement *element = (ALElement *)CFArrayGetValueAtIndex(children, i);
      
      switch(element->getType()){
        
        case ALElementTypeElement:
          if((subdir = ALDirective::createFromWAML(getOptionsModel(), parent, NULL, element, &status)) != NULL){
            parent->addDirective(subdir);
            KSRelease(subdir); subdir = NULL;
          }else{
            goto error;
          }
          break;
          
        case ALElementTypeContent:
          if((content = element->getContent()) != NULL){
            subdir = new ALVerbatimDirective(parent, NULL, content);
            parent->addDirective(subdir);
            KSRelease(subdir); subdir = NULL;
          }
          break;
          
        default:
          KSLogError("Error: unknown element type");
          status = KSStatusError;
          goto error;
          
      }
      
    }
    
  }
  
  if((status = pushJSContext(&parentPeer)) != KSStatusOk){
    KSLogError("Unable to push JS context: %@", KSStatusGetMessage(status));
    goto error;
  }
  
  if((status = parent->emit(this, outs, _jsContext, getContextInfo())) != KSStatusOk){
    KSLogError("An error occured while emitting directives");
    goto error;
  }
  
  if((status = restoreJSContext(parentPeer)) != KSStatusOk){
    KSLogError("Unable to restore JS context: %@", KSStatusGetMessage(status));
    goto error;
  }
  
error:
  if(parent)  KSRelease(parent);
  if(parser)  KSRelease(parser);
  if(tree)    KSRelease(tree);
  if(root)    KSRelease(root);
  if(error)   KSRelease(error);
  
  // tear down the current output stream
  KSRelease(_currentOutputStream);
  
  return status;
}

/**
 * Filter text to the specified output stream.
 * 
 * @param fulltext text to process
 * @param outs processed text output stream
 * @return status
 */
KSStatus ALHTMLSourceFilter::filter(CFStringRef fulltext, KSOutputStream *outs) const {
  return KSStatusUnimplemented;
}

/**
 * Process and write a directive
 * 
 * @param outs output stream
 * @param directive directive content
 * @param context context info
 * @return status
 */
KSStatus ALHTMLSourceFilter::writeDirective(KSOutputStream *outs, ALElement *directive, ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  KSStatus vstatus;
  
  if(getOptionsModel()->isDebugging()){
    KSLog("Processing directive: <%@>", directive->getName());
  }
  
  ALDirective *vdirective;
  if((vdirective = ALDirective::createFromWAML(getOptionsModel(), getParentDirective(), getOptionsModel()->getProperties(), directive, &vstatus)) == NULL){
    status = vstatus;
    goto error;
  }
  
  if((vstatus = vdirective->emit(this, outs, _jsContext, context)) != KSStatusOk){
    KSLogError("An error occured while emitting directives");
    status = vstatus;
    goto error;
  }
  
error:
  if(vdirective) KSRelease(vdirective);
  
  return status;
}

