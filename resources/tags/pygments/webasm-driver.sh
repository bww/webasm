#!/bin/bash
# Web Assembler Driver

# resolve links - $0 may be a link
PRG="$0"
progname=`basename "$0"`

# need this for relative symlinks
while [ -h "$PRG" ] ; do
  ls=`ls -ld "$PRG"`
  link=`expr "$ls" : '.*-> \(.*\)$'`
  if expr "$link" : '/.*' > /dev/null; then
  PRG="$link"
  else
  PRG=`dirname "$PRG"`"/$link"
  fi
done

EXT_HOME=`dirname "$PRG"`/..

# make it fully qualified
EXT_HOME=`cd "$EXT_HOME" > /dev/null && pwd`
EXT_PATH="$EXT_HOME/pygments/pygmentize-webasm"

$EXT_PATH -l $WEBASM_TAG_ATTR_language -f html

