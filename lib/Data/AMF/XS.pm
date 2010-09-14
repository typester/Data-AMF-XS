package Data::AMF::XS;
use strict;
use XSLoader;
use base 'Exporter';

our $VERSION = '0.01_01';

our @EXPORT = qw/encode_amf0 decode_amf0/;

XSLoader::load __PACKAGE__, $VERSION;

1;

__END__

=for stopwords amf amf0 amf3 deserialize RPC

=head1 NAME

Data::AMF::XS - serialize / deserialize AMF data

=head1 SYNOPSIS

    use Data::AMF::XS;
    
    my $amf0_data    = encode_amf0(@perl_objects);
    my @perl_objects = decode_amf0($amf0_data);

=head1 DESCRIPTION

This module provides simple and fast serialize/deserialize functions for AMF data.

=head1 ALPHA LIMITATION

This module is *ALPHA* release and has several limitations at this time.

=over 4

=item Support only AMF0 (and limited type)

=item No AMF RPC Packet support (only AMF data)

=back

=head1 EXPORTED FUNCTIONS

=head2 encode_amf0($obj, ...)

   my $amf0_data = encode_amf0(@objs);

Serialize perl objects to AMF data.

=head2 decode_amf0($amf0_data) 

   my @objs = decode_amf0($amf0_data);

Deserialize AMF data to perl objects.

NOTICE: this function may return list value when C<$amf0_data> contains multiple objects.
So you should treat return value as list.
If you need only first objects, you should do:

    my ($obj) = decode_amf0($amf0_data);

should not do:

    my $obj = decode_amf0($amf0_data); # WRONG

=head1 AUTHOR

Daisuke Murase <typester@cpan.org>

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2010 by KAYAC Inc.

This program is free software; you can redistribute
it and/or modify it under the same terms as Perl itself.

The full text of the license can be found in the
LICENSE file included with this module.

=cut
