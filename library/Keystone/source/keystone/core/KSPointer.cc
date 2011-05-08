// 
// $Id: KSPointer.cc 11 2008-11-26 09:26:58Z brian $
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

#include "KSPointer.hh"
#include "KSLog.hh"

/**
 * Construct
 * 
 * @param path file path
 */
KSPointer::KSPointer(void *pointer, bool f) {
  _pointer = pointer;
  _freeOnDealloc = f;
}

/**
 * Clean up
 */
KSPointer::~KSPointer() {
  if(_freeOnDealloc){
    if(_pointer) free(_pointer);
  }
}

/**
 * Obtain the pointer
 * 
 * @return pointer
 */
void * KSPointer::getPointer(void) const {
  return _pointer;
}

/**
 * Determine if the pointer will be freed on deallocation.  A null pointer
 * will never be freed.
 * 
 * @return free on dealloc
 */
bool KSPointer::getFreeOnDealloc(void) const {
  return _freeOnDealloc;
}

