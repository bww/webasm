// 
// $Id: ALParser.cc 39 2009-10-28 23:43:53Z brian $
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

#include "ALParser.hh"
#include <Keystone/KSLog.hh>

/**
 * Construct
 * 
 * @param model options
 * @param delegate parser delegate
 */
ALParser::ALParser(const ALOptionsModel *model, ALParserDelegate *delegate) {
  _options = (model != NULL) ? (ALOptionsModel *)KSRetain(model) : NULL;
  _delegate = (delegate != NULL) ? (ALParserDelegate *)KSRetain(delegate) : NULL;
}

/**
 * Clean up
 */
ALParser::~ALParser() {
  if(_options)  KSRelease(_options);
  if(_delegate) KSRelease(_delegate);
}

/**
 * Obtain the options model
 * 
 * @return options model
 */
const ALOptionsModel * ALParser::getOptionsModel(void) const {
  return _options;
}

/**
 * Obtain the parser delegate
 * 
 * @return parser delegate
 */
ALParserDelegate * ALParser::getDelegate(void) const {
  return _delegate;
}

/**
 * Set the parser delegate
 * 
 * @param delegate parser delegate
 */
void ALParser::setDelegate(ALParserDelegate *delegate) {
  if(_delegate) KSRelease(_delegate);
  _delegate = (delegate != NULL) ? (ALParserDelegate *)KSRetain(delegate) : NULL;
}


