// 
// $Id: ALWAMLParser.cc 157 2010-08-16 19:16:22Z brian $
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

#include "ALWAMLParser.hh"

#include <Keystone/KSLog.hh>
#include <Keystone/CFStringAdditions.hh>

#define super ALParser

#define kAssemblerTagOpen             '<'
#define kAssemblerTagOpenPrefix       CFSTR("w:")
#define kAssemblerTagClose            '>'
#define kAssemblerTagClosePrefix      CFSTR("/w:")
#define kAssemblerTagCloseSuffix      CFSTR("/>")
#define kAssemblerCDATAOpenPrefix     CFSTR("![CDATA[")
#define kAssemblerCDATAClose          ']'
#define kAssemblerCDATACloseSuffix    CFSTR("]>")
#define kAssemblerAttrAssign          '='
#define kAssemblerAttrValueEnclosure  '"'
#define kAssemblerIdentCharacters     CFSTR("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-")

/**
 * Construct
 * 
 * @param model options
 * @param delegate parser delegate
 */
ALWAMLParser::ALWAMLParser(const ALOptionsModel *model, ALParserDelegate *delegate) : super(model, delegate) {
}

/**
 * Clean up
 */
ALWAMLParser::~ALWAMLParser() {
}

/**
 * Parse a block of text
 * 
 * @param ins text to parse
 * @param on output: the error, if the status returned is not KSStatusOk
 * @param info user data
 * @return status
 */
KSStatus ALWAMLParser::parse(KSInputStream *ins, KSError **error, void *info) const {
  KSStatus status = KSStatusOk;
  CFMutableDataRef buffer = NULL;
  CFStringRef fulltext = NULL;
  KSByte *chunk = NULL;
  KSLength chunklen = 1024;
  KSLength actual = 0;
  
  buffer = CFDataCreateMutable(NULL, 0);
  chunk = (KSByte *)malloc(chunklen);
  
  while((status = ins->read(chunk, chunklen, &actual)) == KSStatusOk && actual > 0){
    CFDataAppendBytes(buffer, chunk, actual);
  }
  
  free(chunk);
  
  if(status != KSStatusOk){
    if(error) *error = new KSError(status, "Error reading document source; partial read");
    goto error;
  }
  
  if((fulltext = CFStringCreateWithBytes(NULL, CFDataGetBytePtr(buffer), CFDataGetLength(buffer), kDefaultStringEncoding, false)) == NULL){
    status = KSStatusError;
    if(error) *error = new KSError(status, "Unable to load document source; invalid string encoding, perhaps? Use UTF-8.");
    goto error;
  }
  
  if((status = parse(fulltext, error, info)) != KSStatusOk){
    goto error;
  }
  
error:
  if(buffer) CFRelease(buffer);
  if(fulltext) CFRelease(fulltext);
  
  return status;
}

/**
 * Parse a block of text
 * 
 * @param fulltext text to parse
 * @param on output: the error, if the status returned is not KSStatusOk
 * @param info user data
 * @return status
 */
KSStatus ALWAMLParser::parse(CFStringRef fulltext, KSError **error, void *info) const {
  KSStatus status = KSStatusOk;
  CFMutableArrayRef stack = NULL;
  CFMutableStringRef cdata = CFStringCreateMutable(NULL, 0);
  
  CFIndex length = 0;
  bool incdata = false;
  
  if(fulltext == NULL){
    status = KSStatusInvalidParameter;
    goto error;
  }
  
  stack = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
  length = CFStringGetLength(fulltext);
  
  register CFIndex i;
  for(i = 0; i < CFStringGetLength(fulltext); ){
    UniChar c = CFStringGetCharacterAtIndex(fulltext, i);
    CFRange range;
    
    switch(c){
      case kAssemblerCDATAClose:
        if(matchAhead(fulltext, i + 1, 1, kAssemblerCDATACloseSuffix, &range)){
          i = (range.location + range.length);
          if(cdata != NULL) parseCharacterData(&cdata, stack, error, info);
          incdata = false;
        }else{
          CFStringAppendCharacters(cdata, &c, 1);
          i++;
        }
        break;
      case kAssemblerTagOpen:
        if(incdata){
          CFStringAppendCharacters(cdata, &c, 1);
          i++;
        }else if(matchAhead(fulltext, i + 1, 1, kAssemblerTagOpenPrefix, &range)){
          i = (range.location + range.length);
          if(cdata != NULL) parseCharacterData(&cdata, stack, error, info);
          if((status = parseTagOpen(fulltext, &i, stack, error, info)) != KSStatusOk){
            goto error;
          }
        }else if(matchAhead(fulltext, i + 1, 1, kAssemblerTagClosePrefix, &range)){
          i = (range.location + range.length);
          if(cdata != NULL) parseCharacterData(&cdata, stack, error, info);
          if((status = parseTagClose(fulltext, &i, stack, error, info)) != KSStatusOk){
            goto error;
          }
        }else if(matchAhead(fulltext, i + 1, 1, kAssemblerCDATAOpenPrefix, &range)){
          i = (range.location + range.length);
          incdata = true;
        }else{
          CFStringAppendCharacters(cdata, &c, 1);
          i++;
        }
        break;
      default:
        CFStringAppendCharacters(cdata, &c, 1);
        i++;
        break;
    }
    
  }
  
  if(cdata != NULL) parseCharacterData(&cdata, stack, error, info);
  
error:
  if(stack) CFRelease(stack);
  if(cdata) CFRelease(cdata);
  
  if(status != KSStatusOk && _delegate != NULL){
    _delegate->handleParseError(this, status, ((error != NULL) ? (CFStringRef)KSUnwrap((*error)->getMessage()) : CFSTR("Unknown error")), info);
  }
  
  return status;
}

/**
 * Parse an opening tag
 * 
 * @param fulltext text to parse
 * @param ix current character index
 * @param stack current tag stack
 * @param on output: the error, if the status returned is not KSStatusOk
 * @return status
 */
KSStatus ALWAMLParser::parseTagOpen(CFStringRef fulltext, CFIndex *index, CFArrayRef stack, KSError **error, void *info) const {
  KSStatus status = KSStatusOk;
  CFCharacterSetRef ident = CFCharacterSetCreateWithCharactersInString(NULL, kAssemblerIdentCharacters);
  CFMutableStringRef name = CFStringCreateMutable(NULL, 0);
  UniChar c;
  
  register CFIndex i = 0;
  
  if(index == NULL){
    status = KSStatusInvalidParameter;
    goto error;
  }
  
  // tag name
  for(i = *index; i < CFStringGetLength(fulltext); i++){
    c = CFStringGetCharacterAtIndex(fulltext, i);
    if(CFCharacterSetIsCharacterMember(ident, c)){
      CFStringAppendCharacters(name, &c, 1);
    }else{
      break;
    }
  }
  
  if(_delegate){
    if((status = _delegate->openElement(this, name, info, error)) != KSStatusOk){
      goto error;
    }
  }
  
  skipWhitespace(fulltext, &i);
  
  if(i < CFStringGetLength(fulltext)){
    c = CFStringGetCharacterAtIndex(fulltext, i);
  }else{
    status = KSStatusError;
    goto error;
  }
  
  bool found;
  while((status = parseAttribute(fulltext, &i, stack, &found, error, info)) == KSStatusOk && found){
    skipWhitespace(fulltext, &i);
  }
  
  CFRange range;
  if(matchAhead(fulltext, i, 0, kAssemblerTagCloseSuffix, &range)){
    i = (range.location + range.length);
    if(_delegate){
      if((status = _delegate->closeElement(this, name, info, error)) != KSStatusOk){
        goto error;
      }
    }
  }else if((c = CFStringGetCharacterAtIndex(fulltext, i)) == kAssemblerTagClose){
    i++;
  }else{
    if(error) *error = new KSError(status, "Expected closing marker '>', got '%c'", c);
    status = KSStatusError;
    goto error;
  }
  
  if(index) *index = i;
  
error:
  if(name)  CFRelease(name);
  if(ident) CFRelease(ident);
  
  return status;
}

/**
 * Parse a tag attribute
 * 
 * @param fulltext text to parse
 * @param ix current character index
 * @param stack current tag stack
 * @param on output: the error, if the status returned is not KSStatusOk
 * @return status
 */
KSStatus ALWAMLParser::parseAttribute(CFStringRef fulltext, CFIndex *index, CFArrayRef stack, bool *found, KSError **error, void *info) const {
  KSStatus status = KSStatusOk;
  CFCharacterSetRef ident = CFCharacterSetCreateWithCharactersInString(NULL, kAssemblerIdentCharacters);
  CFMutableStringRef name = CFStringCreateMutable(NULL, 0);
  CFMutableStringRef value = CFStringCreateMutable(NULL, 0);
  UniChar c;
  
  register CFIndex i = 0;
  
  if(found) *found = false;
  
  if(index == NULL){
    status = KSStatusInvalidParameter;
    goto error;
  }
  
  i = *index;
  
  // check for a valid attribute name
  if(!(i < CFStringGetLength(fulltext)) || !CFCharacterSetIsCharacterMember(ident, (c = CFStringGetCharacterAtIndex(fulltext, i)))){
    // no error; just continue processing
    goto error;
  }
  
  // attribute name
  for( ; i < CFStringGetLength(fulltext); i++){
    c = CFStringGetCharacterAtIndex(fulltext, i);
    if(CFCharacterSetIsCharacterMember(ident, c)){
      CFStringAppendCharacters(name, &c, 1);
    }else{
      break;
    }
  }
  
  skipWhitespace(fulltext, &i);
  
  // check for an assignment
  if(c != kAssemblerAttrAssign){
    if(error) *error = new KSError(status, "Attribute does not have an assignment: %c", c);
    status = KSStatusError;
    goto error;
  }
  
  skipWhitespace(fulltext, &(++i));
  
  // check for an enclosure
  if(!(i < CFStringGetLength(fulltext)) || (c = CFStringGetCharacterAtIndex(fulltext, i++)) != kAssemblerAttrValueEnclosure){
    if(error) *error = new KSError(status, "Attribute value does not have an enclosure (should be %c): %c", kAssemblerAttrValueEnclosure, c);
    status = KSStatusError;
    goto error;
  }
  
  // attribute value
  for( ; i < CFStringGetLength(fulltext); i++){
    if((c = CFStringGetCharacterAtIndex(fulltext, i)) != kAssemblerAttrValueEnclosure){
      CFStringAppendCharacters(value, &c, 1);
    }else{
      break;
    }
  }
  
  if(c != kAssemblerAttrValueEnclosure){
    if(error) *error = new KSError(status, "Attribute value does not end with an enclosure (should be %c): %c", kAssemblerAttrValueEnclosure, c);
    status = KSStatusError;
    goto error;
  }
  
  if(_delegate){
    if((status = _delegate->addElementAttribute(this, name, value, info, error)) != KSStatusOk){
      goto error;
    }
  }
  
  if(found) *found = true;
  if(index) *index = ++i;
  
error:
  if(name)  CFRelease(name);
  if(value) CFRelease(value);
  if(ident) CFRelease(ident);
  
  return status;
}

/**
 * Parse a closing tag
 * 
 * @param fulltext text to parse
 * @param ix current character index
 * @param stack current tag stack
 * @param on output: the error, if the status returned is not KSStatusOk
 * @return status
 */
KSStatus ALWAMLParser::parseTagClose(CFStringRef fulltext, CFIndex *index, CFArrayRef stack, KSError **error, void *info) const {
  KSStatus status = KSStatusOk;
  CFCharacterSetRef ident = CFCharacterSetCreateWithCharactersInString(NULL, kAssemblerIdentCharacters);
  CFMutableStringRef name = CFStringCreateMutable(NULL, 0);
  UniChar c;
  
  register CFIndex i = 0;
  
  if(index == NULL){
    status = KSStatusInvalidParameter;
    goto error;
  }
  
  // tag name
  for(i = *index; i < CFStringGetLength(fulltext); i++){
    c = CFStringGetCharacterAtIndex(fulltext, i);
    if(CFCharacterSetIsCharacterMember(ident, c)){
      CFStringAppendCharacters(name, &c, 1);
    }else{
      break;
    }
  }
  
  skipWhitespace(fulltext, &i);
  
  // closing tag
  if(!(i < CFStringGetLength(fulltext)) || (c = CFStringGetCharacterAtIndex(fulltext, i)) != kAssemblerTagClose){
    if(error) *error = new KSError(status, "Closing tag is malformed (should be %c): %c", kAssemblerTagClose, c);
    status = KSStatusError;
    goto error;
  }
  
  if(_delegate){
    if((status = _delegate->closeElement(this, name, info, error)) != KSStatusOk){
      goto error;
    }
  }
  
  if(index) *index = ++i;
  
error:
  if(name)  CFRelease(name);
  if(ident) CFRelease(ident);
  
  return status;
}

/**
 * Parse character data
 * 
 * @param cdata character data
 * @param stack current tag stack
 * @param on output: the error, if the status returned is not KSStatusOk
 * @return status
 */
KSStatus ALWAMLParser::parseCharacterData(CFMutableStringRef *cdata, CFArrayRef stack, KSError **error, void *info) const {
  KSStatus status = KSStatusOk;
  
  if(cdata == NULL || *cdata == NULL){
    if(error) *error = new KSError(status, "Character data is null");
    status = KSStatusError;
    goto error;
  }
  
  if(_delegate != NULL && CFStringGetLength(*cdata) > 0){
    if((status = _delegate->addCharacterData(this, *cdata, info, error)) != KSStatusOk){
      goto error;
    }
  }
  
  CFRelease(*cdata);
  *cdata = CFStringCreateMutable(NULL, 0);
  
error:
  return status;
}

/**
 * Skip whitespace
 * 
 * @param fulltext text to match within
 * @param ix matching index
 * @param range matched whitespace range
 */
void ALWAMLParser::skipWhitespace(CFStringRef fulltext, CFIndex *index) const {
  if(fulltext != NULL && index != NULL){
    CFCharacterSetRef white = CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline);
    register CFIndex i;
    for(i = *index; i < CFStringGetLength(fulltext); i++){
      if(!CFCharacterSetIsCharacterMember(white, CFStringGetCharacterAtIndex(fulltext, i))) break;
    }
    *index = i;
  }
}

/**
 * Match ahead in a string
 * 
 * @param fulltext text to match within
 * @param ix matching index
 * @param room the minimum number of characters following the match
 * @param match string to match
 * @param range matched range
 * @return matched or not
 */
bool ALWAMLParser::matchAhead(CFStringRef fulltext, CFIndex index, CFIndex room, CFStringRef match, CFRange *range) const {
  if(fulltext != NULL && match != NULL){
    CFIndex tlength = CFStringGetLength(fulltext);
    CFIndex mlength = CFStringGetLength(match);
    return (tlength > (index + mlength + room) && CFStringFindWithOptions(fulltext, match, CFRangeMake(index, mlength), kCFCompareCaseInsensitive, range));
  }else{
    if(range) *range = CFRangeMake(index, 0);
    return false;
  }
}

