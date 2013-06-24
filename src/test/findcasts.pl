 #!/usr/bin/perl
#
# Find C-style casts in C++ sourc code
#
# � 2003 Andrew Dunbar

use strict;

use File::Find;

my $dir;

# Mac file open dialog
# TODO this code for Macs, interpret command line args for others
if ($^O eq 'MacOS') {
        $dir = MacPerl::Choose(GUSI::AF_FILE(), 0, 'Pick a folder ... ', '',
                GUSI::CHOOSE_DIR());
} else {
        $dir = $ARGV[0];
}

if ($dir) {
        print "Searching all files '$dir'...\n";
        find(\&wanted, $dir);
} else {
        print "Nothing to Convert.\n";
}

sub wanted {
        return if (/BeOS/);
        return if (/QNX/);
        return if (/Mac/);
        return if (/Cocoa/);
        return if (/Unix/);
        #if (/\.(h|hpp)$/) {
        if (/\.(cpp)$/) {
                filter($File::Find::name, $_);
        }
        if (/\.(ccg)$/) {
                filter($File::Find::name, $_);
        }
        if (/\.(cc)$/) {
                filter($File::Find::name, $_);
        }
}

sub filter {
        my ($path, $file) = @_;
        if (open(fh, "<$file")) {
                binmode fh;
                my $lino = 0;
                while (<fh>) {
                        ++$lino;
                        if (/^(.*)(\(\s*(\w+(\s*(\w+|\*))*)\s*\)\s*((--|\+\+|&|-|!|\*)?\w+))(.*)/) {
                                my $ml = $_;
                                my $pre = $1;
                                my $mat = $2;
                                my $ctype = $3;
                                my $cexp = $6;
                                my $post = $8;

                                # inside comment
                                next if ($pre =~ /\/\//);
                                next if ($pre =~ /\/\*/ && $pre !~ /\*\//);

                                # copyright
                                next if (lcfirst($ctype) eq 'c' && $cexp >= 1900 && $cexp < 3000);

                                # const member function declaration
                                next if ($cexp eq 'const');

                                # to the left is alphabetic
                                if ($pre =~ /\w\s*$/) {
                                        next unless ($pre =~ /return\s*$/);
                                }

                                print STDOUT "$File::Find::name:$lino: $ml";
                                print STDOUT "$File::Find::name:$lino: $mat -> xxx_cast<$ctype>($cexp...)\n\n";
                        }
                        if (/^(.*)(\(\s*(\w+(\s*(\w+|\*))*)\s*\)\s*)(\(.*)/) {
                                my $ml = $_;
                                my $pre = $1;
                                my $mat = $2;
                                my $ctype = $3;
                                my $post = $6;

                                # inside comment
                                next if ($pre =~ /\/\//);
                                next if ($pre =~ /\/\*/ && $pre !~ /\*\//);

                                # to the left is alphabetic
                                if ($pre =~ /\w\s*$/) {
                                        next unless ($pre =~ /return\s*$/);
                                }

                                print STDOUT "$File::Find::name:$lino: $ml";
                                print STDOUT "$File::Find::name:$lino: ($ctype)(...) -> xxx_cast<$ctype>(...)\n\n";
                        }
                }
                close(fh);
        } else {
                print "Can't open input $file $!\n";
        }
} 
