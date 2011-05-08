#import <Cocoa/Cocoa.h>

// 
// This object is passed during initialization. You must register your
// available functionality with one of the methods implemented by the 
// plug-in controller
//

@class CodaTextView;

@interface CodaPlugInsController : NSObject 
{
	NSMutableArray*			plugins;
	NSMutableDictionary*	loadedMenuItemsDict;
}

// The following methods are available to plugin developers
// in Coda 1.6 and later:


// codaVersion returns the version of Coda that is hosting the plugin,
// such as "1.6.1"

- (NSString*)codaVersion:(id)sender;


// focusedTextView returns to the plugin an abstract object representing the text view
// in Coda that currently has focus

- (CodaTextView*)focusedTextView:(id)sender;


// registerActionWithTitle:target:selector: exposes to the user a plugin action (a menu item)
// with the given title, that will perform the given selector on the target

- (void)registerActionWithTitle:(NSString*)title target:(id)target selector:(SEL)selector;


// apiVersion returns 2 as of Coda 1.6.  It does not exist in previous versions.

- (int)apiVersion;


// Displays the provided HTML in a new tab. 

- (void)displayHTMLString:(NSString*)html;


// Creates a new unsaved document in the frontmost Coda window and returns the Text View associated with it.
// The text view provided is auto-released, so the caller does not need to explicitly release it.

- (CodaTextView*)makeUntitledDocument;


// Similar to registerActionWithTitle:target:selector: but allows further customization of the registered
// menu items, including submenu title, represented object, keyEquivalent and custom plug-in name.

- (void)registerActionWithTitle:(NSString*)title
		  underSubmenuWithTitle:(NSString*)submenuTitle
						 target:(id)target
					   selector:(SEL)selector
			  representedObject:(id)repOb
				  keyEquivalent:(NSString*)keyEquivalent
					 pluginName:(NSString*)aName;


// Causes the frontmost Coda window to save all documents that have unsaved changes.

- (void)saveAll;

@end


// 
// This is your hook to a text view in Coda. You can use this to provide 
// manipulation of files.
//

@class StudioPlainTextEditor;

@interface CodaTextView : NSObject
{
	StudioPlainTextEditor* editor;
}

// The following methods are available to plugin developers
// in Coda 1.6 and later:


// insertText: inserts the given string at the insertion point

- (void)insertText:(NSString*)inText;


// replaces characters in the given range with the given string

- (void)replaceCharactersInRange:(NSRange)aRange withString:(NSString *)aString;


// selectedRange returns the range of currently selected characters

- (NSRange)selectedRange;


// selectedText returns the currently selected text, or nil if none

- (NSString*)selectedText;


// setSelectedRange: selects the given character range

- (void)setSelectedRange:(NSRange)range;

// currentLine returns a string containing the entire content of the
// line that the insertion point is on

- (NSString*)currentLine;


// currentLineNumber returns the line number corresponding to the 
// location of the insertion point

- (unsigned int)currentLineNumber;


// deleteSelection deletes the selected text range

- (void)deleteSelection;


// lineEnding returns the current line ending of the file

- (NSString*)lineEnding;


// Returns the character range of the entire line the insertion point
// is on

- (NSRange)rangeOfCurrentLine;


// startOfLine returns the character index (relative to the beginning of the document)
// of the start of the line the insertion point is on

- (unsigned int)startOfLine;


// string returns the entire document as a plain string

- (NSString*)string;


// stringWithRange: returns the specified ranged substring of the entire document

- (NSString*)stringWithRange:(NSRange)range;


//tabWidth: returns the width of tabs as spaces

- (int)tabWidth;


// previousWordRange: returns the range of the word previous to the insertion point

- (NSRange)previousWordRange;


// usesTabs returns if the editor is currently uses tabs instead of spaces for indentation

- (BOOL)usesTabs;

// saves the document you are working on

- (void)save;

// saves the document you are working on to a local path, returns YES if successful

- (BOOL)saveToPath:(NSString*)aPath;

// allows for multiple text manipulations to be considered one "undo/redo"
// operation

- (void)beginUndoGrouping;
- (void)endUndoGrouping;


// returns the window the editor is located in (useful for showing sheets)

- (NSWindow*)window;


// returns the path to the text view's file (may be nil for unsaved documents)

- (NSString*)path;


// returns the root local path of the site if specified (nil if unspecified in the site or site is not loaded)

- (NSString*)siteLocalPath;


@end


// 
// Your plug-in must conform to this protocol
//

@protocol CodaPlugIn

- (id)initWithPlugInController:(CodaPlugInsController*)aController bundle:(NSBundle*)yourBundle;
- (NSString*)name;

@end




