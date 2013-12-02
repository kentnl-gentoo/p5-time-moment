#include "moment.h"
#include "dt_core.h"
#include "dt_accessor.h"

static const char *aDoW[] = {
    "Mon",
    "Tue",
    "Wed",
    "Thu",
    "Fri",
    "Sat",
    "Sun",
};

static const char *fDoW[] = {
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Sunday",
};

static const char *aMonth[] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};

static const char *fMonth[] = {
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

static const char *Meridiem[] = {
    "AM",
    "PM",
};

/* 
 * The first Sunday of January is the first day of week 1; days in the new
 * year before this are in week 0.
 */
static int
dt_week_number_sun(dt_t dt) {
    int sunday = dt_doy(dt) - dt_dow(dt) % 7;
    return (sunday + 6) / 7;
}

/* 
 * The first Monday of January is the first day of week 1; days in the new
 * year before this are in week 0.
 */
static int
dt_week_number_mon(dt_t dt) {
    int monday = dt_doy(dt) - (dt_dow(dt) + 6) % 7;
    return (monday + 6) / 7;
}

static int 
moment_hour_12(const moment_t *mt) {
    int h = moment_hour(mt) % 12;
    if (h == 0)
        h = 12;
    return h;
}

static const char *
moment_hour_meridiem(const moment_t *mt) {
    return Meridiem[moment_hour(mt) / 12];
}

#define CHR(n, d) (char)('0' + ((n) / (d)) % 10)
static void
THX_format_f(pTHX_ const moment_t *mt, SV *dsv, int len) {
    char buf[6];
    int usec;

    if (len > 6)
        len = 6;
    usec = moment_microsecond(mt);
    switch (len) {
        case 6: buf[5] = CHR(usec, 1);
        case 5: buf[4] = CHR(usec, 10);
        case 4: buf[3] = CHR(usec, 100);
        case 3: buf[2] = CHR(usec, 1000);
        case 2: buf[1] = CHR(usec, 10000);
        case 1: buf[0] = CHR(usec, 100000);
    }
    sv_catpvn(dsv, buf, len);
}
#undef CHR

static void
THX_format_s(pTHX_ const moment_t *mt, SV *dsv) {
    char buf[30], *p, *e;
    int64_t v;

    v = moment_epoch(mt);
    p = e = buf + sizeof(buf);
    if (v < 0) {
        do {
            *--p = '0' - (v % 10);
        } while (v /= 10);
        *--p = '-';
    }
    else {
        do {
            *--p = '0' + (v % 10);
        } while (v /= 10);
    }
    sv_catpvn(dsv, p, e - p);
}

static void
THX_format_z(pTHX_ const moment_t *mt, SV *dsv) {
    int offset, sign;

    offset = moment_offset(mt);
    if (offset < 0)
        sign = '-', offset = -offset;
    else
        sign = '+';
    sv_catpvf(dsv, "%c%04d", sign, (offset / 60) * 100 + (offset % 60));
}

static void
THX_format_Z(pTHX_ const moment_t *mt, SV *dsv) {
    int offset, sign;

    offset = moment_offset(mt);
    if (offset == 0)
        sv_catpvn(dsv, "Z", 1);
    else {
        if (offset < 0)
            sign = '-', offset = -offset;
        else
            sign = '+';
        sv_catpvf(dsv, "%c%02d:%02d", sign, offset / 60, offset % 60);
    }
}

SV *
THX_moment_strftime(pTHX_ const moment_t *mt, const char *s, STRLEN len) {
    const char *e, *p;
    SV *dsv;
    dt_t dt;
    int year, month, day, width;

    dsv = sv_2mortal(newSV(16));
    SvCUR_set(dsv, 0);
    SvPOK_only(dsv);

    dt = moment_local_dt(mt);
    dt_to_ymd(dt, &year, &month, &day);

    e = s + len;
    while (s < e) {
        p = (const char *)memchr(s, '%', e - s);
        if (p == NULL || p + 1 == e)
            p = e;
        sv_catpvn(dsv, s, p - s);
        if (p == e)
            break;

        width = 6;
        s = p;

      label:
        ++s;
        switch (*s) {
            case 'a':
                sv_catpv(dsv, aDoW[dt_dow(dt) - 1]);
                break;
            case 'A':
                sv_catpv(dsv, fDoW[dt_dow(dt) - 1]);
                break;
            case 'h':
            case 'b':
                sv_catpv(dsv, aMonth[month - 1]);
                break;
            case 'B':
                sv_catpv(dsv, fMonth[month - 1]);
                break;
            case 'c':
                sv_catpvf(dsv, "%04d-%02d-%02dT%02d:%02d:%02d",
                          year,
                          month,
                          day,
                          moment_hour(mt),
                          moment_minute(mt),
                          moment_second(mt),
                          moment_microsecond(mt));
                if (moment_microsecond(mt)) {
                    int us = moment_microsecond(mt);
                    if (us < 1000)
                        sv_catpvf(dsv, ".%03d", us);
                    else
                        sv_catpvf(dsv, ".%06d", us);
                }
                THX_format_Z(aTHX_ mt, dsv);
                break;
            case 'C':
                sv_catpvf(dsv, "%02d", year / 100);
                break;
            case 'd':
                sv_catpvf(dsv, "%02d", day);
                break;
            case 'D':
                sv_catpvf(dsv, "%02d/%02d/%02d", month, day, year % 100);
                break;
            case 'e':
                sv_catpvf(dsv, "%2d", day);
                break;
            case 'f':
                THX_format_f(aTHX_ mt, dsv, width);
                break;
            case 'F':
            case 'x':
                sv_catpvf(dsv, "%04d-%02d-%02d", year, month, day);
                break;
            case 'g':
                sv_catpvf(dsv, "%02d", dt_yow(dt) % 100);
                break;
            case 'G':
                sv_catpvf(dsv, "%d", dt_yow(dt));
                break;
            case 'H':
                sv_catpvf(dsv, "%02d", moment_hour(mt));
                break;
            case 'I':
                sv_catpvf(dsv, "%02d", moment_hour_12(mt));
                break;
            case 'j':
                sv_catpvf(dsv, "%03d", dt_doy(dt));
                break;
            case 'm':
                sv_catpvf(dsv, "%02d", month);
                break;
            case 'M':
                sv_catpvf(dsv, "%02d", moment_minute(mt));
                break;
            case 'n':
                sv_catpvn(dsv, "\n", 1);
                break;
            case 'N':
                THX_format_f(aTHX_ mt, dsv, width);
                break;
            case 'p':
                sv_catpv(dsv, moment_hour_meridiem(mt));
                break;
            case 'r':
                sv_catpvf(dsv, "%02d:%02d:%02d %s",
                          moment_hour_12(mt),
                          moment_minute(mt),
                          moment_second(mt),
                          moment_hour_meridiem(mt));
                break;
            case 'R':
                sv_catpvf(dsv, "%02d:%02d",
                          moment_hour(mt),
                          moment_minute(mt));
                break;
            case 's':
                THX_format_s(aTHX_ mt, dsv);
                break;
            case 'S':
                sv_catpvf(dsv, "%02d", moment_second(mt));
                break;
            case 't':
                sv_catpvn(dsv, "\t", 1);
                break;
            case 'X':
            case 'T':
                sv_catpvf(dsv, "%02d:%02d:%02d",
                          moment_hour(mt),
                          moment_minute(mt),
                          moment_second(mt));
                break;
            case 'u':
                sv_catpvf(dsv, "%d", dt_dow(dt));
                break;
            case 'U':
                sv_catpvf(dsv, "%02d", dt_week_number_sun(dt));
                break;
            case 'V':
                sv_catpvf(dsv, "%02d", dt_woy(dt));
                break;
            case 'w':
                sv_catpvf(dsv, "%d", dt_dow(dt) % 7);
                break;
            case 'W':
                sv_catpvf(dsv, "%02d", dt_week_number_mon(dt));
                break;
            case 'y':
                sv_catpvf(dsv, "%02d", year % 100);
                break;
            case 'Y':
                sv_catpvf(dsv, "%d", year);
                break;
            case 'z':
                THX_format_z(aTHX_ mt, dsv);
                break;
            case 'Z':
                THX_format_Z(aTHX_ mt, dsv);
                break;
            case '%':
                sv_catpvn(dsv, "%", 1);
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                if (s + 1 <= e && (s[1] == 'f' || s[1] == 'N')) {
                    width = *s - '0';
                    goto label;
                }
                /* FALLTROUGH */
            default:
                sv_catpvn(dsv, p, s - p + 1);
                break;
        }
        s++;
    }
    return dsv;
}

SV *
THX_moment_to_string(pTHX_ const moment_t *mt) {
    SV *dsv;
    dt_t dt;
    int year, month, day, sec, us, offset, sign;

    dsv = sv_2mortal(newSV(16));
    SvCUR_set(dsv, 0);
    SvPOK_only(dsv);

    dt = moment_local_dt(mt);
    dt_to_ymd(dt, &year, &month, &day);

    sv_catpvf(dsv, "%04d-%02d-%02dT%02d:%02d",
        year, month, day, moment_hour(mt), moment_minute(mt));

    sec = moment_second(mt);
    us  = moment_microsecond(mt);
    if (sec || us) {
        sv_catpvf(dsv, ":%02d", sec);
        if (us) {
            if (us < 1000)
                sv_catpvf(dsv, ".%03d", us);
            else
                sv_catpvf(dsv, ".%06d", us);
        }
    }

    offset = moment_offset(mt);
    if (offset == 0)
        sv_catpvn(dsv, "Z", 1);
    else {
        if (offset < 0)
            sign = '-', offset = -offset;
        else
            sign = '+';

        if ((offset % 60) == 0)
            sv_catpvf(dsv, "%c%02d", sign, offset / 60);
        else
            sv_catpvf(dsv, "%c%02d:%02d", sign, offset / 60, offset % 60);
    }

    return dsv;
}

