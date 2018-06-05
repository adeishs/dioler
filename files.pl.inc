#
# Dioler 1.0
#
# dioler.pl
# (file-handling subs)
#
# Iman S. H. Suyoto and Kok Mun Loon
#
# based on Surething 0.1a
# by Iman S. H. Suyoto and Kok Mun Loon
#
# Compiler: Perl 5.8 or later.
#
# Copyright (C) 2004 by Iman S. H. Suyoto and Kok Mun Loon.
#

use FileHandle;
use strict;

#
# sub: get_settings
# parameter: $conf_filename
# return: hash of settings
# purpose: gets settings from configuration file
# author: Iman S. H. Suyoto
#
sub get_settings
{ my $conf_filename = shift;
  my %result = ();
  my $fh = new FileHandle;
  if($fh->open("<$conf_filename"))
  { while(<$fh>)
    { chomp;
      next if((/%\#/) || ($_ eq ""));
      my ($key, $value) = split /\s+/;
      $result{$key} = $value;
      }
    $fh->close;
    }
  return %result;
  }

#
# sub: get_dir
# parameter: $file
# purpose: gets directory part of a full path name
# author: Kok Mun Loon and Iman S. H. Suyoto
#
sub get_dir
{ # take full path name and return only the directory part
  my $file = shift;
  my $dir = "";
  if($file =~ /(^.*\/)/)
  { $dir = $1;
    }
  return $dir;
  }

#
# sub: create_dir
# parameter: $dir_name
#            $curr_dir_name
# purpose: creates subdirectory
# author: Kok Mun Loon and Iman S. H. Suyoto
#
sub create_dir
{ # take full path name and create the directories if not exist
  my ($dir_name) = @_;
  my $created_dir = "./";
  if(defined($dir_name))
  { chomp $dir_name;
    }
  else
  { $dir_name = "";
    }
  my @dirs = split '/', $dir_name;
  foreach my $dir(@dirs)
  { $created_dir .= "$dir/";
    unless($dir =~ /\s+/ || !(defined($dir)) || $dir eq "")
    { mkdir($created_dir)
      unless((-e $created_dir) && (-d $created_dir));
      }
    }
  }

#
# sub: is_audio
# parameter: $content_type
# return: 1 if $content_type is audio, 0 otherwise
# purpose: determines whether $content_type is audio
# author: Iman S. H. Suyoto
#
sub is_audio
{ $_ = shift;
  return 1 if(/^audio\//);
  return 0;
  }

#
# sub: is_playlist
# parameter: $content_type
# return: 1 if $content_type is playlist, 0 otherwise
# purpose: determines whether $content_type is playlist
# author: Iman S. H. Suyoto
#
sub is_playlist
{ my $content_type = shift;
  return 1 if(($content_type eq "audio/m3u") ||
              ($content_type eq "audio/x-m3u") ||
              ($content_type eq "audio/x-mpegurl"));
  return 0;
  }

my @HYPERTEXT_CONTENT_TYPES =
   sort("text/html",
        "application/xhtml+xml",
        "application/xml",
        "text/xml"
        );

#
# sub: is_hypertext
# parameter: $content_type
# return: 1 if $content_type is hypertext, 0 otherwise
# purpose: determines whether $content_type is hypertext
# author: Iman S. H. Suyoto
# note:
# this still implements linear search. As the
# @HYPERTEXT_CONTENT_TYPES grows bigger, a binary search should
# be implemented instead.
#
sub is_hypertext
{ my $content_type = shift;
  foreach my $htct(@HYPERTEXT_CONTENT_TYPES)
  { return 1 if($content_type eq $htct);
    }
  }

1;
