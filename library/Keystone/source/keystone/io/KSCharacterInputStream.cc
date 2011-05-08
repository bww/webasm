// 
// $Id: KSCharacterInputStream.cc 54 2010-01-21 23:16:48Z brian $
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

#include "KSCharacterInputStream.hh"
#include "KSLog.hh"
#include "KSUtility.hh"

#define kDefaultByteChunkLength 256

/**
 * Construct
 * 
 * @param stream the underlying input stream to read data from
 */
KSCharacterInputStream::KSCharacterInputStream(KSInputStream *stream, CFStringEncoding encoding, bool bom) {
  _istream  = (stream != NULL) ? (KSInputStream *)KSRetain(stream) : NULL;
  _buffer   = CFStringCreateMutable(NULL, 0);
  _encoding = encoding;
  _bom      = bom;
}

/**
 * Clean up
 */
KSCharacterInputStream::~KSCharacterInputStream() {
  if(_istream)  KSRelease(_istream);
  if(_buffer)   CFRelease(_buffer);
}

/**
 * Obtain the underlying input stream
 * 
 * @return input stream
 */
KSInputStream * KSCharacterInputStream::getInputStream(void) const {
  return _istream;
}

/**
 * Obtain the character encoding used by this stream.
 * 
 * @return character encoding
 */
CFStringEncoding KSCharacterInputStream::getEncoding(void) const {
  return _encoding;
}

/**
 * Determine whether a byte-order-marker (BOM) is used by the character
 * encoding.
 * 
 * @return uses byte-order-marker
 */
bool KSCharacterInputStream::useByteOrderMarker(void) const {
  return _bom;
}

/**
 * Obtain the current buffer length, in characters
 * 
 * @return buffer length
 */
CFIndex KSCharacterInputStream::getBufferLength(void) const {
  return CFStringGetLength(_buffer);
}

/**
 * Read from the underlying input stream.  This method bypasses character procesing
 * and reads directly from the underlying input stream.  Bytes read via this method
 * will not be read by subsequent calls to #readCharacters.
 * 
 * @param buffer buffer
 * @param count bytes to read
 * @param actual on output: actual byte count read
 * @return status
 */
KSStatus KSCharacterInputStream::read(KSByte *bytes, KSLength count, KSLength *actual) {
  return ((_istream != NULL) ? _istream->read(bytes, count, actual) : KSStatusInvalidState);
}

/**
 * Read a single character from the underlying input stream
 * 
 * @param character on output provided status is KSStatusOk: the character
 * @return status
 */
KSStatus KSCharacterInputStream::readCharacter(UniChar *character, KSLength *actual) {
  KSStatus status = KSStatusOk;
  CFStringRef string = NULL;
  KSLength vactual;
  
  if(actual) *actual = 0;
  
  if((status = readCharacters(&string, 1, &vactual)) != KSStatusOk){
    goto error;
  }
  if(string != NULL && CFStringGetLength(string) > 0 && character != NULL){
    *character = CFStringGetCharacterAtIndex(string, 0);
  }
  
  if(actual) *actual = vactual;
  
error:
  CFReleaseSafe(string);
  
  return status;
}

/**
 * Read characters from the underlying input stream.
 * 
 * @param string on output: a string containing the characters.  The caller must
 * release this string when through with it.
 * @param count the number of characters to read
 * @param actual on output: the number of characters actualy read
 * @return status
 */
KSStatus KSCharacterInputStream::readCharacters(CFStringRef *string, KSLength count, KSLength *actual) {
  KSStatus status = KSStatusOk;
  KSLength chunklen = kDefaultByteChunkLength;
  KSByte chunk[chunklen];
  
  while(CFStringGetLength(_buffer) < count){
    
    KSLength z;
    if((status = read(chunk, chunklen, &z)) != KSStatusOk){
      goto error;
    }
    
    // check for end of stream
    if(z < 1) break;
    
    CFStringRef temp = CFStringCreateWithBytes(NULL, chunk, z, getEncoding(), useByteOrderMarker());
    CFStringAppend(_buffer, temp);
    if(temp) CFRelease(temp);
    
  }
  
  if(string){
    CFRange range = CFRangeMake(0, KSMin(count, CFStringGetLength(_buffer)));
    *string = (range.length > 0) ? CFStringCreateWithSubstring(NULL, _buffer, range) : NULL;
    CFStringDelete(_buffer, range);
  }
  
  if(actual){
    *actual = KSMin(count, CFStringGetLength(_buffer));
  }
  
error:
  return status;
}

/**
 * Find a character offset in the buffer beginning from the specified index.
 * 
 * @param term terminating character set
 * @param start start index
 * @return first match index or < 0 if no match found
 */
KSLength KSCharacterInputStream::findCharacterOffsetInBuffer(CFCharacterSetRef term, KSLength start) {
  
  register int i;
  for(i = start; i < CFStringGetLength(_buffer); i++){
    UniChar c = CFStringGetCharacterAtIndex(_buffer, i);
    if(CFCharacterSetIsCharacterMember(term, c)){
      return i;
    }
  }
  
  return -1;
}

/**
 * Read from the underlying stream until a terminating character in the specified set is
 * encountered.  The terminating character is read, but discarded.
 * 
 * @param string on output: a string containing the characters.  The caller must
 * release this string when through with it.
 * @param term the terminator character set
 * @param options read options
 * @param actual on output: the actual number of characters read
 * @return status
 */
KSStatus KSCharacterInputStream::readUntilCharacterInSetAsCFString(CFStringRef *string, CFCharacterSetRef term, KSCharacterInputStreamOptions options, KSLength *actual) {
  KSStatus status = KSStatusOk;
  KSLength chunklen = kDefaultByteChunkLength;
  KSByte chunk[chunklen];
  
  KSLength offset =  0;
  
  while(true){
    
    // check for terminating character
    if((offset = findCharacterOffsetInBuffer(term, offset)) < 0){
      offset = CFStringGetLength(_buffer);
    }else{
      break;
    }
    
    KSLength z;
    if((status = read(chunk, chunklen, &z)) != KSStatusOk){
      goto error;
    }
    
    // check for end of stream
    if(z < 1) break;
    
    CFStringRef temp = CFStringCreateWithBytes(NULL, chunk, z, getEncoding(), useByteOrderMarker());
    CFStringAppend(_buffer, temp);
    if(temp) CFRelease(temp);
    
  }
  
  if(string){
    CFRange range = CFRangeMake(0, KSMin(offset, CFStringGetLength(_buffer)));
    *string = (range.length > 0) ? CFStringCreateWithSubstring(NULL, _buffer, range) : NULL;
    CFStringDelete(_buffer, CFRangeMake(0, KSMin(((options & KSCharacterInputStreamOptionDiscardDelimiter) == KSCharacterInputStreamOptionDiscardDelimiter) ? offset + 1 : offset, CFStringGetLength(_buffer))));
  }
  
  if(actual){
    *actual = KSMin(offset, CFStringGetLength(_buffer));
  }
  
error:
  return status;
}

/**
 * Read from the underlying stream until a newline character or end-of-stream
 * is encountered.
 * 
 * @param string on output: a string containing the characters.  The caller must
 * release this string when through with it.
 * @param options read options
 * @param actual on output: the actual number of characters read
 * @return status
 */
KSStatus KSCharacterInputStream::readLineAsCFString(CFStringRef *string, KSCharacterInputStreamOptions options, KSLength *actual) {
  KSStatus status = KSStatusOk;
  CFCharacterSetRef term = CFCharacterSetCreateWithCharactersInString(NULL, CFSTR("\n"));
  KSLength chunklen = kDefaultByteChunkLength;
  KSByte chunk[chunklen];
  
  KSLength offset =  0;
  
  while(true){
    
    // check for terminating character
    if((offset = findCharacterOffsetInBuffer(term, offset)) < 0){
      offset = CFStringGetLength(_buffer);
    }else{
      break;
    }
    
    KSLength z;
    if((status = read(chunk, chunklen, &z)) != KSStatusOk){
      goto error;
    }
    
    // check for end of stream
    if(z < 1) break;
    
    CFStringRef temp = CFStringCreateWithBytes(NULL, chunk, z, getEncoding(), useByteOrderMarker());
    CFStringAppend(_buffer, temp);
    if(temp) CFRelease(temp);
    
  }
  
  if(string){
    CFRange range = CFRangeMake(0, KSMin(offset, CFStringGetLength(_buffer)));
    if(range.length > 0){
      *string = CFStringCreateWithSubstring(NULL, _buffer, (CFStringGetCharacterAtIndex(_buffer, range.location + range.length - 1) == '\r') ? CFRangeMake(range.location, range.length - 1) : range);
    }else{
      *string = NULL;
    }
    CFStringDelete(_buffer, CFRangeMake(0, KSMin(offset + 1, CFStringGetLength(_buffer))));
  }
  
  if(actual){
    *actual = KSMin(offset, CFStringGetLength(_buffer));
  }
  
error:
  if(term) CFRelease(term);
  
  return status;
}

/**
 * Close the underlying input stream.  This call is passed directly through
 * to the underlying stream.  Data that has been buffered from the stream but
 * not yet read is still accessible.
 * 
 * @return status
 */
KSStatus KSCharacterInputStream::close(void) {
  return _istream->close();
}

