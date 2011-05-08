// 
// This is an example Javascript file that demonstrates how Web Assembler
// can be used to perform conditional "compilation".
// 

/* 
 * Log a message to the console.  When the "debug" property is set
 * to "true", the version of this function that actually does logging
 * is used.  When the "debug" property is not set, the version that
 * does nothing is used. This way, the production version doesn't do
 * a bunch of unwanted debug logging.
 */
<w:if test="directive.getProperty('debug') == 'true'">
function LogMessage(message) {
  console.log(message);
}
</w:if>
<w:else>
function LogMessage(message) {
  // Don't log in production
}
</w:else>

// Now, this message will only be logged when debugging.
LogMessage("Hello, I'm debugging now.");


