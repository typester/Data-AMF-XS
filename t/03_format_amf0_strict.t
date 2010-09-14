use Test::Base;

plan tests => 1 * blocks;

use FindBin;
use File::Spec;

use Data::AMF::XS;

sub serialize {
    encode_amf0($_[0]->{data});
}

sub load {
    my $file = File::Spec->catfile( $FindBin::Bin, $_[0] );
    open my $fh, "<$file";
    my $data = do { local $/; <$fh> };
    close $fh;

    $data;
}

filters {
    input => [qw/yaml serialize/],
    expected => 'load',
};

run_compare;

__DATA__

=== null object
--- input
data: {}
--- expected: data/amf0/null_object


