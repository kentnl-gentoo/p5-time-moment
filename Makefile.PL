use lib '.';
use strict;
use warnings;

BEGIN {
    unshift @INC, 'inc';
}
use inc::Module::Install 1.00 ();
use Module::Install::XSUtil 0.36 ();
use Module::Install::ReadmeFromPod 0 ();

use inc::Module::Install;

name           'Time-Moment';
perl_version   '5.008001';
all_from       'lib/Time/Moment.pm';
repository     'https://github.com/chansen/p5-time-moment';
bugtracker     'https://github.com/chansen/p5-time-moment/issues';
readme_from    'lib/Time/Moment.pod';

requires       'Carp'                => '0';
requires       'XSLoader'            => '0';
requires       'Time::HiRes'         => '0';

test_requires  'Test::More'          => '0.88';
test_requires  'Test::Fatal'         => '0.006';
test_requires  'Test::Requires'      => '0';
test_requires  'Test::Number::Delta' => '1.06';

cc_warnings;
cc_include_paths 'src';
cc_src_paths     '.';

ppport;
requires_external_cc;

WriteAll;
