// 
// $Id: KSCharacter.cc 22 2008-12-16 03:10:41Z brian $
// Keystone Framework
// 
// Copyright (c) 2008 Wolter Group New York, Inc., All rights reserved.
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

#include "KSCharacter.hh"

/**
 * Construct
 * 
 * @param c character
 */
KSCharacter::KSCharacter(char c) {
  _character = (KSUniChar)c;
}

/**
 * Construct
 * 
 * @param c character
 */
KSCharacter::KSCharacter(KSUniChar c) {
  _character = c;
}

/**
 * Clean up
 */
KSCharacter::~KSCharacter() {
}

/**
 * Obtain the character value
 * 
 * @return character value
 */
KSUniChar KSCharacter::getCharacterValue(void) const {
  return _character;
}

/**
 * Compare
 * 
 * @param c comapre to
 * @return comparison result
 */
CFComparisonResult KSCharacter::compare(KSCharacter *c) const {
  if(c->_character == _character)     return kCFCompareEqualTo;
  else if(c->_character < _character) return kCFCompareLessThan;
  else return kCFCompareGreaterThan;
}

/**
 * Determine semantic equality
 * 
 * @param o string
 * @return equal or not
 */
bool KSCharacter::isEqual(KSObject *o) const {
  if(o == NULL || o->getTypeID() != KSCharacterTypeID) return false;
  return (o != NULL && (((KSCharacter *)o)->_character == _character));
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSCharacter::copyDescription(void) const {
  return CFStringCreateWithFormat(NULL, NULL, CFSTR("%c"), _character);
}


