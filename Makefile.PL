use inc::Module::Install;

name 'Data-AMF-XS';
all_from 'lib/Data/AMF/XS.pm';

test_requires 'Test::More' => '0.94';
use_test_base;
author_tests('xt');

auto_include;
auto_set_repository;

cc_warnings;
use_ppport 3.19;

if ($ENV{DEBUG}) {
    cc_append_to_ccflags '-g';
}

WriteAll;
