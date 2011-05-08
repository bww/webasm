#!/usr/bin/perl

my $times = $ENV{"WEBASM_TAG_ATTR_times"} || 2;

while(chomp($line = <STDIN>)){
  @parts = split(/\s+/, $line);
  for(my $i = 0; $i <= $#parts; $i++){
    for(my $j = 0; $j < $times; $j++){
      if(($i + $j) > 0){ print " "; }
      print $parts[$i];
    }
  }
}

