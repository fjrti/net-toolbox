#include <stdlib.h>
#include <string.h>

#include <netcore-ng/packet.h>

void packet_context_destroy(struct packet_ctx * pkt_ctx)
{
	assert(pkt_ctx);

	free(pkt_ctx->pkt_buf);
	memset(pkt_ctx, 0. sizeof(*pkt_ctx));

	return (0);
}

int packet_context_create(struct packet_ctx * pkt_ctx, const size_t mtu)
{
	assert(pkt_ctx);
	assert(mtu);

	memset(pkt_ctx, 0, sizeof(*pkt_ctx));

	if ((pkt_ctx->pkt_buf = malloc(sizeof(*pkt_ctx->pkt_buf) * mtu)) == NULL)
	{
		return (ENOMEM);
	}

	memset(pkt_ctx->pkt_buf, 0, sizeof(*pkt_ctx->pkt_buf) * mtu);
	pkt_ctx->mtu = pkt_mtu;

	return (0);
}

void packet_vector_destroy(struct packet_vector * pkt_vec)
{
	size_t a;
	int rc = 0;

	assert(pkt_vec);

	for (a = 0; a < pkt_vec->pkt_nr; a++)
	{
		packet_context_destroy(&pkt_vec->pkt[a]);
	}

	free(pkt_vec->pkt_io_vec);
	free(pkt_vec->pkt);

	memset(pkt_vec, 0, sizeof(*pkt_vec));
}

int packet_vector_create(struct packet_vector * pkt_vec, const size_t pkt_nr, const size_t mtu)
{
	size_t a;
	int rc = 0;
	size_t setup_pkt;

	assert(pkt_vec);
	assert(pkt_nr);
	assert(mtu);

	memset(pkt_vec, 0, sizeof(*pkt_vec));

	/* One vector for the PCAP header, one for the packet itself */
	pkt_vec->pkt_io_vec_nr = pkt_nr * 2;
	pkt_vec->pkt_nr = pkt_nr;

	pkt_vec->pkt = malloc(sizeof(*pkt_vec->pkt) * pkt_nr);
	pkt_vec->pkt_io_vec = malloc(sizeof(*pkt_vec->pkt_hdr_vec) * vector_nr);

	if (pkt_vec->pkt == NULL || pkt_vec->pkt_io_vec == NULL)
	{
		rc = ENOMEM;
		goto error;
	}

	memset(pkt_vec->pkt, 0, sizeof(*pkt_vec->pkt) * pkt_vec->pkt_nr);
	memset(pkt_vec->pkt_io_vec, 0, sizeof(*pkt_vec->pkt_io_vec) * pkt_vec->vector_nr);
	
	for (a = 0; a < pkt_vec->pkt_nr; a++)
	{
		if ((rc = packet_context_create(&pkt_vec->pkt[a])) != 0)
		{
			goto error;
		}
	}

	for (a = 0, setup_pkt = 0; a < pkt_vec->vector_nr; a++)
	{
		if (a % 2 == 0)
		{
			pkt_vec->pkt_io_vec[a].iov_base = &pkt_vec->pkt[setup_pkt].pkt_hdr;
			pkt_vec->pkt_hdr_vec[a].iov_len = sizeof(pkt_vec->pkt[setup_pkt].pkt_hdr);
		}
		else
		{
			pkt_vec->pkt_buf_vec[a].iov_base = pkt_vec->pkt[setup_pkt].pkt_buf;
			/* The ring routine must set the valid packet length in the IO vector */
			pkt_vec->pkt_buf_vec[a].iov_len = 0;

			/* At this point, the packet buffer has its PCAP header, so setup the next one*/
			setup_pkt++;
		}
	}

	return (0);

error:
	packet_vector_destroy(pkt_vec);
	return (rc);
}
