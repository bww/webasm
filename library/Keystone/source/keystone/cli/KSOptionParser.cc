// 
// $Id: KSOptionParser.cc 69 2010-01-28 00:34:07Z brian $
// Keystone Framework
// 
// Copyright (c) 2010 Wolter Group New York, Inc., All rights reserved.
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

#include "KSOptionParser.hh"
#include "KSLog.hh"

/**
 * Construct
 * 
 * @param options options definition
 * @param argc argument count
 * @param argv argument values
 */
KSOptionParser::KSOptionParser(KSOptionParserOptionSet options, int argc, char * const argv[]) {
  
  if(options != NULL){
    register int i;
    
    KSOptionParserOptionRef co = options;
    for(i = 0; co != NULL && co->name != NULL; i++, co++) /* continue */ ;
    
    _options = (KSOptionParserOptionSet)malloc(sizeof(KSOptionParserOption) * (i + 1));
    memcpy(_options, options, sizeof(KSOptionParserOption) * (i + 1));
    
  }else{
    _options = NULL;
  }
  
  _argc     = argc;
  _argv     = (const char **)argv;
  _findex   = 0;
  _argument = NULL;
  _current  = NULL;
  
}

/**
 * Clean up
 */
KSOptionParser::~KSOptionParser() {
  if(_options)  free(_options);
}

/**
 * Obtain the index of the current argument
 * 
 * @return current index
 */
int KSOptionParser::getIndex(void) const {
  return _findex;
}

/**
 * Obtain the argument value of the current argument
 * 
 * @return current value
 */
const char * KSOptionParser::getArgument(void) const {
  return _argument;
}

/**
 * Parse the next option.
 * 
 * @param id on output: the next option identifier
 * @param error on output, if status is other than KSStatusOk:
 * an error describing the problem.
 * @return status
 */
KSStatus KSOptionParser::getNextOption(KSFourCharCode *id, KSError **error) {
  KSStatus status = KSStatusOk;
  KSOptionParserOptionRef option;
  
  _argument = NULL;
  _current  = NULL;
  
  if(_argc < 1 || _argv == NULL || _options == NULL || _findex >= _argc){
    if(id) *id = kKSOptionParserEndOptions;
    goto error;
  }
  
  if((status = getNextFlag(_argv[_findex], &option, error)) != KSStatusOk) goto error;
  
  if(option == NULL){
    if(id) *id = kKSOptionParserEndOptions;
    goto error;
  }
  
  if(id) *id = option->id;
  
  _findex++;
  _current = option;
  
  switch(option->argument){
    
    case kKSOptionNoArgument:
      /* Nothing ... */
      break;
      
    case kKSOptionArgumentOptional:
      if(_findex < _argc && !isOptionFlag(_argv[_findex])){
        _argument = _argv[_findex++];
      }else{
        _argument = option->dvalue;
      }
      break;
      
    case kKSOptionArgumentRequired:
      if(_findex < _argc && !isOptionFlag(_argv[_findex])){
        _argument = _argv[_findex++];
      }else{
        status = KSStatusError;
        if(error) *error = new KSError(status, "Option -%s requires an argument", option->name);
        goto error;
      }
      break;
      
  }
  
error:
  return status;
}

/**
 * Obtain the next option
 * 
 * @param v argument
 * @param option on output: the option
 * @param error on output if status is other than KSStatusOk:
 * an error describing the problem
 * @return status
 */
KSStatus KSOptionParser::getNextFlag(const char *v, KSOptionParserOptionRef *option, KSError **error) {
  KSStatus status = KSStatusOk;
  KSOptionParserOptionRef op = _options;
  
  if(option) *option = NULL;
  if(v == NULL || strlen(v) < 2 || v[0] != '-') goto error;
  
  for( ; op != NULL && op->name != NULL; op++){
    if(strcasecmp(op->name, (v + 1)) == 0){
      if(option) *option = op;
      break;
    }
  }
  
  if(op == NULL || op->name == NULL){
    status = KSStatusError;
    if(error) *error = new KSError(status, "No such option: %s", v);
    goto error;
  }
  
error:
  return status;
}

/**
 * Determine if the specified argument is an option flag or not
 * 
 * @param v argument
 * @param option on output: the flag, if the result is true
 * @return option flag or not
 */
bool KSOptionParser::isOptionFlag(const char *v, KSOptionParserOptionRef *option) {
  KSOptionParserOptionRef op = _options;
  
  if(v == NULL || strlen(v) < 2 || v[0] != '-'){
    return false;
  }
  
  for( ; op != NULL && op->name != NULL; op++){
    if(strcasecmp(op->name, (v + 1)) == 0){
      if(option) *option = op;
      return true;
    }
  }
  
  return false;
}

