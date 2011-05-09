WEB ASSEMBLER

Web Assembler creates complete HTML documents from reusable document fragments,
the output of scripts, evaluated JavaScript, rendered text, and other information.
It can be loosely thought of as a compiler for HTML and other text-based document
formats.  It has been mainly used by Wolter Group for assembling documents from
template components.

SUPPORTED PLATFORMS

Technically, Web Assembler could be compiled on a variety of platforms, however
it relies heavily on Apple's CoreFoundation framework for some core functionality
(strings, arrays, etc.) and while, officially, CoreFoundation is portable, in
practice this is a pain in the ass.  So, for practical purposes, Web Assembler is
really only targeted to (and tested on) Mac OS X until someone gets around to
making a reasonably good CoreFoundation port for other platforms.

BUILDING THE COMMAND LINE TOOL FROM SOURCE

You can build Web Assembler from source using the included Xcode project. The
"Assembler" target will build and install dependency frameworks and build and
install the Web Assembler command line tool.

Frameworks will be copied to:
  /Library/Frameworks
  
The command line tool will be copied to:
  $HOME/Applications/bin

You can change these install directories in the Copy Files build phases for the
various targets.  Note that changing the install location for the dependency
frameworks may cause linkage problems if you don't make corresponding changes
to the target build configurations.

USING THE ASSEMBLER CORE FRAMEWORK

Most of the work done by Web Assembler is performed by the AssemblerCore
framework.  If you're so inclined, you can incorporate this framework into
another project (as we have with the Code plugin).  This is the primary reason
AssemblerCore and Keystone are built as separate frameworks instead of being
linked directly into the command line tool.

MORE INFO ABOUT WEB ASSEMBLER

Read more about the Web Assembler homepage at:
  http://woltergroup.net/webasm/

