#!/usr/bin/perl

$infile = shift(@ARGV);
$outfile = shift(@ARGV);
$funcName = shift(@ARGV);

($indev, $inino, $inmode, $innlink, $inuid, $ingid, $inrdev, $insize, $inatime, $inmtime, $inctime, $inblksize, $inblocks) = stat($infile);
($outdev, $outino, $outmode, $outnlink, $outuid, $outgid, $outrdev, $outsize, $outatime, $outmtime, $outctime, $outblksize, $outblocks) = stat($outfile);

print STDERR "GenerateKSStatusMap: File Modify Times: $inmtime / $outmtime\n";

if($outmtime >= $inmtime){
  print STDERR "GenerateKSStatusMap: Output file is up to date.\n";
  exit(0);
}else{
  print STDERR "GenerateKSStatusMap: Output file is out of date.\n";
}

open(INPUT, "< $infile") || die "Unable to open input file: $!";
open(OUTPUT, "> $outfile") || die "Unable to open output file: $!";

print OUTPUT <<END;
// WARNING: THIS FILE IS GENERATED.  DO NOT MAKE MANUAL CHANGES; THEY WILL BE
// OVERWRITTEN THE NEXT TIME THE PROJECT IS BUILT.
// 
// \$Id\$
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

#include <CoreFoundation/CoreFoundation.h>

CFMutableDictionaryRef $funcName(CFMutableDictionaryRef map) {
  CFNumberRef keyVal;
  SInt32 value;
  
END

while(chomp($line = <INPUT>)){
  
  if($line =~ m|([\w]+)\s*=\s*(-?[\d]+),\s*///<\s*(.*)$|){
    print STDOUT "'$1' [$2]: '$3'\n";
    
    print OUTPUT "  // $1 ($2)\n";
    print OUTPUT "  // \n";
    print OUTPUT "  value = $2;\n";
    print OUTPUT "  keyVal = CFNumberCreate(NULL, kCFNumberSInt32Type, &value);\n";
    print OUTPUT "  CFDictionaryAddValue(map, keyVal, CFSTR(\"$3\"));\n";
    print OUTPUT "  CFRelease(keyVal);\n";
    print OUTPUT "\n";
    
  }
  
}

print OUTPUT <<END;
  
  return map;
}
END

close(INPUT) || die "Unable to close input file: $!";
close(OUTPUT) || die "Unable to close output file: $!";

