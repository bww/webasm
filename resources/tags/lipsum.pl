#!/usr/bin/perl
# $Id: lipsum.pl 88 2010-01-25 11:19:58Z brian $
# 
# A trivial placeholder text generator for Web Assembler.
# 

$LIPSUM = << "END";
Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean imperdiet feugiat neque vel rhoncus. Pellentesque euismod egestas dolor, a lacinia est pharetra sed. Duis malesuada molestie nisi a ullamcorper. Integer accumsan, dui eget tincidunt dictum, neque tellus ultricies elit, vitae dapibus quam ante non nisi. Sed sit amet sem volutpat massa tincidunt tristique. Fusce eget velit elit. Praesent in blandit turpis. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Duis interdum lacus in augue accumsan a auctor odio tristique. Praesent adipiscing, tortor quis consequat bibendum, ligula justo elementum sapien, ut ornare elit sem sed lectus. Maecenas semper, lacus vitae aliquet vestibulum, risus nisl lacinia augue, vitae eleifend dui est at magna. Donec auctor tellus in eros placerat sodales. Pellentesque eu aliquet leo. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Aenean placerat facilisis bibendum. Pellentesque ligula sem, laoreet ac hendrerit et, egestas ac odio. Nam hendrerit adipiscing lacus in tempor. Suspendisse potenti. Etiam neque velit, dignissim facilisis commodo eget, mattis sit amet diam. Curabitur metus metus, ullamcorper quis rhoncus vitae, lobortis ac est. Vivamus sit amet neque nunc. Fusce vestibulum, arcu vitae sollicitudin ultricies, ante eros ullamcorper mauris, sit amet adipiscing sem lectus non turpis. Praesent faucibus convallis ipsum sit amet bibendum. Sed bibendum leo quis odio rutrum dignissim. Fusce placerat ornare arcu eu laoreet. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Nunc vestibulum gravida sem, viverra faucibus ipsum pellentesque ac. Vestibulum imperdiet sodales ullamcorper. Nam sed lectus eget turpis malesuada volutpat ac in purus. Etiam ut tempus nisl. Quisque id sodales augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Quisque vitae purus non est semper blandit. Duis nec elit quis ante tincidunt ultricies. Donec eros est, ultrices eu ultricies sed, consequat sed mauris. Aenean quis nibh velit, ut rhoncus mi. Proin eu ligula diam. Praesent tincidunt lacus vel tellus rutrum at dapibus tellus porta. Duis pulvinar tincidunt sapien, sed dapibus nulla ultricies non. Sed quis lorem quis ligula consectetur molestie sit amet nec est. Curabitur aliquet, metus suscipit rutrum laoreet, odio urna egestas augue, at consectetur ipsum tellus vulputate turpis. Cras convallis ultrices nulla at varius. Vestibulum auctor sapien at nibh euismod eu porta arcu auctor. Nullam ut lobortis odio. Maecenas feugiat, urna non vulputate facilisis, felis lacus sollicitudin augue, in lacinia mi velit sollicitudin nunc. Nam malesuada placerat ante sit amet semper. Nam velit leo, vulputate nec tincidunt ut, volutpat eget nibh. Pellentesque vel dolor ut lacus pulvinar faucibus. Phasellus congue orci in mauris interdum ornare. Suspendisse potenti. Mauris rutrum interdum ipsum, interdum rutrum erat lacinia vulputate. Ut et dui sem, in commodo risus. In a nunc vitae turpis faucibus euismod semper ut leo. Nunc et ornare sem. Integer tincidunt dictum lacus vitae sagittis. Morbi posuere, lorem ut tincidunt mattis, nisl tortor consequat quam, bibendum convallis ligula mauris sed ligula. Etiam dignissim ultricies egestas. In at mauris in velit scelerisque consectetur. Etiam tincidunt turpis pulvinar elit blandit porta. Integer nec lectus vitae tortor rhoncus pharetra. Fusce faucibus mauris in libero tristique accumsan. Pellentesque lacus tortor, ullamcorper non dignissim et, viverra in mauris. Maecenas egestas, orci quis rutrum suscipit, leo sapien elementum tellus, id mollis dolor est vehicula tortor. Curabitur mollis leo eget tellus tincidunt ornare posuere justo adipiscing. Duis vel nunc vel nulla consectetur hendrerit at eu eros. Suspendisse placerat tempor nibh, vitae adipiscing nibh venenatis ac. Praesent dictum, lectus sit amet facilisis dictum, dui est tincidunt quam, vel rutrum risus lectus et magna. Fusce fermentum vulputate ipsum vel ullamcorper. Integer in felis ac erat elementum ultricies nec eget est. Donec ornare vestibulum libero, id congue nisi sollicitudin quis. Donec est est, viverra id porttitor vel, vehicula viverra orci. Phasellus vel vestibulum orci. In nulla elit, cursus id tristique in, sollicitudin ut nibh. Aliquam suscipit, lacus in bibendum vehicula, ligula ante tempus mauris, consectetur suscipit turpis odio nec risus. In hac habitasse platea dictumst. Ut dapibus fringilla elit, eu hendrerit quam aliquet vitae.
END

my $wcount = $ENV{"WEBASM_TAG_ATTR_words"} || 50;
my @words = split(/[^\w]+/, $LIPSUM);

my $s = 3 + rand(7);
my $p = 5 + rand(5);
my $c = 0;
my $z = 0;
my $i;

print "<p>";

for($i = 0; $i < $wcount; $i++){
  $word = $words[$i % $#words];
  
  if(++$c > $s){
    
    print ".";
    $word =~ s/(\w+)/\u\L$1/;
    $s = 3 + rand(7);
    $c = 0;
    
    if(++$z > $p){
      print "</p>";
      print "<p>";
      $p = 5 + rand(5);
      $z = 0;
    }
    
  }
  
  if($i > 0){ print " "; }
  print $word;
}

if($c <= $s){ print "."; }
print "</p>";

