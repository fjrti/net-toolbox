/* __LICENSE_HEADER_BEGIN__ */

/*
 * Copyright (C) 2009-2011	Emmanuel Roullit <emmanuel@netsniff-ng.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA 02110, USA
 *
 */

 /* __LICENSE_HEADER_END__ */


#ifndef _NET_MACROS_H_
#define _NET_MACROS_H_

/* Standardized info/warning/error printing routines that should be used source-wide */
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define min(x,y) x < y ? x : y

#define max(x,y) x > y ? x : y

#define info(...)                                             \
                                fprintf(stdout, __VA_ARGS__)

#define warn(...)                                                           \
                                do {                                        \
                                        fprintf(stderr, "W: " __VA_ARGS__); \
                                        fflush(stderr);                     \
                                } while(0)

/* Do not a perror() after this and do not end string with '\n'! */
#define err(...)                                                                    \
                                do {                                                \
                                        fprintf(stderr, "E: %s,%u: ", __FUNCTION__, __LINE__);         \
                                        fprintf(stderr, __VA_ARGS__);         \
                                        fprintf(stderr, ": %s\n", strerror(errno)); \
                                        fflush(stderr);                             \
                                } while(0)

/* Calc macros for Byte conversion */
#define DIV_KBYTES(x)           ((x) / (1024LLU))
#define DIV_MBYTES(x)           ((x) / (1048576LLU))
#define DIV_GBYTES(x)           ((x) / (1073741824LLU))

#define DIV_US2HOURS(x)         ((x) / (3600000000LLU))
#define MOD_HOURS2US(x)         ((x) % (3600000000LLU))
#define DIV_US2MINUT(x)         ((x) / (60000000LLU))
#define MOD_MINUT2US(x)         ((x) % (60000000LLU))
#define DIV_US2SECON(x)         ((x) / (1000000LLU))
#define MOD_SECON2US(x)         ((x) % (1000000LLU))
#define DIV_US2MILLI(x)         ((x) / (1000LLU))
#define MOD_MILLI2US(x)         ((x) % (1000LLU))

#define DIV_S2DAYS(x)           ((x) / (86400LLU))
#define MOD_DAYS2S(x)           ((x) % (86400LLU))
#define DIV_S2HOURS(x)          ((x) / (3600LLU))
#define MOD_HOURS2S(x)          ((x) % (3600LLU))
#define DIV_S2MINUT(x)          ((x) / (60LLU))
#define MOD_MINUT2S(x)          ((x) % (60LLU))

/* Some preprocessor terminal color related stuff */
#define __reset                 "0"
#define __bold                  "1"

#define __black                 "30"
#define __red                   "31"
#define __green                 "32"
#define __yellow                "33"
#define __blue                  "34"
#define __magenta               "35"
#define __cyan                  "36"
#define __white                 "37"
#define __on_black              "40"
#define __on_red                "41"
#define __on_green              "42"
#define __on_yellow             "43"
#define __on_blue               "44"
#define __on_magenta            "45"
#define __on_cyan               "46"
#define __on_white              "47"

/* Some preprocessor hacks */
#define colorize_start(fore)                 "\033[" __##fore "m"
#define colorize_start_full(fore, back)      "\033[" __##fore ";" __on_##back "m"
#define colorize_end()                       "\033[" __reset "m"

#define colorize_str(fore, text)  	     colorize_start(fore) text colorize_end()
#define colorize_full_str(fore, back, text)  colorize_start_full(fore, back) text colorize_end()

#endif				/* _NET_MACROS_H_ */
