/*
 * Print.cpp - Base class that provides print() and println()
 * Copyright (c) 2008 David A. Mellis.  All right reserved.
 * Copyright (c) 2011 LeafLabs, LLC.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Modified 23 November 2006 by David A. Mellis
 * Modified 12 April 2011 by Marti Bolivar <mbolivar@leaflabs.com>
 */

#include "Print.h"
#include "wirish_math.h"
#include "limits.h"

#ifndef LLONG_MAX
/*
 * Note:
 *
 * At time of writing (12 April 2011), the limits.h that came with the
 * newlib we distributed didn't include LLONG_MAX.  Because we're
 * staying away from using templates (see /notes/coding_standard.rst,
 * "Language Features and Compiler Extensions"), this value was
 * copy-pasted from a println() of the value
 *
 *     std::numeric_limits<long long>::max().
 */
#define LLONG_MAX 9223372036854775807LL
#endif

/*
 * Public methods
 */

size_t Print::write(const char *str)
{
    size_t n = 0;
    while (*str)
    {
        write(*str++);
        n++;
    }
    return n;
}

size_t Print::write(const void *buffer, uint32_t size)
{
    size_t n = 0;
    uint8 *ch = (uint8*)buffer;
    while (size--)
    {
        write(*ch++);
        n++;
    }
    return n;
}

size_t Print::print(uint8 b, int base)
{
    return print((uint64)b, base);
}

size_t Print::print(const String &s)
{
    return write(s.c_str(), s.length());
}

size_t Print::print(char c)
{
    return write(c);
}

size_t Print::print(const char str[])
{
    return write(str);
}

size_t Print::print(int n, int base)
{
    return print((long long)n, base);
}

size_t Print::print(unsigned int n, int base)
{
    return print((unsigned long long)n, base);
}

size_t Print::print(long n, int base)
{
    return print((long long)n, base);
}

size_t Print::print(unsigned long n, int base)
{
    return print((unsigned long long)n, base);
}

size_t Print::print(long long n, int base)
{
    if (base == BYTE)
    {
        return write((uint8)n);
    }
    if (n < 0)
    {
        print('-');
        n = -n;
    }
    return printNumber(n, base);
}

size_t Print::print(unsigned long long n, int base)
{
    size_t c = 0;
    if (base == BYTE)
    {
        c = write((uint8)n);
    }
    else
    {
        c = printNumber(n, base);
    }
    return c;
}

size_t Print::print(double n, int digits)
{
    return printFloat(n, digits);
}

size_t Print::print(const __FlashStringHelper *ifsh)
{
    return print(reinterpret_cast<const char *>(ifsh));
}

size_t Print::print(const Printable& x)
{
    return x.printTo(*this);
}

size_t Print::println(void)
{
    size_t n =  print('\r');
    n += print('\n');
    return n;
}

size_t Print::println(const String &s)
{
    size_t n = print(s);
    n += println();
    return n;
}

size_t Print::println(char c)
{
    size_t n = print(c);
    n += println();
    return n;
}

size_t Print::println(const char c[])
{
    size_t n = print(c);
    n += println();
    return n;
}

size_t Print::println(uint8 b, int base)
{
    size_t n = print(b, base);
    n += println();
    return n;
}

size_t Print::println(int n, int base)
{
    size_t s = print(n, base);
    s += println();
    return s;
}

size_t Print::println(unsigned int n, int base)
{
    size_t s = print(n, base);
    s += println();
    return s;
}

size_t Print::println(long n, int base)
{
    size_t s = print((long long)n, base);
    s += println();
    return s;
}

size_t Print::println(unsigned long n, int base)
{
    size_t s = print((unsigned long long)n, base);
    s += println();
    return s;
}

size_t Print::println(long long n, int base)
{
    size_t s = print(n, base);
    s += println();
    return s;
}

size_t Print::println(unsigned long long n, int base)
{
    size_t s = print(n, base);
    s += println();
    return s;
}

size_t Print::println(double n, int digits)
{
    size_t s = print(n, digits);
    s += println();
    return s;
}

size_t Print::println(const __FlashStringHelper *ifsh)
{
    size_t n = print(ifsh);
    n += println();
    return n;
}

size_t Print::println(const Printable& x)
{
    size_t n = print(x);
    n += println();
    return n;
}

#ifdef PRINT_SUPPORTS_PRINTF

#define PAD_RIGHT 1
#define PAD_ZERO 2

void Print::xprintchar(char **str, int c)
{
    if (str)
    {
        **str = (char)c;
        ++(*str);
    }
    else
    {
        write(c);
    }
}

int Print::xprints(char **out, const char *string, int width, int pad)
{
    int pc = 0, padchar = ' ';

    if (width > 0)
    {
        int len = 0;
        const char *ptr;
        for (ptr = string; *ptr; ++ptr) ++len;
        if (len >= width) width = 0;
        else width -= len;
        if (pad & PAD_ZERO) padchar = '0';
    }
    if (!(pad & PAD_RIGHT))
    {
        for ( ; width > 0; --width)
        {
            xprintchar (out, padchar);
            ++pc;
        }
    }
    for ( ; *string ; ++string)
    {
        xprintchar (out, *string);
        ++pc;
    }
    for ( ; width > 0; --width)
    {
        xprintchar (out, padchar);
        ++pc;
    }

    return pc;
}

int Print::xprinti(char **out, int i, int b, int sg, int width, int pad, int letbase)
{
    /* the following should be enough for 32 bit int */
    #define PRINT_BUF_LEN 12
    char print_buf[PRINT_BUF_LEN];
    char *s;
    int t, neg = 0, pc = 0;
    unsigned int u = (unsigned int)i;

    if (i == 0)
    {
        print_buf[0] = '0';
        print_buf[1] = '\0';
        return xprints (out, print_buf, width, pad);
    }

    if (sg && b == 10 && i < 0)
    {
        neg = 1;
        u = (unsigned int) - i;
    }

    s = print_buf + PRINT_BUF_LEN - 1;
    *s = '\0';

    while (u)
    {
        t = (unsigned int)u % b;
        if( t >= 10 )
            t += letbase - '0' - 10;
        *--s = (char)(t + '0');
        u /= b;
    }

    if (neg)
    {
        if( width && (pad & PAD_ZERO) )
        {
            xprintchar (out, '-');
            ++pc;
            --width;
        }
        else
        {
            *--s = '-';
        }
    }

    return pc + xprints (out, s, width, pad);
}

int Print::xprint( char **out, const char *format, va_list args)
{
    int width, pad;
    int pc = 0;
    char scr[2];

    for (; *format != 0; ++format)
    {
        if (*format == '%')
        {
            ++format;
            width = pad = 0;
            if (*format == '\0') break;
            if (*format == '%') goto out;
            if (*format == '-')
            {
                ++format;
                pad = PAD_RIGHT;
            }
            while (*format == '0')
            {
                ++format;
                pad |= PAD_ZERO;
            }
            for ( ; *format >= '0' && *format <= '9'; ++format)
            {
                width *= 10;
                width += *format - '0';
            }
            if( *format == 's' )
            {
                char *s = (char *)va_arg( args, int );
                pc += xprints (out, s ? s : "(null)", width, pad);
                continue;
            }
            if( *format == 'd' )
            {
                pc += xprinti (out, va_arg( args, int ), 10, 1, width, pad, 'a');
                continue;
            }
            if( *format == 'x' )
            {
                pc += xprinti (out, va_arg( args, int ), 16, 0, width, pad, 'a');
                continue;
            }
            if( *format == 'X' )
            {
                pc += xprinti (out, va_arg( args, int ), 16, 0, width, pad, 'A');
                continue;
            }
            if( *format == 'u' )
            {
                pc += xprinti (out, va_arg( args, int ), 10, 0, width, pad, 'a');
                continue;
            }
            if( *format == 'c' )
            {
                /* char are converted to int then pushed on the stack */
                scr[0] = (char)va_arg( args, int );
                scr[1] = '\0';
                pc += xprints (out, scr, width, pad);
                continue;
            }
        }
        else
        {
out:
            xprintchar (out, *format);
            ++pc;
        }
    }
    if (out) **out = '\0';
    va_end( args );
    return pc;
}

int Print::printf (const char *__restrict __format, ...)
{
    va_list args;
    va_start(args, __format);
    return xprint(0, __format, args);
}

int Print::sprintf(char *out, const char *format, ...)
{
    va_list args;

    va_start( args, format );
    return xprint(&out, format, args );
}

#endif

/*
 * Private methods
 */
size_t Print::printNumber(unsigned long long n, uint8 base)
{
    unsigned char buf[CHAR_BIT * sizeof(long long)];
    unsigned long i = 0;
    size_t s = 0;
    if (n == 0)
    {
        print('0');
        return 1;
    }

    while (n > 0)
    {
        buf[i++] = n % base;
        n /= base;
    }

    for (; i > 0; i--)
    {
        s += print((char)(buf[i - 1] < 10 ?
                          '0' + buf[i - 1] :
                          'A' + buf[i - 1] - 10));
    }
    return s;
}


/* According to snprintf(),
 *
 * nextafter((double)numeric_limits<long long>::max(), 0.0) ~= 9.22337e+18
 *
 * This slightly smaller value was picked semi-arbitrarily. */
#define LARGE_DOUBLE_TRESHOLD (9.1e18)

/* THIS FUNCTION SHOULDN'T BE USED IF YOU NEED ACCURATE RESULTS.
 *
 * This implementation is meant to be simple and not occupy too much
 * code size.  However, printing floating point values accurately is a
 * subtle task, best left to a well-tested library function.
 *
 * See Steele and White 2003 for more details:
 *
 * http://kurtstephens.com/files/p372-steele.pdf
 */
size_t Print::printFloat(double number, uint8 digits)
{
    size_t s = 0;
    // Hackish fail-fast behavior for large-magnitude doubles
    if (abs(number) >= LARGE_DOUBLE_TRESHOLD)
    {
        if (number < 0.0)
        {
            s = print('-');
        }
        s += print("<large double>");
        return s;
    }

    // Handle negative numbers
    if (number < 0.0)
    {
        s += print('-');
        number = -number;
    }

    // Simplistic rounding strategy so that e.g. print(1.999, 2)
    // prints as "2.00"
    double rounding = 0.5;
    for (uint8 i = 0; i < digits; i++)
    {
        rounding /= 10.0;
    }
    number += rounding;

    // Extract the integer part of the number and print it
    long long int_part = (long long)number;
    double remainder = number - int_part;
    s += print(int_part);

    // Print the decimal point, but only if there are digits beyond
    if (digits > 0)
    {
        s += print(".");
    }

    // Extract digits from the remainder one at a time
    while (digits-- > 0)
    {
        remainder *= 10.0;
        int to_print = (int)remainder;
        s += print(to_print);
        remainder -= to_print;
    }
    return s;
}
