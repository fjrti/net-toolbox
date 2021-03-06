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


#ifndef _NET_BPF_H_
#define _NET_BPF_H_

#include <stdint.h>
#include <linux/filter.h>

extern void bpf_dump_all(const struct sock_fprog * const bpf);
extern int bpf_is_valid(const struct sock_fprog * const bpf);
extern uint32_t bpf_filter(const struct sock_fprog * const bpf, uint8_t * packet, size_t plen);
extern int bpf_parse(const char * const bpf_path, struct sock_fprog *bpf);

#endif				/* _NET_BPF_H_ */
