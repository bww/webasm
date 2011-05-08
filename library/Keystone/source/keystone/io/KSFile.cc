// 
// $Id: KSFile.cc 80 2010-07-24 22:08:26Z brian $
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

#include "KSFile.hh"
#include "KSLog.hh"
#include "KSUtility.hh"
#include "CFStringAdditions.hh"
#include "KSFileInputStream.hh"
#include "KSFileOutputStream.hh"

#include <DiskArbitration/DiskArbitration.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/mount.h>

/**
 * Construct
 * 
 * @param path file path
 */
KSFile::KSFile(CFStringRef path) {
  _path = (path != NULL) ? (CFStringRef)CFRetain(path) : NULL;
  _sb = NULL;
}

/**
 * Construct
 * 
 * @param url file URL
 */
KSFile::KSFile(CFURLRef url) {
  _path = (url != NULL) ? CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle) : NULL;
  _sb = NULL;
}

/**
 * Clean up
 */
KSFile::~KSFile() {
  if(_path) CFRelease(_path);
  if(_sb) free(_sb);
}

/**
 * Copy the current working directory.
 * 
 * @return current working directory
 */
CFStringRef KSFile::copyCurrentWorkingDirectory(void) {
  char cwd[MAXPATHLEN];
  getcwd(cwd, MAXPATHLEN - 1);
  return CFStringCreateWithCString(NULL, cwd, kDefaultStringEncoding);
}

/**
 * Copy the BSD filesystem path for a given path.
 * 
 * @param path a path
 * @return filesystem path (probably a device)
 */
CFStringRef KSFile::copyFileSystemPathForPath(CFStringRef path) {
  struct statfs sfsb;
  CFStringRef result = NULL;
  char *cpath = NULL;
  
  if((cpath = CFStringCopyCString(path, kDefaultStringEncoding)) == NULL){
    KSLogError("Unable to copy path: %@", path);
    goto error;
  }
  
  if(statfs(cpath, &sfsb) < 0){
    KSLogError("Unable to stat filesystem for: %s", cpath);
    goto error;
  }
  
  result = CFStringCreateWithCString(NULL, sfsb.f_mntfromname, kDefaultStringEncoding);
  
error:
  if(cpath) free(cpath);
  
  return result;
}

/**
 * Copy the OS X volume path for a given path.
 * 
 * @param path a path
 * @return volume path
 */
CFStringRef KSFile::copyVolumeNameForPath(CFStringRef path) {
  DASessionRef session = NULL;
  DADiskRef disk = NULL;
  CFDictionaryRef dd = NULL;
  CFStringRef mount = NULL;
  char *cmount = NULL;
  CFStringRef volume = NULL;
  CFStringRef result = NULL;
  
  if((mount = copyFileSystemPathForPath(path)) == NULL){
    KSLogError("Unable to copy filesystem path");
    goto error;
  }
  
  if((cmount = CFStringCopyCString(mount, kDefaultStringEncoding)) == NULL){
    KSLogError("Unable to copy mount path");
    goto error;
  }
  
  if((session = DASessionCreate(NULL)) == NULL){
    KSLogError("Unable to create disk arbitration session");
    goto error;
  }
  
  if((disk = DADiskCreateFromBSDName(NULL, session, cmount)) == NULL){
    KSLogError("Unable to create disk");
    goto error;
  }
  
  if((dd = DADiskCopyDescription(disk)) == NULL){
    KSLogError("Unable to copy disk description");
    goto error;
  }
  
  if((volume = (CFStringRef)CFDictionaryGetValue(dd, kDADiskDescriptionVolumeNameKey)) != NULL){
    result = CFStringCreateCopy(NULL, volume);
  }
  
error:
  CFReleaseSafe(mount);
  if(cmount) free(cmount);
  CFReleaseSafe(dd);
  CFReleaseSafe(disk);
  CFReleaseSafe(session);
  
  return result;
}

/**
 * Check the file status
 * 
 * @param status on output: status
 * @return status
 */
struct stat * KSFile::getFileStatus(KSStatus *status) const {
  if(_sb != NULL) _sb;
  
  char *cpath;
  if((cpath = CFStringCopyCString(getPath(), kDefaultStringEncoding)) == NULL)
    return NULL;
  
  if((_sb = (struct stat *)malloc(sizeof(struct stat))) == NULL)
    goto error;
  
  if(stat(cpath, _sb) < 0){
    free(_sb);
    _sb = NULL;
    if(status) *status = KSStatusError;
    goto error;
  }
  
  if(status) *status = KSStatusOk;
  
error:
  if(cpath) free(cpath);
  
  return _sb;
}

/**
 * Obtain the file path
 * 
 * @return path
 */
CFStringRef KSFile::getPath(void) const {
  return _path;
}

/**
 * Determine if the file exists on disk or not.  The effect of this method
 * is actually more accurately put: "file exists and is accessible by current
 * user".
 * 
 * @return exists or not
 */
bool KSFile::exists(void) const {
  return (getFileStatus(NULL) != NULL);
}

/**
 * Determine if this file is actually a directory or not.
 * 
 * @return directory or not
 */
bool KSFile::isDirectory(void) const {
  
  struct stat *st;
  if((st = getFileStatus(NULL)) == NULL)
    return false;
  
  return (st->st_mode & S_IFDIR) == S_IFDIR;
}

/**
 * Determine if this file represents an absolute path or not.
 * 
 * @return path is absolute or not
 */
bool KSFile::isAbsolute(void) const {
  CFStringRef path;
  if((path = getPath()) != NULL && CFStringGetLength(path) > 0)
    return (CFStringGetCharacterAtIndex(path, 0) == kPathComponentDelimiter);
  else
    return false;
}

/**
 * Obtain the file length on disk, in bytes.  A length of 0 indicates an error.
 * 
 * @return file length
 */
KSLength KSFile::getLength(void) const {
  
  struct stat *st;
  if((st = getFileStatus(NULL)) == NULL)
    return 0;
  
  return st->st_size;
}

/**
 * Copy the BSD filesystem path for this file.
 * 
 * @return filesystem path
 */
CFStringRef KSFile::copyFileSystemPath(void) const {
  return copyFileSystemPathForPath(getPath());
}

/**
 * Copy the OS X volume name for this file.
 * 
 * @return volume name
 */
CFStringRef KSFile::copyVolumeName(void) const {
  return copyVolumeNameForPath(getPath());
}

/**
 * Unlink this file.
 * 
 * @return status
 */
KSStatus KSFile::unlink(void) const {
  KSStatus status = KSStatusOk;
  
  if(!exists()){
    status = KSStatusNoSuchResource;
    goto error;
  }
  
  char *cpath;
  if((cpath = CFStringCopyCString(getPath(), kDefaultStringEncoding)) == NULL){
    status = KSStatusError;
    goto error;
  }
  
  int z;
  if((z = ::unlink(cpath)) < 0){
    
    switch(errno){
      case EPERM:
      case EROFS:
      case EACCES:        status = KSStatusPermissionDenied;  break;
      case EBUSY:         status = KSStatusBusy;              break;
      case EFAULT:        status = KSStatusInvalidResource;   break;
      case EIO:           status = KSStatusIOError;           break;
      case ELOOP:         status = KSStatusInfiniteLoop;      break;
      case ENAMETOOLONG:  status = KSStatusPathTooLong;       break;
      case ENOENT:        status = KSStatusNoSuchResource;    break;
      case ENOTDIR:       status = KSStatusInvalidResource;   break;
      default:            status = KSStatusIOError;           break;
    }
    
    goto error;
  }
  
error:
  if(cpath) free(cpath);
  
  return status;
}

/**
 * Truncate this file.
 * 
 * @param length length to truncate to, in bytes
 * @return status
 */
KSStatus KSFile::truncate(KSLength length) const {
  KSStatus status = KSStatusOk;
  
  if(!exists()){
    status = KSStatusNoSuchResource;
    goto error;
  }
  
  char *cpath;
  if((cpath = CFStringCopyCString(getPath(), kDefaultStringEncoding)) == NULL){
    status = KSStatusError;
    goto error;
  }
  
  int z;
  if((z = ::truncate(cpath, length)) < 0){
    
    switch(errno){
      case EPERM:
      case EROFS:
      case EACCES:        status = KSStatusPermissionDenied;  break;
      case EBUSY:         status = KSStatusBusy;              break;
      case EFAULT:        status = KSStatusInvalidResource;   break;
      case EIO:           status = KSStatusIOError;           break;
      case ELOOP:         status = KSStatusInfiniteLoop;      break;
      case ENAMETOOLONG:  status = KSStatusPathTooLong;       break;
      case ENOENT:        status = KSStatusNoSuchResource;    break;
      case ENOTDIR:       status = KSStatusInvalidResource;   break;
      default:            status = KSStatusIOError;           break;
    }
    
    goto error;
  }
  
error:
  if(cpath) free(cpath);
  
  return status;
}

/**
 * Create directories along the path to this file if they do not already exist.
 * 
 * @param makelast should the last path component be treated as a directory
 * itself (default: false)
 * @return status
 */
KSStatus KSFile::makeDirectories(bool isdir) const {
  KSStatus status = KSStatusOk;
  CFStringRef cpath = (isAbsolute()) ? CFSTR("/") : NULL;
  CFArrayRef components = CFStringCreateArrayWithPathComponents(getPath());
  struct stat sb;
  
  register int i;
  for(i = 0; i < CFArrayGetCount(components) - ((isdir) ? 0 : 1); i++){
    CFStringRef part = (CFStringRef)CFArrayGetValueAtIndex(components, i);
    CFStringRef tpath = CFStringCreateCanonicalPath(cpath, part);
    if(cpath) CFRelease(cpath);
    cpath = tpath;
    
    char *vpath = CFStringCopyCString(cpath, kDefaultStringEncoding);
    if(stat(vpath, &sb) < 0){
      if(mkdir(vpath, 0777) < 0){
        status = KSStatusError;
        goto error;
      }
    }else if((sb.st_mode & S_IFDIR) != S_IFDIR){
      KSLogError("Path component exists and is not a directory: %s", vpath);
      status = KSStatusError;
      goto error;
    }
    
  }
  
error:
  if(components) CFRelease(components);
  if(cpath) CFRelease(cpath);
  
  return status;
}

/**
 * Create an input stream to this file.
 * 
 * @param status on output: status
 * @return input stream
 */
KSInputStream * KSFile::createInputStream(KSStatus *status) const {
  KSInputStream *stream = NULL;
  char *vpath = CFStringCopyCString(getPath(), kDefaultStringEncoding);
  int fd;
  
  if((fd = open(vpath, O_RDONLY)) < 0){
    if(status){
      switch(errno){
        case EACCES:        *status = KSStatusPermissionDenied; break;
        case EDQUOT:        *status = KSStatusQuotaExceeded;    break;
        case EEXIST:        *status = KSStatusResourceExists;   break;
        case EINTR:         *status = KSStatusInterrupted;      break;
        case EINVAL:        *status = KSStatusInvalidOperand;   break;
        case EISDIR:        *status = KSStatusInvalidResource;  break;
        case EIO:           *status = KSStatusIOError;          break;
        case ELOOP:         *status = KSStatusTooManyLinks;     break;
        case EMFILE:        *status = KSStatusTooManyOpenFiles; break;
        case ENFILE:        *status = KSStatusFileTableFull;    break;
        case ENAMETOOLONG:  *status = KSStatusPathTooLong;      break;
        case ENOENT:        *status = KSStatusNoSuchResource;   break;
        default:            *status = KSStatusError;            break;
      }
    }
    goto error;
  }
  
  if(status) *status = KSStatusOk;
  stream = new KSFileInputStream(fd);
  
error:
  if(vpath) free(vpath);
  
  return stream;
}

/**
 * Create an output stream to this file.
 * 
 * @param status on output: status
 * @return output stream
 */
KSOutputStream * KSFile::createOutputStream(KSStatus *status) const {
  KSOutputStream *stream = NULL;
  char *vpath = CFStringCopyCString(getPath(), kDefaultStringEncoding);
  int flags = O_WRONLY | O_CREAT | O_TRUNC;
  int fd;
  
  if((fd = open(vpath, flags, 0644)) < 0){
    if(status) *status = KSStatusError;
    goto error;
  }
  
  if(status) *status = KSStatusOk;
  stream = new KSFileOutputStream(fd);
  
error:
  if(vpath) free(vpath);
  
  return stream;
}

/**
 * Obtain a string description
 * 
 * @return description
 */
CFStringRef KSFile::copyDescription(void) const {
  return CFStringCreateCopy(NULL, _path);
}


