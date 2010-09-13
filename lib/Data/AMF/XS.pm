package Data::AMF::XS;
use strict;
use XSLoader;
use base 'Exporter';

our $VERSION = '0.01';

our @EXPORT = 'encode_amf0';

XSLoader::load __PACKAGE__, $VERSION;

1;

__END__

=head1 NAME

Data::AMF::XS - Module abstract (<= 44 characters) goes here

=head1 SYNOPSIS

  use Data::AMF::XS;
  blah blah blah

=head1 DESCRIPTION

Stub documentation for this module was created by ExtUtils::ModuleMaker.
It looks like the author of the extension was negligent enough
to leave the stub unedited.

Blah blah blah.

=head1 AUTHOR

Daisuke Murase <typester@cpan.org>

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2009 by KAYAC Inc.

This program is free software; you can redistribute
it and/or modify it under the same terms as Perl itself.

The full text of the license can be found in the
LICENSE file included with this module.

=cut
