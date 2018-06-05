#
# Dioler 1.0
#
# dioler.pl
# (web and internet-related subs)
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

#
# sub: validate_uri
# parameters: $raw
# returns: valid URI
# purpose: validates a URL
# author: Kok Mun Loon and Iman S. H. Suyoto
#
sub validate_uri
{ # adds "http://" prefix if necessary
  my $raw = shift;
  $raw = "http://" . $raw if($raw !~ /http:\/\//);
  # create a URI
  my $uri = URI->new($raw);
  $uri->canonical;
  return $uri;
  }

#
# sub: new_uri_obj
# returns: new URI array. Element [0] contains the absolute
#          URI. Element [1] contains the relative URI.
# author: Iman S. H. Suyoto and Kok Mun Loon
#
sub new_uri_ref
{ my @uri_obj = (undef, undef);
  return \@uri_obj;
  }

#
# sub: extract_links
# parameters: $response
#             $to_visit_ref (array ref)
# purpose: extract HTTP links from response
# author: Kok Mun Loon and Iman S. H. Suyoto
#
sub extract_links
{ my ($response, $to_visit_ref) = @_;
  my $document = $response->content();
  my $base_uri = $response->base();
  while($document =~ m/href\s*=\s*"([^"\s]+)"/gi)
  { my $new_uri = URI->new_abs($1, $base_uri)->canonical;
    next unless $new_uri =~ /http:\/\//;
    my $uri_ref = new_uri_ref();
    $uri_ref->[0] = $new_uri;
    $uri_ref->[1] = $base_uri->rel($uri_ref->[0]);
    push @$to_visit_ref, $uri_ref;
    }
  }

#
# sub: extract_playlist
# parameters: $playlist
#             $base_uri
#             $to_visit_ref (array ref)
# purpose: extract HTTP links from a playlist
# author: Iman S. H. Suyoto and Kok Mun Loon
#
sub extract_playlist
{ my ($playlist, $base_uri, $to_visit_ref) = @_;
  my @uris = split /\n/, $playlist;
  foreach my $uri(@uris)
  { my $new_uri = URI->new_abs($uri, $base_uri)->canonical;
    my $uri_ref = new_uri_ref();
    $uri_ref->[0] = $new_uri;
    $uri_ref->[1] = $base_uri->rel($uri_ref->[0]);
    push @$to_visit_ref, $uri_ref;
    }
  }

#
# sub: is_subdomain_of
# parameters: $hl_domain, $subdomain
# returns: 1 if $subdomain is a subdomain of $hl_domain, 0
#          otherwise
# purpose: identifies whether $subdomain is a subdomain of
#          $hl_domain
# author: Iman S. H. Suyoto
#
sub is_subdomain_of
{ my ($hl_domain, $subdomain) = @_;
  my $result = 0;
  $_ = $subdomain;
  $result = 1 if(m/$hl_domain/);
  return $result;
  }

1;
