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

#include <assert.h>
#include <netcore-ng/macros.h>
#include <netcore-ng/dissector/ethernet/dissector.h>

static hi_handle_t * ethernet_dissector_hash;

int ethernet_dissector_display_set(const enum display_type dtype)
{
	void *key_ptr = NULL;
	hi_iterator_t * it = NULL;
	struct protocol_dissector * dis = NULL;
	uint32_t keylen = 0;
	int rc;

	if ((rc = hi_iterator_create(ethernet_dissector_hash, &it)) != 0)
	{
		err("Could not create iterator\n");
		return (EAGAIN);
	}

	while (hi_iterator_getnext(it, (void **) &dis, &key_ptr, &keylen) == 0 && dis != NULL)
	{
		dis->display_set(dtype);
	}

	hi_iterator_fini(it);

	return (0);
}

int ethernet_dissector_insert(const struct protocol_dissector * const dis)
{
	int rc = 0;
	assert(dis);

	if ((rc = hi_insert_uint16_t(ethernet_dissector_hash, dis->key, dis)) != 0)
	{
		return (rc);
	}

	//info("Added dissector %p with key %x\n", (void *)dis, dis->key);

	return (0);
}

int ethernet_dissector_run(uint8_t * pkt, size_t len)
{
	size_t off = 0;
	size_t read = 0;
	uint16_t key = ETHERNET_HDR_DEFAULT_KEY;
	struct protocol_dissector * dis = NULL;

	assert(pkt);
	assert(len);

	while (hi_get_uint16_t(ethernet_dissector_hash, key, (void **)&dis) == 0)
	{
		if (dis->display)
			read = dis->display(pkt, len, off);

		if (dis->get_next_key == NULL)
		{
			break;
		}

		key = dis->get_next_key(pkt, len, off);
		off += read;
	}

	return (0);
}

void ethernet_dissector_destroy(void)
{
	oui_hash_destroy();
	ether_types_hash_destroy();
	udp_ports_hash_destroy();
	tcp_ports_hash_destroy();
	hi_fini(ethernet_dissector_hash);
}

int ethernet_dissector_init(const enum display_type dtype)
{
	int rc = 0;

	if ((rc = tcp_ports_hash_init()) != 0)
	{
		goto error;
	}

	if ((rc = udp_ports_hash_init()) != 0)
	{
		goto error;
	}

	if ((rc = ether_types_hash_init()) != 0)
	{
		goto error;
	}

	if ((rc = oui_hash_init()) != 0)
	{
		goto error;
	}

	if ((rc = hi_init_uint16_t(&ethernet_dissector_hash, DISSECTOR_MAX)) != 0)
	{
		goto error;
	}

	if ((rc = dissector_raw_insert(ethernet_dissector_insert)) != 0)
	{
		goto error;
	}

	if ((rc = dissector_ethernet_insert()) != 0)
	{
		goto error;
	}

	if ((rc = dissector_arp_insert()) != 0)
	{
		goto error;
	}

	if ((rc = dissector_ip_insert()) != 0)
	{
		goto error;
	}

	if ((rc = dissector_tcphdr_insert()) != 0)
	{
		goto error;
	}

	if ((rc = dissector_icmp_insert()) != 0)
	{
		goto error;
	}

	if ((rc = ethernet_dissector_display_set(dtype)) != 0)
	{
		goto error;
	}

	return (0);
error:
	ethernet_dissector_destroy();
	return (rc);
}

