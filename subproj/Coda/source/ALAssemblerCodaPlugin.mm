// 
// $Id: ALAssemblerCodaPlugin.mm 135 2010-02-03 01:18:00Z brian $
// XHTML Assembler Coda Plugin
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

#import "ALAssemblerCodaPlugin.h"

#import <Keystone/KSUtility.hh>
#import <Keystone/KSFile.hh>
#import <Keystone/KSFileInputStream.hh>
#import <Keystone/KSDataBufferOutputStream.hh>
#import <Keystone/KSFileOutputStream.hh>
#import <Keystone/CFStringAdditions.hh>

#import <AssemblerCore/ALOptionsModel.hh>
#import <AssemblerCore/ALUtility.hh>
#import <AssemblerCore/ALDirective.hh>
#import <AssemblerCore/ALHTMLSourceFilter.hh>
#import <AssemblerCore/ALDirectorySourceCollection.hh>
#import <AssemblerCore/ALSourceFileFilter.hh>

#define kAssemblerConfigPrefix              @"WebasmConfig:"
#define kAssemblerConfigConfigPathKey       @"Configuration"
#define kAssemblerConfigPreviewModeKey      @"PreviewMode"

#define kAssemberlConfigPreviewModeInline   @"inline"
#define kAssemberlConfigPreviewModeBrowser  @"browser"

@interface ALAssemblerCodaPlugin (Private)

-(NSDictionary *)currentSiteConfig;
-(void)setCurrentSiteConfig:(NSDictionary *)config;
-(id)currentSiteConfigObjectForKey:(NSString *)key;
-(BOOL)setCurrentSiteConfigObject:(id)value forKey:(NSString *)key;

-(void)defineConfigurationPathPanelDidEnd:(NSOpenPanel *)panel returnCode:(int)rcode contextInfo:(void  *)info;

@end

@implementation ALAssemblerCodaPlugin

/**
 * Plugin initialize
 * 
 * @param controller plugin controller
 * @param bundle our bundle
 */
-(id)initWithPlugInController:(CodaPlugInsController *)controller bundle:(NSBundle *)bundle {
  
  if(!(self = [super init])){
    NSLog(@"Unable to initialize Web Assembler");
    return nil;
  }
  
  if(_controller) [_controller release];
  _controller = (controller != nil) ? [controller retain] : nil;
  
  [self.controller
    registerActionWithTitle:@"Set Web Assembler Configuration For Site..."
    target:self
    selector:@selector(defineConfigurationPathForSite:)
  ];
  /*
  [self.controller
    registerActionWithTitle:@"Preview in Browser"
    target:self
    selector:@selector(definePreviewMode:)
  ];
  */
  [self.controller
    registerActionWithTitle:@"Assemble All Site Files..."
    underSubmenuWithTitle:nil
    target:self
    selector:@selector(assembleAllFiles:)
    representedObject:nil
    keyEquivalent:@"$@k"
    pluginName:@"Assembler"
  ];
  
  [self.controller
    registerActionWithTitle:@"Assemble & Preview Current File..."
    underSubmenuWithTitle:nil
    target:self
    selector:@selector(assembleAndPreviewCurrentFile:)
    representedObject:nil
    keyEquivalent:@"$@j"
    pluginName:@"Assembler"
  ];
  
  NSLog(@"Init Web Assembler Plugin");
  return self;
}

/**
 * Plugin name
 * 
 * @return our name
 */
-(NSString *)name {
  return @"Web Assembler";
}

/**
 * Define the configuration path for the current site
 * 
 * @param sender event sender
 */
-(void)defineConfigurationPathForSite:(id)sender {
  NSLog(@"Define configuration path...");
  
  NSString *psite;
  if((psite = [self currentSitePath]) == nil){
    
    [[NSAlert
      alertWithMessageText:@"Cannot Define Web Assembler Configuration"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, you cannot define an Assembler configuration file unless you are currently working on a file that is part of a site."
      ] runModal];
    
    return;
  }
  
  NSOpenPanel *panel = [NSOpenPanel openPanel];
  [panel setCanChooseFiles:TRUE];
  [panel setCanChooseDirectories:FALSE];
  [panel setAllowsMultipleSelection:FALSE];
  
  [panel
    beginSheetForDirectory:psite
    file:[self currentSiteConfigurationPath]
    types:[NSArray arrayWithObject:@"xml"]
    modalForWindow:[[self.controller focusedTextView:self] window]
    modalDelegate:self
    didEndSelector:@selector(defineConfigurationPathPanelDidEnd:returnCode:contextInfo:)
    contextInfo:nil
  ];
  
}

/**
 * Panel did end
 */
-(void)defineConfigurationPathPanelDidEnd:(NSOpenPanel *)panel returnCode:(int)rcode contextInfo:(void  *)info {
  
  if(rcode != NSOKButton)
    return;
  
  NSArray *paths;
  if((paths = [panel filenames]) == nil || [paths count] < 1)
    return;
  
  [self setCurrentSiteConfigurationPath:[paths objectAtIndex:0]];
  
}

/**
 * Define the preview mode
 * 
 * @param sender event sender
 */
-(void)definePreviewMode:(id)sender {
  NSLog(@"Sender: %@", sender);
}

/**
 * Assemble all files
 * 
 * @param sender event sender
 */
-(void)assembleAllFiles:(id)sender {
  NSLog(@"Assemble Project...");
  
  CodaTextView *view;
  if((view = [self.controller focusedTextView:self]) == nil){
    
    [[NSAlert
      alertWithMessageText:@"Cannot Assemble Nothing"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, you can only assemble and preview a file from the source view."
      ] runModal];
    
    return;
  }
  
  NSString *config;
  if((config = [self currentSiteConfigurationPath]) == nil){
    BOOL inSite = [self currentSitePath] != nil;
    
    NSInteger result =
      [[NSAlert
        alertWithMessageText:@"Cannot Assemble Project"
        defaultButton:(inSite) ? @"Cancel" : @"Ok"
        alternateButton:nil
        otherButton:(inSite) ? @"Define Site Configuration" : nil
        informativeTextWithFormat:@"Sorry, you cannot assemble and preview a project until you have defined a configuration for the file's site."
        ] runModal];
    
    if(result == NSAlertOtherReturn){
      [self defineConfigurationPathForSite:sender];
    }
    
    return;
  }
  
  KSStatus status;
  ALOptionsModel *options = new ALOptionsModel();
  
  CFURLRef defaultConfigURL;
  if((defaultConfigURL = CFBundleCopyResourceURL(CFBundleGetBundleWithIdentifier(kAssemblerCoreBundleID), CFSTR("webasm-default"), CFSTR("xml"), NULL)) != NULL){
    CFStringRef defaultConfigPath = CFURLCopyPath(defaultConfigURL);
    if(options->loadConfigurationFromXML(defaultConfigPath) != KSStatusOk){
      
      [[NSAlert
        alertWithMessageText:@"Could Not Load Default Configuration"
        defaultButton:@"Ok"
        alternateButton:nil
        otherButton:nil
        informativeTextWithFormat:@"The default Web Assembler configuration could not be loaded.  This may indicate a serious problem."
        ] runModal];
      
      return;
    }
    CFRelease(defaultConfigPath);
    CFRelease(defaultConfigURL);
  }else{
    NSLog(@"No default configuration present; this is weird, but ok...");
  }
  
  if((status = options->loadConfigurationFromXML((CFStringRef)config)) != KSStatusOk){
    
    [[NSAlert
      alertWithMessageText:@"Could Not Load Assembler Configuration"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, the Assembler configuration file could not be loaded from: %@.", config
      ] runModal];
    
    return;
  }
  
  options->setFlags(kOptionVerbose);
  
  NSLog(@"Assembler configuration loaded: %@", config);
  NSLog(@"  Project Base: %@", options->getProjectBasePath());
  NSLog(@"   Output Base: %@", options->getOutputBasePath());
  
  CFStringRef spath = options->getProjectBasePath();
  CFStringRef opath = options->getOutputBasePath();
  KSError *error;
  
  if((status = ALAssemblerProcessPath(options, spath, spath, opath, &error)) != KSStatusOk){
    
    [[NSAlert
      alertWithMessageText:@"Could Not Assemble Project"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, an error occured while assembling your project: %@.", KSUnwrap(error->getMessage())
      ] runModal];
    
    KSRelease(error);
    goto error;
  }
  
  [[NSAlert
    alertWithMessageText:@"Okay, All Done."
    defaultButton:@"Thanks! You're Great!"
    alternateButton:nil
    otherButton:nil
    informativeTextWithFormat:@"Assembler is done processing this site.  Yeah, this dialog is annoying, but otherwise you wouldn't know...\n"
    ] runModal];
  
error:
  if(options) KSRelease(options);
  
}

/**
 * Assemble and preview the currently focused file
 * 
 * @param sender event sender
 */
-(void)assembleAndPreviewCurrentFile:(id)sender {
  NSLog(@"Assemble and preview current...");
  
  CodaTextView *view;
  if((view = [self.controller focusedTextView:self]) == nil){
    
    [[NSAlert
      alertWithMessageText:@"Cannot Assemble Nothing"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, you cannot assemble and preview a file until you have, uh... opened a file."
      ] runModal];
    
    return;
  }
  
  NSString *document;
  if((document = [view path]) == nil){
    
    [[NSAlert
      alertWithMessageText:@"Cannot Assemble Unsaved File"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, you have to save the file before you can assemble and preview it."
      ] runModal];
    
    return;
  }
  
  NSString *config;
  if((config = [self currentSiteConfigurationPath]) == nil){
    BOOL inSite = [self currentSitePath] != nil;
    
    NSInteger result =
      [[NSAlert
        alertWithMessageText:@"Cannot Assemble Project"
        defaultButton:(inSite) ? @"Cancel" : @"Ok"
        alternateButton:nil
        otherButton:(inSite) ? @"Define Site Configuration" : nil
        informativeTextWithFormat:@"Sorry, you cannot assemble and preview a project until you have defined a configuration for the file's site."
        ] runModal];
    
    if(result == NSAlertOtherReturn){
      [self defineConfigurationPathForSite:sender];
    }
    
    return;
  }
  
  KSStatus status;
  ALOptionsModel *options = new ALOptionsModel();
  
  CFURLRef defaultConfigURL;
  if((defaultConfigURL = CFBundleCopyResourceURL(CFBundleGetBundleWithIdentifier(kAssemblerCoreBundleID), CFSTR("webasm-default"), CFSTR("xml"), NULL)) != NULL){
    CFStringRef defaultConfigPath = CFURLCopyPath(defaultConfigURL);
    if(options->loadConfigurationFromXML(defaultConfigPath) != KSStatusOk){
      
      [[NSAlert
        alertWithMessageText:@"Could Not Load Default Configuration"
        defaultButton:@"Ok"
        alternateButton:nil
        otherButton:nil
        informativeTextWithFormat:@"The default Web Assembler configuration could not be loaded.  This may indicate a serious problem."
        ] runModal];
      
      return;
    }
    CFRelease(defaultConfigPath);
    CFRelease(defaultConfigURL);
  }else{
    NSLog(@"No default configuration present; this is weird, but ok...");
  }
  
  if((status = options->loadConfigurationFromXML((CFStringRef)config)) != KSStatusOk){
    
    [[NSAlert
      alertWithMessageText:@"Could Not Load Assembler Configuration"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, the Assembler configuration file could not be loaded from: %@.", config
      ] runModal];
    
    return;
  }
  
  options->setFlags(kOptionVerbose);
  
  NSLog(@"Assembler configuration loaded: %@", config);
  NSLog(@"  Project Base: %@", options->getProjectBasePath());
  NSLog(@"   Output Base: %@", options->getOutputBasePath());
  
  CFStringRef volume = KSFile::copyVolumeNameForPath((CFStringRef)document);
  CFStringRef volumePath = CFStringCreateWithFormat(NULL, 0, CFSTR("/Volumes/%@"), volume);
  
  NSString *realpath;
  if([document hasPrefix:(NSString *)volumePath]){
    realpath = [document substringFromIndex:CFStringGetLength(volumePath)];
  }else{
    realpath = document;
  }
  
  CFStringRef spath = options->getProjectBasePath();
  CFStringRef opath = options->getOutputBasePath();
  KSError *error;
  
  CFStringRef vpath = CFStringCreateByRelocatingPath((CFStringRef)realpath, opath, false);
  CFStringRef vbase = CFStringCopyPathDirectory(vpath);
  CFStringRef vname = CFStringCopyLastPathComponent(vpath);
  CFStringRef zname = CFStringCreateWithFormat(NULL, 0, CFSTR(".WebAssemblerPreview-%@"), vname);
  CFStringRef zpath = CFStringCreateCanonicalPath(vbase, zname);
  
  KSFile *ifile = new KSFile((CFStringRef)document);
  KSFile *ofile = new KSFile(zpath);
  KSInputStream *istream = NULL;
  KSOutputStream *ostream = NULL;
  
  if((istream = ifile->createInputStream(&status)) == NULL){
    
    [[NSAlert
      alertWithMessageText:@"Could Not Assemble & Preview File"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, the document could not be loaded: %@: %@", document, KSStatusGetMessage(status)
      ] runModal];
    
    goto error;
  }
  
  if((ostream = ofile->createOutputStream(&status)) == NULL){
    
    [[NSAlert
      alertWithMessageText:@"Could Not Assemble & Preview File"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, the output file could not be created: %@: %@", zpath, KSStatusGetMessage(status)
      ] runModal];
    
    goto error;
  }
  
  if((status = ALAssemblerProcessStream(options, istream, ostream, (CFStringRef)realpath, zpath, spath, opath, &error)) != KSStatusOk){
    
    [[NSAlert
      alertWithMessageText:@"Could Not Assemble & Preview File"
      defaultButton:@"Ok"
      alternateButton:nil
      otherButton:nil
      informativeTextWithFormat:@"Sorry, an error occured while trying to assemble this document: %@.", KSUnwrap(error->getMessage())
      ] runModal];
    
    KSRelease(error);
    goto error;
  }
  
  [[NSWorkspace sharedWorkspace] openFile:(NSString *)zpath];
  
error:
  if(options) KSRelease(options);
  if(vpath)   CFRelease(vpath);
  if(vname)   CFRelease(vname);
  if(zname)   CFRelease(zname);
  if(zpath)   CFRelease(zpath);
  
  if(istream){
    istream->close();
    KSRelease(istream);
  }
  
  if(ostream){
    ostream->close();
    KSRelease(ostream);
  }
  
}

/**
 * Current site base path
 * 
 * @return base path
 */
-(NSString *)currentSitePath {
  
  CodaTextView *view;
  if((view = [self.controller focusedTextView:self]) == nil)
    return nil;
  
  return [view siteLocalPath];
}

/**
 * Obtain the current site configuration
 * 
 * @return current site configuration
 */
-(NSDictionary *)currentSiteConfig {
  
  NSString *psite;
  if((psite = [self currentSitePath]) != nil){
    return (NSDictionary *)[[NSUserDefaults standardUserDefaults] objectForKey:[NSString stringWithFormat:@"%@%@", kAssemblerConfigPrefix, psite]];
  }
  
  return nil;
}

/**
 * Set the current site configuration
 * 
 * @param config site configuration
 */
-(void)setCurrentSiteConfig:(NSDictionary *)config {
  
  NSString *psite;
  if((psite = [self currentSitePath]) != nil){
    [[NSUserDefaults standardUserDefaults] setObject:config forKey:[NSString stringWithFormat:@"%@%@", kAssemblerConfigPrefix, psite]];
  }
  
}

/**
 * Obtain a property of the current site configuration
 * 
 * @param key property key
 * @return property value
 */
-(id)currentSiteConfigObjectForKey:(NSString *)key {
  
  NSDictionary *config;
  if((config = [self currentSiteConfig]) != nil){
    return [config objectForKey:key];
  }
  
  return nil;
}

/**
 * Set a property of the current site configuration
 * 
 * @param value property value
 * @param key property key
 * @return set or not
 */
-(BOOL)setCurrentSiteConfigObject:(id)value forKey:(NSString *)key {
  NSMutableDictionary *mconfig;
  
  NSDictionary *config;
  if((config = [self currentSiteConfig]) == nil){
    config = [NSDictionary dictionary];
  }
  
  mconfig = [config mutableCopy];
  [mconfig setObject:value forKey:key];
  [self setCurrentSiteConfig:mconfig];
  [mconfig release];
  
  return TRUE;
}

/**
 * Current site configuration path
 * 
 * @return configuration path
 */
-(NSString *)currentSiteConfigurationPath {
  return [self currentSiteConfigObjectForKey:kAssemblerConfigConfigPathKey];
}

/**
 * Set the current site configuration path
 * 
 * @param path configuration path
 */
-(BOOL)setCurrentSiteConfigurationPath:(NSString *)path {
  return [self setCurrentSiteConfigObject:path forKey:kAssemblerConfigConfigPathKey];
}

/**
 * Obtain the current site preview mode
 * 
 * @return preview mode
 */
-(NSString *)globalPreviewMode {
  return [[NSUserDefaults standardUserDefaults] stringForKey:[NSString stringWithFormat:@"%@%@", kAssemblerConfigPrefix, kAssemblerConfigPreviewModeKey]];
}

/**
 * Set the current site preview mode
 * 
 * @param mode preview mode
 */
-(BOOL)setGlobalPreviewMode:(NSString *)mode {
  [self setCurrentSiteConfigObject:mode forKey:[NSString stringWithFormat:@"%@%@", kAssemblerConfigPrefix, kAssemblerConfigPreviewModeKey]];
  return true;
}

/**
 * Obtain the work panel
 * 
 * @return work panel
 */
-(NSPanel *)workPanel {
  static NSNib *workNib = nil;
  
  if(workNib == nil){
    
    if((workNib = [[NSNib alloc] initWithNibNamed:@"AssemblerWorkPanel" bundle:[NSBundle bundleForClass:[self class]]]) == nil){
      NSLog(@"Unable to initialize work panel nib");
      return nil;
    }
    
    if(![workNib instantiateNibWithOwner:self topLevelObjects:nil]){
      NSLog(@"Unable to instantiate work panel nib");
      return nil;
    }
    
  }
  
  return _workPanel;
}

@dynamic workPanel;
@synthesize controller = _controller;

@end

