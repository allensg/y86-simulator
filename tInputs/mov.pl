#!/usr/bin/perl

if ($#ARGV != 1)
{
    print "usage: mv source_prefix dest_prefix\n";
    exit;
}
print "$ARGV[0]\n";
print "$ARGV[1]\n";

$expr = $ARGV[0]."*.*";
print $expr."\n";
@files = glob($expr);

foreach $file (@files) {
    @pieces = split('\\.', $file);
    $prefix = $pieces[0];
    $suffix = $pieces[1];
#    $lastchar = substr($prefix, -1, 1);
    $newfile = $ARGV[1].".".$suffix;
    print "source: $file   dest: $newfile\n";
    system("cp $file $newfile");
    system("rm $file");
#    print "\n".$file."    ".$prefix."      ".$suffix."     ".$newfile."\n";
#   foreach $piece (@pieces)
#   {
#       print "   ".$piece;
#   }
}


