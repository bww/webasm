// 
// $Id: KSMutableArray.cc 24 2008-12-20 21:15:53Z brian $
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

#include "KSMutableArray.hh"

#define super KSArray

/**
 * Construct
 */
KSMutableArray::KSMutableArray() {
}

/**
 * Construct
 * 
 * @param array initial contents
 */
KSMutableArray::KSMutableArray(KSArray *array) : super(array) {
}

/**
 * Clean up
 */
KSMutableArray::~KSMutableArray() {
}

/**
 * Add an object to the end of this array
 * 
 * @param object the object to add
 */
void KSMutableArray::addObject(KSObject *object) {
  CFArrayAppendValue(_array, object);
}

/**
 * Add objects from the specified array
 * 
 * @param array objects to add
 */
void KSMutableArray::addObjectsFromArray(KSArray *array) {
  
  register int i;
  for(i = 0; i < array->getCount(); i++){
    addObject(array->getObjectAtIndex(i));
  }
  
}

/**
 * Add an object at the specified index
 * 
 * @param i index to add at
 * @param object the object
 */
void KSMutableArray::addObjectAtIndex(KSIndex i, KSObject *object) {
  CFArrayInsertValueAtIndex(_array, i, object);
}

/**
 * Set the object at the specified index
 * 
 * @param i index to set
 * @param object the object
 */
void KSMutableArray::setObjectAtIndex(KSIndex i, KSObject *object) {
  CFArraySetValueAtIndex(_array, i, object);
}

/**
 * Remove an object at the specified index
 * 
 * @param i index to remove
 */
void KSMutableArray::removeObjectAtIndex(KSIndex i) {
  CFArrayRemoveValueAtIndex(_array, i);
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSMutableArray::copyDescription(void) const {
  return CFStringCreateWithFormat(NULL, NULL, CFSTR("[KSMutableArray %d]"), getCount());
}


