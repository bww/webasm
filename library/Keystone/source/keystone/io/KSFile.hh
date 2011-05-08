// 
// $Id: KSFile.hh 80 2010-07-24 22:08:26Z brian $
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

#if !defined(__KEYSTONE_KSFILE__)
#define __KEYSTONE_KSFILE__ 1

#include <CoreFoundation/CoreFoundation.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/dir.h>

#include "KSObject.hh"
#include "KSTypes.hh"
#include "KSStatus.hh"
#include "KSInputStream.hh"
#include "KSOutputStream.hh"

#define KEYSTONE_CLASS_KSFile "KSFile"

/**
 * Represents a filesystem object.
 * 
 * @author BWW
 */
class KSFile : public KSObject {
private:
  CFStringRef           _path;
  mutable struct stat   *_sb;
  
  struct stat * getFileStatus(KSStatus *status = NULL) const;
  
public:
  KSFile(CFStringRef path);
  KSFile(CFURLRef url);
  virtual ~KSFile();
  
  static CFStringRef copyCurrentWorkingDirectory(void);
  static CFStringRef copyFileSystemPathForPath(CFStringRef path);
  static CFStringRef copyVolumeNameForPath(CFStringRef path);
  
  virtual unsigned long getTypeID() const { return KSFileTypeID; }
  
  CFStringRef getPath(void) const;
  bool exists(void) const;
  bool isDirectory(void) const;
  bool isAbsolute(void) const;
  KSLength getLength(void) const;
  
  CFStringRef copyFileSystemPath(void) const;
  CFStringRef copyVolumeName(void) const;
  
  KSStatus unlink(void) const;
  KSStatus truncate(KSLength length = 0) const;
  
  KSStatus makeDirectories(bool isdir = false) const;
  
  KSInputStream * createInputStream(KSStatus *status = NULL) const;
  KSOutputStream * createOutputStream(KSStatus *status = NULL) const;
  
  virtual CFStringRef copyDescription(void) const;
  
};

#endif __KEYSTONE_KSFILE__


