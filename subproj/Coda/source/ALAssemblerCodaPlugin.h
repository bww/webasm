// 
// $Id: ALAssemblerCodaPlugin.h 21 2008-12-08 07:27:10Z brian $
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

#import <Cocoa/Cocoa.h>
#import "CodaPlugInsController.h"

@interface ALAssemblerCodaPlugin : NSObject <CodaPlugIn> {
  
  CodaPlugInsController *_controller;
  NSPanel               *_workPanel;
  
}

-(id)initWithPlugInController:(CodaPlugInsController *)controller bundle:(NSBundle *)bundle;
-(NSString *)name;

-(void)defineConfigurationPathForSite:(id)sender;
-(void)definePreviewMode:(id)sender;
-(void)assembleAllFiles:(id)sender;
-(void)assembleAndPreviewCurrentFile:(id)sender;

-(NSString *)currentSitePath;
-(NSString *)currentSiteConfigurationPath;
-(BOOL)setCurrentSiteConfigurationPath:(NSString *)path;
-(NSString *)globalPreviewMode;
-(BOOL)setGlobalPreviewMode:(NSString *)mode;

@property (readonly) CodaPlugInsController *controller;
@property (readonly) NSPanel *workPanel;

@end

