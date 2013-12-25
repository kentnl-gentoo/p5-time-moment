#!perl
use strict;
use warnings;

use Test::More;
use t::Util     qw[throws_ok lives_ok];

BEGIN {
    use_ok('Time::Moment');
}

sub TRUE () { !!1 }

{
    my @tests = (
        [ '0001-01-01T00:00:00Z',            -62135596800,           0,    0 ],
        [ '00010101T000000Z',                -62135596800,           0,    0 ],
        [ '0001-W01-1T00:00:00Z',            -62135596800,           0,    0 ],
        [ '0001W011T000000Z',                -62135596800,           0,    0 ],
        [ '0001-001T00:00:00Z',              -62135596800,           0,    0 ],
        [ '0001001T000000Z',                 -62135596800,           0,    0 ],
        [ '1970-01-01T00:00:00Z',                       0,           0,    0 ],
        [ '1970-01-01T02:00:00+02:00',                  0,           0,  120 ],
        [ '1970-01-01T01:30:00+01:30',                  0,           0,   90 ],
        [ '1970-01-01T01:00:00+01:00',                  0,           0,   60 ],
        [ '1970-01-01T00:01:00+00:01',                  0,           0,    1 ],
        [ '1970-01-01T00:00:00+00:00',                  0,           0,    0 ],
        [ '1969-12-31T23:59:00-00:01',                  0,           0,   -1 ],
        [ '1969-12-31T23:00:00-01:00',                  0,           0,  -60 ],
        [ '1969-12-31T22:30:00-01:30',                  0,           0,  -90 ],
        [ '1969-12-31T22:00:00-02:00',                  0,           0, -120 ],
        [ '1970-01-01T00:00:00.123456789Z',             0,   123456789,    0 ],
        [ '1970-01-01T00:00:00.12345678Z',              0,   123456780,    0 ],
        [ '1970-01-01T00:00:00.1234567Z',               0,   123456700,    0 ],
        [ '1970-01-01T00:00:00.123456Z',                0,   123456000,    0 ],
        [ '1970-01-01T00:00:00.12345Z',                 0,   123450000,    0 ],
        [ '1970-01-01T00:00:00.1234Z',                  0,   123400000,    0 ],
        [ '1970-01-01T00:00:00.123Z',                   0,   123000000,    0 ],
        [ '1970-01-01T00:00:00.12Z',                    0,   120000000,    0 ],
        [ '1970-01-01T00:00:00.1Z',                     0,   100000000,    0 ],
        [ '1970-01-01T00:00:00.01Z',                    0,    10000000,    0 ],
        [ '1970-01-01T00:00:00.001Z',                   0,     1000000,    0 ],
        [ '1970-01-01T00:00:00.0001Z',                  0,      100000,    0 ],
        [ '1970-01-01T00:00:00.00001Z',                 0,       10000,    0 ],
        [ '1970-01-01T00:00:00.000001Z',                0,        1000,    0 ],
        [ '1970-01-01T00:00:00.0000001Z',               0,         100,    0 ],
        [ '1970-01-01T00:00:00.00000001Z',              0,          10,    0 ],
        [ '1970-01-01T00:00:00.000000001Z',             0,           1,    0 ],
        [ '1970-01-01T00:00:00.000000009Z',             0,           9,    0 ],
        [ '1970-01-01T00:00:00.00000009Z',              0,          90,    0 ],
        [ '1970-01-01T00:00:00.0000009Z',               0,         900,    0 ],
        [ '1970-01-01T00:00:00.000009Z',                0,        9000,    0 ],
        [ '1970-01-01T00:00:00.00009Z',                 0,       90000,    0 ],
        [ '1970-01-01T00:00:00.0009Z',                  0,      900000,    0 ],
        [ '1970-01-01T00:00:00.009Z',                   0,     9000000,    0 ],
        [ '1970-01-01T00:00:00.09Z',                    0,    90000000,    0 ],
        [ '1970-01-01T00:00:00.9Z',                     0,   900000000,    0 ],
        [ '1970-01-01T00:00:00.99Z',                    0,   990000000,    0 ],
        [ '1970-01-01T00:00:00.999Z',                   0,   999000000,    0 ],
        [ '1970-01-01T00:00:00.9999Z',                  0,   999900000,    0 ],
        [ '1970-01-01T00:00:00.99999Z',                 0,   999990000,    0 ],
        [ '1970-01-01T00:00:00.999999Z',                0,   999999000,    0 ],
        [ '1970-01-01T00:00:00.9999999Z',               0,   999999900,    0 ],
        [ '1970-01-01T00:00:00.99999999Z',              0,   999999990,    0 ],
        [ '1970-01-01T00:00:00.999999999Z',             0,   999999999,    0 ],
        [ '1970-01-01T00:00:00.0Z',                     0,           0,    0 ],
        [ '1970-01-01T00:00:00.00Z',                    0,           0,    0 ],
        [ '1970-01-01T00:00:00.000Z',                   0,           0,    0 ],
        [ '1970-01-01T00:00:00.0000Z',                  0,           0,    0 ],
        [ '1970-01-01T00:00:00.00000Z',                 0,           0,    0 ],
        [ '1970-01-01T00:00:00.000000Z',                0,           0,    0 ],
        [ '1970-01-01T00:00:00.0000000Z',               0,           0,    0 ],
        [ '1970-01-01T00:00:00.00000000Z',              0,           0,    0 ],
        [ '1970-01-01T00:00:00.000000000Z',             0,           0,    0 ],
        [ '1973-11-29T21:33:09Z',               123456789,           0,    0 ],
        [ '2013-10-28T17:51:56Z',              1382982716,           0,    0 ],
        [ '9999-12-31T23:59:59Z',            253402300799,           0,    0 ],
    );

    foreach my $test (@tests) {
        my ($string, $epoch, $nanosecond, $offset) = @$test;

        my $tm = Time::Moment->from_string($string);
        is($tm->epoch,       $epoch,       "${string} epoch");
        is($tm->nanosecond,  $nanosecond,  "${string} nanosecond");
        is($tm->offset,      $offset,      "${string} offset");
    }
}

{
    my $exp = Time::Moment->from_string('2012-12-24T15:30Z');
    my @tests = (
        '2012-12-24 15:30Z',
        '2012-12-24 15:30z',
        '2012-12-24 16:30+01:00',
        '2012-12-24 16:30+0100',
        '2012-12-24 16:30+01',
        '2012-12-24 14:30-01:00',
        '2012-12-24 14:30-0100',
        '2012-12-24 14:30-01',
        '2012-12-24 15:30:00Z',
        '2012-12-24 15:30:00z',
        '2012-12-24 16:30:00+01:00',
        '2012-12-24 16:30:00+0100',
        '2012-12-24 14:30:00-01:00',
        '2012-12-24 14:30:00-0100',
        '2012-12-24 15:30:00.123456Z',
        '2012-12-24 15:30:00.123456z',
        '2012-12-24 16:30:00.123456+01:00',
        '2012-12-24 16:30:00.123456+01',
        '2012-12-24 14:30:00.123456-01:00',
        '2012-12-24 14:30:00.123456-01',
        '2012-12-24t15:30Z',
        '2012-12-24t15:30z',
        '2012-12-24t16:30+01:00',
        '2012-12-24t16:30+0100',
        '2012-12-24t14:30-01:00',
        '2012-12-24t14:30-0100',
        '2012-12-24t15:30:00Z',
        '2012-12-24t15:30:00z',
        '2012-12-24t16:30:00+01:00',
        '2012-12-24t16:30:00+0100',
        '2012-12-24t14:30:00-01:00',
        '2012-12-24t14:30:00-0100',
        '2012-12-24t15:30:00.123456Z',
        '2012-12-24t15:30:00.123456z',
        '2012-12-24t16:30:00.123456+01:00',
        '2012-12-24t14:30:00.123456-01:00',
        '2012-12-24 16:30 +01:00',
        '2012-12-24 14:30 -01:00',
        '2012-12-24 16:30:00 +01:00',
        '2012-12-24 14:30:00 -01:00',
        '2012-12-24 16:30:00.123456 +01:00',
        '2012-12-24 14:30:00.123456 -01:00',
        '2012-12-24 15:30:00.123456 -00:00',
    );
    
    foreach my $string (@tests) {
        {
            my $name = "from_string($string)";
            throws_ok { Time::Moment->from_string($string) } q/^Cannot parse/, $name;
        }
        {
            my $tm;
            my $name = "->from_string($string, TRUE)";
            lives_ok { $tm = Time::Moment->from_string($string, TRUE) } $name;
            is($tm->epoch, $exp->epoch, "$name->epoch");
        }
    }
}

done_testing();

