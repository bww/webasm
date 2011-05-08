// 
// $Id: ALHelpCommand.cc 156 2010-07-25 22:45:00Z brian $
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

#include "ALHelpCommand.hh"
#include <Keystone/KSLog.hh>
#include <Keystone/KSFile.hh>
#include <Keystone/KSInputStream.hh>
#include <Keystone/CFStringAdditions.hh>

#include <AssemblerCore/ALOptionsModel.hh>
#include <AssemblerCore/ALDirectorySourceCollection.hh>
#include <AssemblerCore/ALSourceFileFilter.hh>

#include <unistd.h>

char * ALHelpCopyVersionString(CFStringRef ident);
char * ALHelpCopyShortVersionString(CFStringRef ident);

/**
 * Execute this command
 * 
 * @param options global options
 * @param argc argument count
 * @param argv arguments
 * @return result
 */
int ALHelpCommand::__execute(int argc, char * const argv[]) {
  
  if(argc < 1){
    displayOverviewVerbose();
    return 0;
  }
  
  const char *command = argv[0];
  
  if(strcasecmp(command, "build") == 0 || strcasecmp(command, "mk") == 0){
    displaySection(kALHelpSectionBuild);
  }else if(strcasecmp(command, "plan") == 0 || strcasecmp(command, "pl") == 0){
    displaySection(kALHelpSectionPlan);
  }else if(strcasecmp(command, "parse") == 0 || strcasecmp(command, "wa") == 0){
    displaySection(kALHelpSectionParse);
  }else{
    displayOverviewVerbose();
    return 0;
  }
  
  return 0;
}

/**
 * Display usage
 * 
 * @param stream output stream
 * @param message usage message
 */
void ALHelpDisplayUsage(FILE *stream, CFStringRef message) {
  if(message != NULL){
    char *cmessage = CFStringCopyCString(message, kDefaultStringEncoding);
    fputs(cmessage, stream);
    fputc('\n', stream);
    if(cmessage) free(cmessage);
  }
}

/**
 * Display brief overview
 */
void ALHelpCommand::displayOverviewBrief(void) {
  char *assemblerCoreVersion = ALHelpCopyShortVersionString(kAssemblerCoreBundleID);
  char *assemblerCoreRevision = ALHelpCopyVersionString(kAssemblerCoreBundleID);
  char *keystoneVersion = ALHelpCopyShortVersionString(kKeystoneBundleID);
  char *keystoneRevision = ALHelpCopyVersionString(kKeystoneBundleID);
  
  fprintf(stderr,
    "Web Assembler \u00A9 2010 Wolter Group New York, Inc. All rights reserved.\n"
    "Assembler Core %s (r%s), Keystone %s (r%s)\n"
    "\n"
    KSBOLDULINE("USAGE") ": webasm [options] " KSULINE("command") " [...]\n"
    "       where " KSULINE("command") " is one of: " KSBOLD("build") ", " KSBOLD("plan") ", " KSBOLD("parse") "\n"
    "\n"
    " " KSBOLDULINE("HELP") ": man webasm\n"
    "\n"
    KSBOLDULINE("GLOBAL OPTIONS") ":\n"
    "\n"
    "   -" KSBOLD("config") " " KSULINE("path") "       Use the configuration located at " KSULINE("path") "\n"
    "   -" KSBOLD("source") " " KSULINE("path") "       Set root source path (default: \"./\")\n"
    "   -" KSBOLD("output") " " KSULINE("path") "       Set root output path (default: \"./webasm\")\n"
    "   -" KSBOLD("types") "  " KSULINE("a") "," KSULINE("b") "," KSULINE("c") "      Set file extensions to process (default: \"html, htm\")\n"
    "   -" KSBOLD("match") "  " KSULINE("pattern") "    Process files matching pattern (overrides -" KSBOLD("types") ")\n"
    "   -" KSBOLD("define") " " KSULINE("key") "=" KSULINE("value") "  Define a global property key/value pair\n"
    "   -" KSBOLD("include-hidden") "    Don't ignore hidden files\n"
    "   -" KSBOLD("collapse-white") "    Collapse whitespace in verbatim content (experimental)\n"
    "   -" KSBOLD("verbose") "           Be more verbose\n"
    "   -" KSBOLD("debug") "             Be extremely verbose\n"
    "   -" KSBOLD("help") "              Display more detailed help information\n"
    "\n"
    KSBOLDULINE("COMMANDS") ":\n"
    "\n"
    "   " KSBOLD("build") "   Assemble a project or specific files\n"
    "   " KSBOLD("plan") "    Display a listing of files to be processed\n"
    "   " KSBOLD("parse") "   Parse project files and display a tree representation\n"
    "\n",
    assemblerCoreVersion, assemblerCoreRevision, keystoneVersion, keystoneRevision);
  
  if(assemblerCoreVersion) free(assemblerCoreVersion);
  if(assemblerCoreRevision) free(assemblerCoreRevision);
  if(keystoneVersion) free(keystoneVersion);
  if(keystoneRevision) free(keystoneRevision);
}

/**
 * Display verbose overview
 */
void ALHelpCommand::displayOverviewVerbose(void) {
  char *assemblerCoreVersion = ALHelpCopyShortVersionString(kAssemblerCoreBundleID);
  char *assemblerCoreRevision = ALHelpCopyVersionString(kAssemblerCoreBundleID);
  char *keystoneVersion = ALHelpCopyShortVersionString(kKeystoneBundleID);
  char *keystoneRevision = ALHelpCopyVersionString(kKeystoneBundleID);
  
  fprintf(stderr,
    "Web Assembler \u00A9 2010 Wolter Group New York, Inc. All rights reserved.\n"
    "Assembler Core %s (r%s), Keystone %s (r%s)\n"
    "\n"
    KSBOLDULINE("USAGE") ": webasm [options] " KSULINE("command") " [...]\n"
    "       where " KSULINE("command") " is one of: " KSBOLD("build") ", " KSBOLD("plan") ", " KSBOLD("parse") "\n"
    "\n"
    " " KSBOLDULINE("HELP") ": man webasm\n"
    "\n"
    KSBOLDULINE("GLOBAL OPTIONS") ":\n"
    "\n"
    "   -" KSBOLD("config") " " KSULINE("path") "\n"
    "       Use the configuration file located at " KSULINE("path") ".  Refer to the Web Assembler\n"
    "       documentation for more on configuration files.\n"
    "\n"
    "   -" KSBOLD("source") " " KSULINE("path") "\n"
    "       Set the root source path.  This is the path under which files to assemble\n"
    "       are located.\n"
    "\n"
    "   -" KSBOLD("output") " " KSULINE("path") "\n"
    "       Set the root output path.  This is the path under whilch processed files\n"
    "       will be output.  Processed files will be placed at the same relative\n"
    "       location as under the source root\n"
    "\n"
    "   -" KSBOLD("types") "  " KSULINE("a") "," KSULINE("b") "," KSULINE("c") "\n"
    "       Set extensions of files that should be processed.  Only files matching these\n"
    "       extensions under the source root are evaluated.  More than one extension can\n"
    "       be specified as a list.  The default extensions are: \"html, htm\".\n"
    "\n"
    "   -" KSBOLD("match") "  " KSULINE("pattern") "\n"
    "       Process files matching a regular expression pattern.  Only paths matching\n"
    "       the specified pattern under the source root are evaluated.  This option\n"
    "       overrides -" KSBOLD("types") ".\n"
    "\n"
    "   -" KSBOLD("define") " " KSULINE("key") "=" KSULINE("value") "\n"
    "       Define a property which will be made globally accessible to directives.\n"
    "       The property is expressed as a key/value pair in the form: \"key=value\". A\n"
    "       property defined on the command line overrides the same property defined in\n"
    "       a configuration file.\n"
    "\n"
    "   -" KSBOLD("include-hidden") "\n"
    "       Don't ignore hidden files under the source root.  By default hidden files\n"
    "       are not processed even if their extensions match the types set.\n"
    "\n"
    "   -" KSBOLD("collapse-white") "\n"
    "       Collapse whitespace in verbatim content.  This feature is experimental and\n"
    "       may be removed in a later release.  When enabled, whitespace in processed\n"
    "       files is reduced.  This can be useful in generating more compact HTML at\n"
    "       the expense of legibility.\n"
    "\n"
    "   -" KSBOLD("verbose") "\n"
    "       Be more verbose.  More informational messages are logged to the terminal.\n"
    "\n"
    "   -" KSBOLD("debug") "\n"
    "       Be extremely verbose.  A huge amount of information is logged to the\n"
    "       terminal.\n"
    "\n"
    "   -" KSBOLD("help") "\n"
    "       Display this help information and exit.\n"
    "\n"
    KSBOLDULINE("COMMANDS") ":\n"
    "\n"
    "   " KSBOLD("build") "   Assemble a project or specific files\n"
    "   " KSBOLD("plan") "    Display a listing of files to be processed\n"
    "   " KSBOLD("parse") "   Parse project files and display a tree representation\n"
    "\n",
    assemblerCoreVersion, assemblerCoreRevision, keystoneVersion, keystoneRevision);
  
  if(assemblerCoreVersion) free(assemblerCoreVersion);
  if(assemblerCoreRevision) free(assemblerCoreRevision);
  if(keystoneVersion) free(keystoneVersion);
  if(keystoneRevision) free(keystoneRevision);
}

/**
 * Display help for a specific command
 * 
 * @param command the command
 */
void ALHelpCommand::displaySection(int command) {
  char *assemblerCoreVersion = ALHelpCopyShortVersionString(kAssemblerCoreBundleID);
  char *assemblerCoreRevision = ALHelpCopyVersionString(kAssemblerCoreBundleID);
  char *keystoneVersion = ALHelpCopyShortVersionString(kKeystoneBundleID);
  char *keystoneRevision = ALHelpCopyVersionString(kKeystoneBundleID);
  
  switch(command){
    
    case kALHelpSectionBuild:
      fprintf(stderr,
        "Web Assembler \u00A9 2010 Wolter Group New York, Inc. All rights reserved.\n"
        "Assembler Core %s (r%s), Keystone %s (r%s)\n"
        "\n"
        KSBOLDULINE("BUILD PROJECT or FILES") "\n"
        "\n"
        "By default, the " KSULINE("source root") " directory is recursively traversed and any\n"
        "files that are encountered which match the current input file extension(s)\n"
        "are processed and output to the same relative location under the " KSULINE("output") "\n"
        KSULINE("root") ". Any missing intermediate directories under the output root are\n"
        "created as necessary.\n"
        "\n"
        "For example, given the source root ./source and the output root ./output,\n"
        "the result for a file, input.html, is:\n"
        "\n"
        "  ./source/html/index.html \u2192 ./output/html/index.html\n"
        "\n"
        "A single file (as opposed to a set of files) can be built by specifying\n"
        "a path on the command line:\n"
        "\n"
        "   $ webasm [" KSULINE("options") "] build [-output " KSULINE("path") "] " KSULINE("input") "\n"
        "\n"
        KSBOLDULINE("COMMAND OPTIONS") ":\n"
        "\n"
        "   -" KSBOLD("output") "  " KSULINE("path") "\n"
        "       When building a single input file, output to the specified path.\n"
        "       If an input file is specified on the command line and no output\n"
        "       file is specified, the processed output is written to standard\n"
        "       output.  This can be used to integrate Web Assembler into a\n"
        "       larger toolchain.\n"
        "\n",
        assemblerCoreVersion, assemblerCoreRevision, keystoneVersion, keystoneRevision);
      break;
      
    case kALHelpSectionPlan:
      fprintf(stderr,
        "Web Assembler \u00A9 2010 Wolter Group New York, Inc. All rights reserved.\n"
        "Assembler Core %s (r%s), Keystone %s (r%s)\n"
        "\n"
        KSBOLDULINE("PLAN PROJECT BUILD") "\n"
        "\n"
        "Display a listing of files to be processed if the " KSBOLD("build") " command were invoked,\n"
        "but don't actually do any processing.  This can be used to find out which files\n"
        "a build will affect.\n"
        "\n"
        "  ./source/html/index.html \u2192 ./output/html/index.html\n"
        "\n",
        assemblerCoreVersion, assemblerCoreRevision, keystoneVersion, keystoneRevision);
      break;
      
    default:
      fprintf(stderr,
        "Web Assembler \u00A9 2010 Wolter Group New York, Inc. All rights reserved.\n"
        "Assembler Core %s (r%s), Keystone %s (r%s)\n"
        "\n"
        KSBOLDULINE("PARSE INPUT FILES") "\n"
        "\n"
        "Parse input files and display a representation of the document tree.  This is\n"
        "is mainly only useful for debugging a problematic input file.  You can use this\n"
        "command to see how Web Assembler is interpreting input.\n"
        "\n",
        assemblerCoreVersion, assemblerCoreRevision, keystoneVersion, keystoneRevision);
      break;
  }
  
  if(assemblerCoreVersion) free(assemblerCoreVersion);
  if(assemblerCoreRevision) free(assemblerCoreRevision);
  if(keystoneVersion) free(keystoneVersion);
  if(keystoneRevision) free(keystoneRevision);
}

/**
 * Copy the long version string (revision)
 */
char * ALHelpCopyVersionString(CFStringRef ident) {
  char *version = NULL;
  
  CFBundleRef bundle;
  if((bundle = CFBundleGetBundleWithIdentifier(ident)) != NULL){
    CFDictionaryRef info;
    if((info = CFBundleGetInfoDictionary(bundle)) != NULL){
      CFStringRef string;
      if((string = (CFStringRef)CFDictionaryGetValue(info, kCFBundleVersionKey)) != NULL){
        version = CFStringCopyCString(string, kDefaultStringEncoding);
      }
    }
  }
  
  return version;
}

/**
 * Copy the short version string (dot notation)
 */
char * ALHelpCopyShortVersionString(CFStringRef ident) {
  char *version = NULL;
  
  CFBundleRef bundle;
  if((bundle = CFBundleGetBundleWithIdentifier(ident)) != NULL){
    CFDictionaryRef info;
    if((info = CFBundleGetInfoDictionary(bundle)) != NULL){
      CFStringRef string;
      if((string = (CFStringRef)CFDictionaryGetValue(info, CFSTR("CFBundleShortVersionString"))) != NULL){
        version = CFStringCopyCString(string, kDefaultStringEncoding);
      }
    }
  }
  
  return (version != NULL) ? version : ALHelpCopyVersionString(ident);
}


