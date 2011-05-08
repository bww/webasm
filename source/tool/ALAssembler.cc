// 
// $Id: ALAssembler.cc 159 2010-12-06 14:49:05Z brian $
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

#include <CoreFoundation/CoreFoundation.h>

#include <Keystone/KSLog.hh>
#include <Keystone/KSObject.hh>
#include <Keystone/KSStatus.hh>
#include <Keystone/KSUtility.hh>
#include <Keystone/KSError.hh>
#include <Keystone/KSOptionParser.hh>
#include <Keystone/CFStringAdditions.hh>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/param.h>

#include <AssemblerCore/ALOptionsModel.hh>
#include "ALCommand.hh"
#include "ALHelpCommand.hh"
#include "ALBuildCommand.hh"
#include "ALPlanCommand.hh"
#include "ALParseCommand.hh"
#include "ALPushCommand.hh"

int ALLoadOptions(ALOptionsModel *options, int argc, char * const argv[]);
void ALDisplayUsage(FILE *stream);

/**
 * Main
 */
int main(int argc, char * const argv[]) {
  
  if(argc < 2){
    ALDisplayUsage(stdout);
    return -1;
  }
  
  int count = ALLoadOptions((ALOptionsModel *)ALOptionsModel::getShared(), argc -= 1, argv += 1);
  argv += count;
  argc -= count;
  
  if(argc < 1){
    ALDisplayUsage(stdout);
    return -1;
  }
  
  const char *command = *argv++;
  argc--;
  
  ALCommand *instance = NULL;
  
  if(strcasecmp(command, "build") == 0 || strcasecmp(command, "mk") == 0){
    instance = new ALBuildCommand();
  }else if(strcasecmp(command, "plan") == 0 || strcasecmp(command, "pl") == 0){
    instance = new ALPlanCommand();
  }else if(strcasecmp(command, "parse") == 0 || strcasecmp(command, "wa") == 0){
    instance = new ALParseCommand();
  }else if(strcasecmp(command, "push") == 0 || strcasecmp(command, "pu") == 0){
    instance = new ALPushCommand();
  }else{
    ALDisplayUsage(stdout);
    return -1;
  }
  
  // invoke the command
  if(instance){
    return instance->execute(argc, argv);
  }else{
    return 0;
  }
  
}

/**
 * Load command line options
 */
int ALLoadOptions(ALOptionsModel *options, int argc, char * const argv[]) {
  KSStatus status = KSStatusOk;
  CFStringRef rtbase = NULL;
  CFStringRef sarg = NULL;
  CFArrayRef aarg = NULL;
  int acount = 0;
  
  // runtime base
  char *crbase = getcwd(NULL, MAXPATHLEN);
  rtbase = CFStringCreateWithCString(NULL, crbase, kDefaultStringEncoding);
  options->setRuntimeBasePath(rtbase);
  if(crbase) free(crbase);
  
  // set default paths
  options->setProjectBasePath(rtbase);
  
  sarg = CFStringCreateCanonicalPath(rtbase, CFSTR("webasm"));
  options->setOutputBasePath(sarg);
  CFReleaseSafe(sarg);
  
  // set default file types
  CFArrayRef types = CFStringCreateArrayFromDelimitedList(CFSTR("html, htm"), CFSTR(","));
  options->setProcessFileTypes(types);
  CFRelease(types);
  
  // load properties from the environment
  options->loadPropertiesFromEnvironment();
  
  // load default configuration
  CFURLRef defaultConfigURL;
  if((defaultConfigURL = CFBundleCopyResourceURL(CFBundleGetBundleWithIdentifier(kAssemblerCoreBundleID), CFSTR("webasm-default"), CFSTR("xml"), NULL)) != NULL){
    CFStringRef defaultConfigPath = CFURLCopyPath(defaultConfigURL);
    if(options->loadConfigurationFromXML(defaultConfigPath) != KSStatusOk){
      KSLogError("Unable to load default configuration: %@", defaultConfigPath);
      exit(0);
    }
    CFRelease(defaultConfigPath);
    CFRelease(defaultConfigURL);
  }else{
    KSLogError("No default configuration present; this is weird, but ok...");
  }
  
  // command line property definitions
  CFMutableDictionaryRef dprops = CFDictionaryCreateMutable(NULL, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
  
  static KSOptionParserOption ksoptions[] = {
    { "config",           'conf',   kKSOptionArgumentRequired,  NULL },
    { "define",           'defn',   kKSOptionArgumentRequired,  NULL },
    { "source",           'srsc',   kKSOptionArgumentRequired,  NULL },
    { "output",           'outp',   kKSOptionArgumentRequired,  NULL },
    { "types",            'typs',   kKSOptionArgumentRequired,  NULL },
    { "match",            'matc',   kKSOptionArgumentRequired,  NULL },
    { "include-hidden",   'inch',   kKSOptionNoArgument,        NULL },
    { "collapse-white",   'clpw',   kKSOptionNoArgument,        NULL },
    { "force",            'forc',   kKSOptionNoArgument,        NULL },
    { "quiet",            'sssh',   kKSOptionNoArgument,        NULL },
    { "verbose",          'loud',   kKSOptionNoArgument,        NULL },
    { "debug",            'dbug',   kKSOptionNoArgument,        NULL },
    { "help",             'help',   kKSOptionNoArgument,        NULL },
    NULL
  };
  
  KSOptionParser *parser = new KSOptionParser(ksoptions, argc, argv);
  KSFourCharCode fid;
  KSError *error;
  
  while((status = parser->getNextOption(&fid, &error)) == KSStatusOk && fid != kKSOptionParserEndOptions){
    switch(fid){
      
      
      case 'conf':
        sarg = CFStringCreateWithCString(NULL, parser->getArgument(), kDefaultStringEncoding);
        options->setConfigPath(sarg);
        CFRelease(sarg);
        break;
        
      case 'defn':
        sarg = CFStringCreateWithCString(NULL, parser->getArgument(), kDefaultStringEncoding);
        aarg = CFStringCreateArrayFromDelimitedList(sarg, CFSTR("="));
        if(CFArrayGetCount(aarg) == 2){
          CFDictionarySetValue(dprops, (CFStringRef)CFArrayGetValueAtIndex(aarg, 0), (CFStringRef)CFArrayGetValueAtIndex(aarg, 1));
        }else{
          KSLogError("Properties must be declared as: -Dname=value; ignoring improperly formatted property: \"%@\"", sarg);
        }
        CFRelease(sarg);
        CFRelease(aarg);
        break;
        
      case 'srsc':
        sarg = CFStringCreateWithCString(NULL, parser->getArgument(), kDefaultStringEncoding);
        options->setProjectBasePath(sarg);
        CFRelease(sarg);
        break;
        
      case 'outp':
        sarg = CFStringCreateWithCString(NULL, parser->getArgument(), kDefaultStringEncoding);
        options->setOutputBasePath(sarg);
        CFRelease(sarg);
        break;
        
      case 'typs':
        sarg = CFStringCreateWithCString(NULL, parser->getArgument(), kDefaultStringEncoding);
        aarg = CFStringCreateArrayFromDelimitedList(sarg, CFSTR(","));
        options->setProcessFileTypes(aarg);
        CFRelease(sarg);
        CFRelease(aarg);
        break;
        
      case 'matc':
        sarg = CFStringCreateWithCString(NULL, parser->getArgument(), kDefaultStringEncoding);
        options->setProcessFilePattern(sarg);
        CFRelease(sarg);
        break;
        
      case 'inch':
        options->setFlag(kOptionIncludeHiddenFiles, true);
        break;
        
      case 'clpw':
        options->setFlag(kOptionCollapseWhitespace, true);
        break;
        
      case 'forc':
        options->setFlag(kOptionForce, true);
        break;
        
      case 'loud':
        options->setFlag(kOptionVerbose, true);
        options->setFlag(kOptionQuiet, false);
        break;
        
      case 'sssh':
        options->setFlag(kOptionQuiet, true);
        options->setFlag(kOptionVerbose, false);
        break;
        
      case 'dbug':
        options->setFlag(kOptionDebug, true);
        break;
        
      case 'help':
        ALHelpCommand::displayOverviewVerbose();
        exit(0);
        
      default:
        ALDisplayUsage(stdout);
        exit(0);
      
    }
  }
  
  if(status != KSStatusOk){
    KSLogK("Wah waaah: %K", error->getMessage());
    fputs("--\n", stderr);
    KSRelease(error);
    ALDisplayUsage(stdout);
    exit(-1);
  }
  
  CFStringRef configPath;
  if((configPath = options->getConfigPath()) != NULL){
    if(options->loadConfigurationFromXML(configPath) != KSStatusOk){
      KSLogError("Unable to load configuration: %@", configPath);
      exit(0);
    }
  }
  
  // assign command line properties after configuration (override)
  if(dprops != NULL){
    options->addProperties(dprops);
    CFRelease(dprops);
  }
  
  if(options->isVerbose()){
    
    // paths and such
    KSLog(" Project Base: %@", options->getProjectBasePath());
    KSLog("  Output Base: %@", options->getOutputBasePath());
    CFStringRef typelist = CFStringCreateDelimitedListFromArray(options->getProcessFileTypes(), CFSTR(", "));
    KSLog("Process Types: %@", typelist);
    CFRelease(typelist);
    
    // properties
    CFDictionaryRef properties = options->getProperties();
    CFIndex pcount = CFDictionaryGetCount(properties);
    const void **keys = (const void **)malloc(sizeof(CFTypeRef) * pcount);
    const void **vals = (const void **)malloc(sizeof(CFTypeRef) * pcount);
    CFDictionaryGetKeysAndValues(properties, keys, vals);
    
    register int i;
    for(i = 0; i < pcount; i++){
      KSLog(" Set Property: \"%@\" = \"%@\"", (CFStringRef)keys[i], (CFStringRef)vals[i]);
    }
    
    if(keys) free(keys);
    if(vals) free(vals);
  }
  
  acount = parser->getIndex();
  
  KSRelease(parser);
  CFReleaseSafe(rtbase);
  
  return acount;
}

/**
 * Display useage information
 * 
 * @param stream file stream to print to
 */
void ALDisplayUsage(FILE *stream) {
  ALHelpCommand::displayOverviewBrief();
}

