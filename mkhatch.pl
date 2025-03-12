#!/usr/bin/env perl
$size = $#ARGV >= 0 ? $ARGV[0] : 16;
$i = $#ARGV >= 1 ? $ARGV[1] : 3;
$j = $#ARGV >= 2 ? $ARGV[2] : 12;
$k = $#ARGV >= 3 ? $ARGV[3] : 20;
$l = $#ARGV >= 4 ? $ARGV[4] : 31;
$p{$i} = $p{$j} = $p{$k} = $p{$l} = 1;
for ($i = 0;$ i <= $size; ++$i) {
       	for ($j = 0; $j <= $size; ++$j) {
		if ($i == 0 || $i == $size || $j == 0 || $j == $size) {
		       	print "*";
	       	} else {
	 	      	print ($p{$i%4*10+$j%4} ? "*" : " ");
		}
       	}
       	print "\n" ;
}
