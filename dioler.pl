#!/usr/bin/perl -w

#
# Dioler 1.0
#
# dioler.pl
# (main program)
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

use strict;

use LWP;
use LWP::Simple;
use URI;
use Cwd;
use Getopt::Long;

require "files.pl.inc";
require "http.pl.inc";

# crawler's name
my $USER_AGENT_NAME = "Dioler/1.0";
my $URI_ABS = 0;
my $URI_REL = 1;

my $base_url = "";  # current working directory
my $conf_filename = "dioler.conf";
GetOptions
("base=s" => \$base_url,
 "conf=s" => \$conf_filename);
if($base_url eq "")
{ show_usage();
  exit;
  }
# array of pages to visit. To be accessed FIFO
my @to_visits = ();
# is a URI already visited? This variable is for fast lookup
my %is_visited = ();
my $curr_dir = cwd();
# read config parameters
my %settings = get_settings($conf_filename);
my $request_timeout = $settings{"REQUEST_TIMEOUT"};
$request_timeout = 30 unless($request_timeout);
my $storage_dirname = $settings{"STORAGE_DIRNAME"};
my $delay = $settings{"DELAY"};
my $domain = $settings{"DOMAIN"};
my $proxy = $settings{"PROXY"};
my $is_restrict_domain = defined($domain);
# create a crawler
my $ua = LWP::UserAgent->new();
$ua->agent($USER_AGENT_NAME);
$ua->timeout($request_timeout);
$ua->proxy("http", $proxy) if $proxy;
my $base_uri = validate_uri($base_url);
my $uri = new_uri_ref();
$uri->[$URI_ABS] = $base_uri;
$uri->[$URI_REL] = $base_uri->rel($uri->[$URI_ABS]);
push @to_visits, $uri;
while(@to_visits)
{ # get the next URI
  my $next_uri_ref = shift @to_visits;
  # extract the absolute URI
  my $next_uri = $next_uri_ref->[0];
  next if $is_visited{$next_uri};
  if($is_restrict_domain)
  { my $host = (new URI($next_uri))->host;
    next unless is_subdomain_of($domain, $host);
    }
  # validate URI
  $next_uri = validate_uri($next_uri);
  # get path for storing on local disk
  my $local_file = $next_uri;
  $local_file =~ s/\//%%/g;
  $local_file =~ s/:/__/g;
  $local_file = "$storage_dirname/$local_file";
  my $local_dir = get_dir($local_file);
  # retrieve the document
  my $response = $ua->get($next_uri);
  if($response->is_success)
  { my $content_type =
       (split /;/,
        ($response->headers()->header("content-type"))[0])[0];
    print STDERR "$content_type: $next_uri --> $local_file\n";
    if(is_playlist($content_type))
    { # handle playlist files
      extract_playlist($response->content(), $response->base(),
                       \@to_visits);
      }
    elsif(is_audio($content_type))
    { # handle audio files
      create_dir($local_dir);
      $ua->mirror($next_uri, $local_file);
      }
    elsif(is_hypertext($content_type))
    { # handle html files
      extract_links($response, \@to_visits);
      }
    $is_visited{$next_uri} = 1;
    sleep($delay);
    }
  else
  { warn "Can't retrieve $next_uri: $!\n";
    next;
    }
  }

#
# sub: show_usage
# purpose: shows program usage
#
sub show_usage
{ print STDERR<<EOM;
Dioler v1.0

by: Iman S. H. Suyoto

Copyright (C) 2004 by Iman S. H. Suyoto and Kok Mun Loon

Usage:
dioler.pl --base=base-url --conf=conf-file
EOM
  }
