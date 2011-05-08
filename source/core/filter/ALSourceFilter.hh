// 
// $Id: ALSourceFilter.hh 160 2010-12-06 22:07:33Z brian $
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

#if !defined(__ALSOURCEFILTER__)
#define __ALSOURCEFILTER__ 1

#include <CoreFoundation/CoreFoundation.h>

#include <Keystone/KSObject.hh>
#include <Keystone/KSOutputStream.hh>
#include <Keystone/KSInputStream.hh>
#include <Keystone/KSStatus.hh>

#include "ALOptionsModel.hh"
#include "ALDirective.hh"

class ALSourceFilter : public KSObject {
private:
  const ALOptionsModel        *_options;
  const ALSourceFilter        *_parentFilter;
  const ALDirective           *_parentDirective;
  const ALSourceFilterContext *_context;
  
protected:
  ALSourceFilter(const ALOptionsModel *model, const ALSourceFilter *parentFilter, const ALDirective *parentDirective, const ALSourceFilterContext *context);
  
public:
  virtual ~ALSourceFilter();
  
  static ALSourceFilter * createFilterForFileType(CFStringRef type);
  
  const ALOptionsModel * getOptionsModel(void) const;
  const ALSourceFilter * getParentFilter(void) const;
  const ALDirective * getParentDirective(void) const;
  const ALSourceFilterContext * getContextInfo(void) const;
  
  virtual KSStatus filter(KSInputStream *ins, KSOutputStream *outs) const = 0;
  virtual KSStatus filter(CFStringRef fulltext, KSOutputStream *outs) const = 0;
  
};

#endif __ALSOURCEFILTER__

