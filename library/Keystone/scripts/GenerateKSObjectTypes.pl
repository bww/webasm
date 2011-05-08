#!/usr/bin/perl

$base     = shift(@ARGV);
$outfile  = shift(@ARGV);
$changes  = shift(@ARGV);
$bindex   = shift(@ARGV) || 0;
$mguard   = shift(@ARGV) || "__KEYSTONE_KSOBJECTYPES__";

$walktool = "/Users/brian/Applications/bin/walk";
$counter = $bindex;
$basetime = substr "". time(), -6;
%ctimes = ();

open(CHANGES, "< $changes");
if(CHANGES){
  while(chomp($line = <CHANGES>)){
    if($line =~ m|([\w\d\.\-\/]+)\s+(\d+)|){
      $ctimes{$1} = $2;
    }
  }
  close(CHANGES);
}

open(CHANGES, "> $changes") || die "Unable to open changes file (output): $!";

@paths = ();
$outofdate = 0;

open(WALKER, "$walktool -DH '$base' |") || die "Unable to walk directory: $!";
while(chomp($path = <WALKER>)){
  
  if(!($path =~ m/([h]{1,2}$)/)){ next; }
  if($path eq $outfile){ next; }
  
  push(@paths, $path);
  
  # MTIME is 9
  $inmtime = (stat($path))[9];
  $ccmtime = $ctimes{$path};
  
  if($ccmtime >= $inmtime){
    print STDERR "GenerateKSObjectTypes: File is up to date: $path\n";
  }else{
    print STDERR "GenerateKSObjectTypes: File is out of date: $path\n";
    $outofdate = 1;
  }
  
  print CHANGES "$path $inmtime\n";
}

close(WALKER);
close(CHANGES) || die "Unable to close changes file: $!";

if(!$outofdate && (stat($outfile) != null)){
  print STDERR "GenerateKSObjectTypes: All files are up to date\n";
  exit(0);
}

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

#if !defined($mguard)
#define $mguard 1

END

foreach(@paths){
  $path = $_;
  
  open(INPUT, "< $path") || die "Unable to open input file: $path: $!";
  
  while(chomp($line = <INPUT>)){
    
    if($line =~ m/^\#define\s+KEYSTONE_CLASS_([\w\d]+)\s+\"([\w\d]+)\"/){
      $typeid = $counter;
      print  STDOUT "Class '$1': $typeid\n";
      printf OUTPUT "#define %-40s $typeid\n", "$1TypeID";
      $counter++;
    }
    
  }
  
  close(INPUT) || die "Unable to close input file: $!";
  
}

print OUTPUT <<END;

#endif //$mguard

END

close(OUTPUT) || die "Unable to close output file: $!";

