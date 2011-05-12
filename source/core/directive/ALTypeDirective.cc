// 
// $Id: ALTypeDirective.cc 160 2010-12-06 22:07:33Z brian $
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

#include "ALTypeDirective.hh"
#include "ALOptionsModel.hh"
#include "ALSourceFilter.hh"

#include <ApplicationServices/ApplicationServices.h>
#include <QuartzCore/QuartzCore.h>

#include <Keystone/KSLog.hh>
#include <Keystone/CFStringAdditions.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSInputStream.hh>
#include <Keystone/KSDataBufferOutputStream.hh>

#define super ALDirectiveList

#define kALTypeDirectiveDefaultFontName   CFSTR("Helvetica");
#define kALTypeDirectiveDefaultFontSize   32.0

const char kALDirectiveHexCharsLower[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
const char kALDirectiveHexCharsUpper[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

const char *kALTypeDirectiveAttributes[] = {
  "width",
  "height",
  "font-name",
  "font-size",
  "foreground",
  "background",
  "padding",
  NULL
};

/**
 * Determine if an attribute is reserved
 * 
 * @param name attribute name
 * @return reserved or not
 */
bool ALTypeDirectiveIsReservedAttributeName(CFStringRef name) {
  char *cname = CFStringCopyCString(name, kDefaultStringEncoding);
  bool result = false;
  
  register int i;
  for(i = 0; kALTypeDirectiveAttributes[i] != NULL; i++){
    if(strcasecmp(kALTypeDirectiveAttributes[i], cname) == 0){
      result = true;
      goto done;
    }
  }
  
done:
  if(cname) free(cname);
  
  return result;
}

/**
 * Decode a color value specified in hex notation ("#rrggbb" or "rrggbb").
 * 
 * @param color color string
 * @return color value
 */
int32_t ALTypeDirectiveGetColorValue(CFStringRef color) {
  int32_t cvalue = 0;
  
  CFStringRef actual;
  int offset = 0;
  register int i;
  
  if(color == NULL)
    return cvalue;
  
  for(i = 0; i < CFStringGetLength(color) && CFStringGetCharacterAtIndex(color, i) == '#'; i++){
    offset = i + 1;
  }
  
  actual = CFStringCreateWithSubstring(NULL, color, CFRangeMake(offset, CFStringGetLength(color) - offset));
  
  int count;
  if((count = CFStringGetLength(actual)) < 6)
    return cvalue;
  
  for(i = 0; i < count; i++){
    UniChar c = CFStringGetCharacterAtIndex(actual, i);
    
    register int j;
    for(j = 0; j < 16; j++){
      if(c == kALDirectiveHexCharsLower[j] || c == kALDirectiveHexCharsUpper[j]){
        cvalue |= (j << ((count - (i + 1)) * 4));
      }
    }
    
  }
  
  if(actual) CFRelease(actual);
  
  return cvalue;
}

/**
 * Create a directional value array from the provided specifier.  Directional
 * values are up to 4 floats which correspond to top, right, bottom, and left
 * directions.
 * 
 * When 2 values are provided, they are interpreted as vertical and horizontal
 * (i.e., 1 2 1 2); when 1 value is provided it is interpreted as applying to
 * all directions (i.e., 1 1 1 1).
 * 
 * If 3 values are provided the third value is discarded and it is interpreted
 * as 2 values.  More than 4 values is an error.
 * 
 * @param value the value specifier
 * @return directional values
 */
CFArrayRef ALTypeDirectiveCreateDirectionalValueArray(CFStringRef value) {
  CFCharacterSetRef delimiter = CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline);
  CFMutableArrayRef padding = NULL;
  
  CFArrayRef parts;
  if((parts = CFStringCreateArrayFromComponentList(value, delimiter, NULL)) != NULL){
    padding = CFArrayCreateMutable(NULL, 0, &kCFTypeArrayCallBacks);
    
    CFIndex pvcount = 4;
    CGFloat pvalues[pvcount];
    
    bzero(pvalues, sizeof(CGFloat) * pvcount);
    
    switch(CFArrayGetCount(parts)){
      case 4:
        pvalues[0] = CFStringGetDoubleValue((CFStringRef)CFArrayGetValueAtIndex(parts, 0));
        pvalues[1] = CFStringGetDoubleValue((CFStringRef)CFArrayGetValueAtIndex(parts, 1));
        pvalues[2] = CFStringGetDoubleValue((CFStringRef)CFArrayGetValueAtIndex(parts, 2));
        pvalues[3] = CFStringGetDoubleValue((CFStringRef)CFArrayGetValueAtIndex(parts, 3));
        break;
      case 3:
      case 2:
        pvalues[0] = CFStringGetDoubleValue((CFStringRef)CFArrayGetValueAtIndex(parts, 0));
        pvalues[2] = pvalues[0];
        pvalues[1] = CFStringGetDoubleValue((CFStringRef)CFArrayGetValueAtIndex(parts, 1));
        pvalues[3] = pvalues[1];
        break;
      case 1:
        pvalues[0] = CFStringGetDoubleValue((CFStringRef)CFArrayGetValueAtIndex(parts, 0));
        pvalues[1] = pvalues[0];
        pvalues[2] = pvalues[0];
        pvalues[3] = pvalues[0];
        break;
      default:
        break;
    }
    
    register int i;
    for(i = 0; i < pvcount; i++){
      CFNumberRef number = CFNumberCreate(NULL, kCFNumberFloat32Type, (pvalues + i));
      CFArrayAppendValue(padding, number);
      CFRelease(number);
    }
    
    CFRelease(parts);
  }
  
  return padding;
}

/**
 * Internal constructor
 * 
 * @param parent parent directive
 * @param properties properties
 * @param path path (under the output root) at which the generated image should
 * be written.
 * @param width generated image width.  When @p width is less than 1, the image is
 * rendered with one line of type and the width is fit to the content.
 * @param height generated image height.  When @p height is less than 1, the image
 * is fit to the content.  @p height has no effect when @p is less than 1.
 * @param width generated image height
 * @param attributes type layout attributes
 * @param passthrough attributes to pass through to the generated img tag
 */
ALTypeDirective::ALTypeDirective(const ALDirective *parent, CFDictionaryRef properties, CFStringRef path, CFIndex width, CFIndex height, CFDictionaryRef attributes, CFDictionaryRef passthrough) : super(parent, properties) {
  _path = (path != NULL) ? (CFStringRef)CFRetain(path) : NULL;
  _width = width;
  _height = height;
  _attributes = (attributes != NULL) ? (CFDictionaryRef)CFRetain(attributes) : NULL;
  _passthrough = (passthrough != NULL) ? (CFDictionaryRef)CFRetain(passthrough) : NULL;
}

/**
 * Clean up
 */
ALTypeDirective::~ALTypeDirective() {
  if(_path)         CFRelease(_path);
  if(_attributes)   CFRelease(_attributes);
  if(_passthrough)  CFRelease(_passthrough);
}

/**
 * Create from WAML
 * 
 * @param options options model
 * @param parent parent directive
 * @param properties properties
 * @param el root element
 * @param status on output: status
 */
ALTypeDirective * ALTypeDirective::createFromWAML(const ALOptionsModel *options, const ALDirective *parent, CFDictionaryRef properties, ALElement *el, KSStatus *status) {
  KSStatus vstatus = KSStatusOk;
  ALTypeDirective *directive = NULL;
  CFMutableDictionaryRef attributes  = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFMutableDictionaryRef passthrough = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFDictionaryRef declared = NULL;
  CFStringRef path = NULL;
  CFIndex width  = 0;
  CFIndex height = 0;
  CFStringRef value;
  
  if((value = el->getAttribute(CFSTR("path"))) != NULL && CFStringGetLength(value) > 0){
    path = value;
  }else{
    KSLogError("Type directive must define a path attribute");
    vstatus = KSStatusError;
    goto error;
  }
  
  if((value = el->getAttribute(CFSTR("width"))) != NULL && CFStringGetLength(value) > 0){
    width = CFStringGetIntValue(value);
  }
  
  if((value = el->getAttribute(CFSTR("height"))) != NULL && CFStringGetLength(value) > 0){
    height = CFStringGetIntValue(value);
  }
  
  if((value = el->getAttribute(CFSTR("font-name"))) != NULL && CFStringGetLength(value) > 0){
    CFDictionarySetValue(attributes, kALTypeAttributeFontName, value);
  }
  
  if((value = el->getAttribute(CFSTR("font-size"))) != NULL && CFStringGetLength(value) > 0){
    float fsize = (float)CFStringGetDoubleValue(value);
    CFNumberRef size = CFNumberCreate(NULL, kCFNumberFloatType, &fsize);
    CFDictionarySetValue(attributes, kALTypeAttributeFontSize, size);
    if(size) CFRelease(size);
  }
  
  if((value = el->getAttribute(CFSTR("foreground"))) != NULL && CFStringGetLength(value) > 0){
    int32_t cvalue = ALTypeDirectiveGetColorValue(value);
    CFNumberRef color = CFNumberCreate(NULL, kCFNumberSInt32Type, &cvalue);
    CFDictionarySetValue(attributes, kALTypeAttributeForegroundColor, color);
    if(color)  CFRelease(color);
  }
  
  if((value = el->getAttribute(CFSTR("background"))) != NULL && CFStringGetLength(value) > 0){
    int32_t cvalue = ALTypeDirectiveGetColorValue(value);
    CFNumberRef color = CFNumberCreate(NULL, kCFNumberSInt32Type, &cvalue);
    CFDictionarySetValue(attributes, kALTypeAttributeBackgroundColor, color);
    if(color)  CFRelease(color);
  }
  
  if((value = el->getAttribute(CFSTR("padding"))) != NULL && CFStringGetLength(value) > 0){
    CFArrayRef pvalues;
    if((pvalues = ALTypeDirectiveCreateDirectionalValueArray(value)) != NULL){
      CFDictionarySetValue(attributes, kALTypeAttributePadding, pvalues);
      CFRelease(pvalues);
    }
  }
  
  
  if((declared = el->getAttributes()) != NULL){
    const void **dnames = (const void **)malloc(sizeof(const void *) * CFDictionaryGetCount(declared));
    
    CFDictionaryGetKeysAndValues(declared, dnames, NULL);
    
    register int i;
    for(i = 0; i < CFDictionaryGetCount(declared); i++){
      CFStringRef dname = (CFStringRef)dnames[i];
      if(!ALTypeDirectiveIsReservedAttributeName(dname)){
        CFDictionarySetValue(passthrough, dname, el->getAttribute(dname));
      }
    }
    
    if(dnames) free(dnames);
  }
  
  directive = new ALTypeDirective(parent, properties, path, width, height, attributes, passthrough);
  if((vstatus = directive->processWAMLChildren(options, el)) != KSStatusOk){
    KSRelease(directive); directive = NULL;
    goto error;
  }
  
error:
  if(status) *status = vstatus;
  if(attributes) CFRelease(attributes);
  if(passthrough) CFRelease(passthrough);
  
  return directive;
}

/**
 * Obtain this directive's name
 * 
 * @return name
 */
CFStringRef ALTypeDirective::getName(void) const {
  return CFSTR("Type Directive");
}

/**
 * Obtain the generated image output path, relative to the project output base.
 * 
 * @return output path
 */
CFStringRef ALTypeDirective::getImagePath(void) const {
  return _path;
}

/**
 * Obtain the generated image width
 * 
 * @return image width
 */
CFIndex ALTypeDirective::getImageWidth(void) const {
  return _width;
}

/**
 * Obtain the generated image height
 * 
 * @return image height
 */
CFIndex ALTypeDirective::getImageHeight(void) const {
  return _height;
}

/**
 * Obtain typesetting attributes
 * 
 * @return typesetting attributes
 */
CFDictionaryRef ALTypeDirective::getAttributes(void) const {
  return _attributes;
}

/**
 * Obtain passthrough attributes
 * 
 * @return passthrough attributes
 */
CFDictionaryRef ALTypeDirective::getPassthroughAttributes(void) const {
  return _passthrough;
}

/**
 * Emit the result of this directive to the specified output stream.
 * 
 * @param filter invoking filter
 * @param outs output stream
 * @param jsContext javascript context
 * @param context context info
 */
KSStatus ALTypeDirective::emit(const ALSourceFilter *filter, KSOutputStream *outs, JSContextRef jsContext, const ALSourceFilterContext *context) const {
  KSStatus status = KSStatusOk;
  KSDataBufferOutputStream *ostream = NULL;
  CFMutableDataRef buffer = NULL;
  CFStringRef fulltext = NULL;
  CFStringRef enctext = NULL;
  CFNumberRef number;
  
  CFIndex width = getImageWidth();
  CFIndex height = getImageHeight();
  CGFloat leading = 0;
  CFIndex padding = 0;
  CFDictionaryRef passthrough = getPassthroughAttributes();
  CFDictionaryRef attributes = getAttributes();
  CFMutableDictionaryRef ctattributes = NULL;
  CFStringRef gentag = NULL;
  CFMutableStringRef dattrs = NULL;
  
  CGColorRef foreground = NULL;
  CGColorRef background = NULL;
  
  size_t bytesPerPixel = 4;
  size_t bitsPerComponent = 8;
  CGColorSpaceRef colorspace = NULL;
  CGContextRef graphics = NULL;
  CGImageRef image = NULL;
  CGImageDestinationRef destination = NULL;
  
  KSFile *ofile = NULL;
  CFURLRef destURL = NULL;
  CFStringRef cbase = NULL;
  CFStringRef zpath = NULL;
  CFStringRef ipath = NULL;
  char *cdurls = NULL;
  
  CFAttributedStringRef content = NULL;
  CTFontRef font = NULL;
  CTFramesetterRef framesetter = NULL;
  CTFrameRef frame = NULL;
  CGMutablePathRef textpath = NULL;
  
  JSObjectRef peer = NULL;
  
  const ALOptionsModel *options;
  if((options = filter->getOptionsModel()) == NULL){
    options = gSharedOptions;
  }
  
  CFStringRef fontName;
  if((fontName = ((attributes != NULL) ? (CFStringRef)CFDictionaryGetValue(attributes, kALTypeAttributeFontName) : NULL)) == NULL){
    fontName = kALTypeDirectiveDefaultFontName;
  }
  
  float fontSize;
  if((number = ((attributes != NULL) ? (CFNumberRef)CFDictionaryGetValue(attributes, kALTypeAttributeFontSize) : NULL)) != NULL){
    CFNumberGetValue(number, kCFNumberFloatType, &fontSize);
  }else{
    fontSize = kALTypeDirectiveDefaultFontSize;
  }
  
  CFStringRef dpath;
  if((dpath = context->getDestinationPath()) == NULL){
    KSLogError("No destination path");
    return KSStatusError;
  }
  
  CFStringRef bpath;
  if((bpath = context->getSourceBasePath()) == NULL){
    KSLogError("No base path");
    return KSStatusError;
  }
  
  if((ipath = getImagePath()) == NULL){
    KSLogError("Output image path is null");
    status = KSStatusError;
    goto error;
  }
  
  if((status = pushJSContext(jsContext, &peer, filter, outs, context)) != KSStatusOk){
    goto error;
  }
  
  buffer = CFDataCreateMutable(NULL, 0);
  ostream = new KSDataBufferOutputStream(buffer);
  
  if((status = super::emit(filter, ostream, jsContext, context)) != KSStatusOk){
    KSLogError("Unable to write to child output stream: %@", KSStatusGetMessage(status));
    goto error;
  }
  
  if((fulltext = CFStringCreateWithBytes(NULL, CFDataGetBytePtr(buffer), CFDataGetLength(buffer), kDefaultStringEncoding, false)) == NULL){
    KSLogError("Unable to create full text from buffer");
    status = KSStatusError;
    goto error;
  }
  
  KSRelease(ostream); ostream = NULL;
  CFRelease(buffer); buffer = NULL;
  
  if((colorspace = CGColorSpaceCreateDeviceRGB()) == NULL){
    KSLogError("Unable to create colorspace");
    status = KSStatusError;
    goto error;
  }
  
  if((number = ((attributes != NULL) ? (CFNumberRef)CFDictionaryGetValue(attributes, kALTypeAttributeForegroundColor) : NULL)) != NULL){
    int32_t vcolor;
    CFNumberGetValue(number, kCFNumberSInt32Type, &vcolor);
    CGFloat parts[] = { (CGFloat)((vcolor >> 16) & 0xff) / (CGFloat)0xff, (CGFloat)((vcolor >> 8) & 0xff) / (CGFloat)0xff, (CGFloat)((vcolor >> 0) & 0xff) / (CGFloat)0xff, 1 };
    foreground = CGColorCreate(colorspace, parts);
  }
  
  if((number = ((attributes != NULL) ? (CFNumberRef)CFDictionaryGetValue(attributes, kALTypeAttributeBackgroundColor) : NULL)) != NULL){
    int32_t vcolor;
    CFNumberGetValue(number, kCFNumberSInt32Type, &vcolor);
    CGFloat parts[] = { (CGFloat)((vcolor >> 16) & 0xff) / (CGFloat)0xff, (CGFloat)((vcolor >> 8) & 0xff) / (CGFloat)0xff, (CGFloat)((vcolor >> 0) & 0xff) / (CGFloat)0xff, 1 };
    background = CGColorCreate(colorspace, parts);
  }
  
  if((font = CTFontCreateWithName(fontName, fontSize, NULL)) == NULL){
    KSLogError("Unable to create font named: %@", fontName);
    status = KSStatusInvalidParameter;
    goto error;
  }
  
  ctattributes = CFDictionaryCreateMutable(NULL, 0, &kCFCopyStringDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  CFDictionarySetValue(ctattributes, kCTFontAttributeName, font);
  
  if(foreground != NULL){
    CFDictionarySetValue(ctattributes, kCTForegroundColorAttributeName, foreground);
  }
  
  if((content = CFAttributedStringCreate(NULL, fulltext, ctattributes)) == NULL){
    KSLogError("Unable to create attributed string: %@", fulltext);
    status = KSStatusError;
    goto error;
  }
  
  CFRelease(ctattributes); ctattributes = NULL;
  
  if(width < 1){
    CTLineRef line;
    
    CGFloat ascent;
    CGFloat descent;
    double  lwidth;
    
    if((line = CTLineCreateWithAttributedString(content)) == NULL){
      KSLogError("Unable to create setter line for: %@", fulltext);
      status = KSStatusError;
      goto error;
    }
    
    if((lwidth = CTLineGetTypographicBounds(line, &ascent, &descent, &leading)) < 1){
      KSLogError("Typographical bounds make no sense (%@ %.2fpt for %d characters): width: %.2f, ascent: %.2f, descent: %.2f, leading: %.2f", fontName, fontSize, CFAttributedStringGetLength(content), lwidth, ascent, descent, leading);
      status = KSStatusError;
      goto error;
    }
    
    width  = lwidth + (padding * 2) + (fontSize / 2);
    height = ascent + descent + (padding * 2) + (fontSize / 2);
    
    if(options->isDebugging()){
      KSLog("Text typographical bounds: width: %.2f, height: %.2f, ascent: %.2f, descent: %.2f, leading: %.2f", lwidth, ascent + descent, ascent, descent, leading);
    }
    
    if(line) CFRelease(line);
  }else if(height < 1){
    height = fontSize;
  }
  
  if(options->isDebugging()){
    CFStringRef condensed = CFStringCreateCondensedCopy(fulltext, CFStringCondensationOptionNone, 0);
    KSLog("Emitting directive: set text (%d x %d) %@ %.1f pt \"%@\"", width, height, fontName, fontSize, condensed);
    if(condensed) CFRelease(condensed);
  }
  
  graphics =
    CGBitmapContextCreate(
      NULL,
      width, height, bitsPerComponent, width * bytesPerPixel,
      colorspace,
      kCGImageAlphaPremultipliedLast
    );
  
  if(graphics == NULL){
    KSLogError("Unable to create drawing context");
    status = KSStatusError;
    goto error;
  }
  
  if(background != NULL){
    CGContextSetFillColorWithColor(graphics, background);
    CGContextFillRect(graphics, CGRectMake(0, 0, width, height));
  }
  
  if((framesetter = CTFramesetterCreateWithAttributedString(content)) == NULL){
    KSLogError("Unable to create text framesetter");
    status = KSStatusError;
    goto error;
  }
  
  textpath = CGPathCreateMutable();
  CGPathAddRect(textpath, NULL, CGRectMake(0, padding - leading - 1, width - (padding * 2), height + leading - (padding * 2)));
  
  if((frame = CTFramesetterCreateFrame(framesetter, CFRangeMake(0, 0), textpath, NULL)) == NULL){
    KSLogError("Unable to create text frame");
    status = KSStatusError;
    goto error;
  }
  
  CTFrameDraw(frame, graphics);
  
  CFRelease(textpath); textpath = NULL;
  
  if((image = CGBitmapContextCreateImage(graphics)) == NULL){
    KSLogError("Unable to create image");
    status = KSStatusError;
    goto error;
  }
  
  CFRelease(graphics); graphics = NULL;
  
  cbase = CFStringCopyPathDirectory(dpath);
  zpath = CFStringCreateCanonicalPath(cbase, ipath);
  cdurls = CFStringCopyCString(zpath, kDefaultStringEncoding);
  ofile = new KSFile(zpath);
  
  if((status = ofile->makeDirectories(false)) != KSStatusOk){
    KSLogError("Unable to create intermediate directories to: %@", dpath);
    goto error;
  }
  
  if((destURL = CFURLCreateFromFileSystemRepresentation(NULL, (UInt8 *)cdurls, strlen(cdurls), false)) == NULL){
    KSLogError("Unable to create output destination URL: %@", dpath);
    status = KSStatusError;
    goto error;
  }
  
  if((destination = CGImageDestinationCreateWithURL(destURL, CFSTR("public.png"), 1, NULL)) == NULL){
    KSLogError("Unable to create image output destination");
    status = KSStatusError;
    goto error;
  }
  
  CGImageDestinationAddImage(destination, image, NULL);
  
  if(CGImageDestinationFinalize(destination) == false){
    KSLogError("Unable to write image");
    status = KSStatusError;
    goto error;
  }
  
  if((enctext = CFURLCreateStringByAddingPercentEscapes(NULL, fulltext, CFSTR(" "), NULL, kDefaultStringEncoding)) == NULL){
    KSLogError("Unable to encode text: %@", fulltext);
    status = KSStatusError;
    goto error;
  }
  
  if(fulltext) CFRelease(fulltext); fulltext = NULL;
  
  if(passthrough != NULL){
    const void **dnames  = (const void **)malloc(sizeof(const void *) * CFDictionaryGetCount(passthrough));
    const void **dvalues = (const void **)malloc(sizeof(const void *) * CFDictionaryGetCount(passthrough));
    
    CFDictionaryGetKeysAndValues(passthrough, dnames, dvalues);
    dattrs = CFStringCreateMutable(NULL, 0);
    
    register int i;
    for(i = 0; i < CFDictionaryGetCount(passthrough); i++){
      CFStringAppend(dattrs, CFSTR(" "));
      CFStringAppend(dattrs, (CFStringRef)dnames[i]);
      CFStringAppend(dattrs, CFSTR("=\""));
      CFStringAppend(dattrs, (CFStringRef)dvalues[i]);
      CFStringAppend(dattrs, CFSTR("\""));
    }
    
    if(dnames)  free(dnames);
    if(dvalues) free(dvalues);
  }
  
  gentag = CFStringCreateWithFormat(NULL, 0, CFSTR("<img src=\"%@\" width=\"%d\" height=\"%d\" alt=\"%@\"%@ />"), ipath, width, height, enctext, ((dattrs != NULL) ? dattrs : CFSTR("")));
  
  if((status = outs->writeCFString(gentag, CFRangeMake(0, CFStringGetLength(gentag)), NULL)) != KSStatusOk){
    KSLogError("Unable to write replacement image tag");
    goto error;
  }
  
error:
  if(peer != NULL){
    if((status = restoreJSContext(jsContext, peer)) != KSStatusOk){
      KSLogError("Unable to restore peer");
    }
  }
  
  if(buffer)        CFRelease(buffer);
  if(fulltext)      CFRelease(fulltext);
  if(enctext)       CFRelease(enctext);
  if(content)       CFRelease(content);
  if(framesetter)   CFRelease(framesetter);
  if(frame)         CFRelease(frame);
  if(textpath)      CFRelease(textpath);
  if(foreground)    CFRelease(foreground);
  if(background)    CFRelease(background);
  if(ctattributes)  CFRelease(ctattributes);
  if(colorspace)    CFRelease(colorspace);
  if(font)          CFRelease(font);
  if(destination)   CFRelease(destination);
  if(ofile)         KSRelease(ofile);
  if(destURL)       CFRelease(destURL);
  if(cbase)         CFRelease(cbase);
  if(zpath)         CFRelease(zpath);
  if(cdurls)        free(cdurls);
  if(image)         CFRelease(image);
  if(gentag)        CFRelease(gentag);
  if(dattrs)        CFRelease(dattrs);
  
  if(ostream){
    ostream->close();
    KSRelease(ostream);
  }
  
  return status;
}

