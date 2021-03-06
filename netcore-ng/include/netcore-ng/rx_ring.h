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


#ifndef _NET_RX_RING_H_
#define _NET_RX_RING_H_

#include <netcore-ng/types.h> 
#include <netcore-ng/thread.h> 
#include <netcore-ng/bpf.h> 
#include <netcore-ng/generic.h> 

/* a rx ring must only belong to one entity */
struct netsniff_ng_rx_nic_context
{
	struct generic_nic_context		generic;
	struct ring_buff			nic_rb;
};

struct netsniff_ng_rx_thread_context
{
	struct netsniff_ng_thread_context	thread_ctx;
	struct netsniff_ng_rx_nic_context	nic_ctx;
};

/* Function signatures */
extern struct netsniff_ng_rx_thread_context * rx_thread_create(const cpu_set_t run_on, const int sched_prio, const int sched_policy, const char * const rx_dev, const char * const bpf_path, const char * const pcap_path);
extern void rx_thread_destroy(struct netsniff_ng_rx_thread_context * thread_config);


#define DEFAULT_RX_RING_SILENT_MESSAGE "Receive ring dumping ... |"

#endif				/* _NET_RX_RING_H_ */
