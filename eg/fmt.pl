#!/usr/bin/perl
use strict;
use warnings;

use Time::Moment;

my @formats = (
    #                 Basic               Extended
    ['Calendar date', '%Y%m%dT%H%M%S%z',  '%Y-%m-%dT%H:%M:%S%Z'  ],
    ['Ordinal date',  '%Y%jT%H%M%S%z',    '%Y-%jT%H:%M:%S%Z'     ],
    ['Week date',     '%GW%V%uT%H%M%S%z', '%G-W%V-%uT%H:%M:%S%Z' ],
);

my $tm = Time::Moment->now;
foreach my $f (@formats) {
    my ($name, $basic, $extended) = @$f;
    printf "%-15s %-22s %-22s\n", 
      $name, $tm->strftime($basic), $tm->strftime($extended);
}

