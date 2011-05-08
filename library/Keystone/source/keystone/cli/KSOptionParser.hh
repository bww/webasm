// 
// $Id: KSOptionParser.hh 69 2010-01-28 00:34:07Z brian $
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

#if !defined(__KEYSTONE_KSOPTIONPARSER__)
#define __KEYSTONE_KSOPTIONPARSER__ 1

#include <CoreFoundation/CoreFoundation.h>

#include "KSObject.hh"
#include "KSTypes.hh"
#include "KSStatus.hh"
#include "KSError.hh"

#define KEYSTONE_CLASS_KSOptionParser "KSOptionParser"

typedef enum KSOptionParserArgumentType_ {
  kKSOptionNoArgument         = 0,
  kKSOptionArgumentRequired   = 1,
  kKSOptionArgumentOptional   = 2
} KSOptionParserArgumentType;

typedef struct KSOptionParserOption_ {
  const char *                name;
  KSFourCharCode              id;
  KSOptionParserArgumentType  argument;
  const char *                dvalue;
} KSOptionParserOption;

typedef KSOptionParserOption * KSOptionParserOptionRef;
typedef KSOptionParserOption * KSOptionParserOptionSet;

const int kKSOptionParserEndOptions = 0;

/**
 * An option parser.
 * 
 * @author Brian William Wolter
 */
class KSOptionParser : public KSObject {
private:
  KSOptionParserOptionSet _options;
  int                     _argc;
  const char **           _argv;
  int                     _findex;
  const char *            _argument;
  KSOptionParserOptionRef _current;
  
  KSStatus getNextFlag(const char *v, KSOptionParserOptionRef *option, KSError **error = NULL);
  bool isOptionFlag(const char *v, KSOptionParserOptionRef *option = NULL);
  
public:
  KSOptionParser(KSOptionParserOptionSet options, int argc, char * const argv[]);
  virtual ~KSOptionParser();
  
  virtual unsigned long getTypeID() const { return KSOptionParserTypeID; }
  
  int getIndex(void) const;
  const char * getArgument(void) const;
  KSStatus getNextOption(KSFourCharCode *id, KSError **error = NULL);
  
};

#endif //__KEYSTONE_KSOPTIONPARSER__

