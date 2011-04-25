#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <netcore-ng/packet_vector.h>

void packet_context_destroy(struct packet_ctx * pkt_ctx)
{
	assert(pkt_ctx);

	free(pkt_ctx->buf);
	memset(pkt_ctx, 0, sizeof(*pkt_ctx));
}

int packet_context_create(struct packet_ctx * pkt_ctx, const size_t mtu)
{
	assert(pkt_ctx);
	assert(mtu);

	memset(pkt_ctx, 0, sizeof(*pkt_ctx));

	if ((pkt_ctx->buf = calloc(mtu, sizeof(*pkt_ctx->buf))) == NULL)
	{
		return (ENOMEM);
	}

	pkt_ctx->mtu = mtu;

	return (0);
}

void packet_vector_destroy(struct packet_vector * pkt_vec)
{
	size_t a;

	assert(pkt_vec);

	for (a = 0; a < pkt_vec->total_pkt_nr; a++)
	{
		packet_context_destroy(&pkt_vec->pkt[a]);
	}

	free(pkt_vec->pkt_io_vec);
	free(pkt_vec->pkt);

	memset(pkt_vec, 0, sizeof(*pkt_vec));
}

int packet_vector_create(struct packet_vector * pkt_vec, const size_t total_pkt_nr, const size_t mtu)
{
	size_t a;
	int rc = 0;

	assert(pkt_vec);
	assert(total_pkt_nr);
	assert(mtu);

	memset(pkt_vec, 0, sizeof(*pkt_vec));

	/* One vector for the PCAP header, one for the packet itself */
	pkt_vec->total_pkt_io_vec = total_pkt_nr * 2;
	pkt_vec->total_pkt_nr = total_pkt_nr;

	pkt_vec->pkt = calloc(total_pkt_nr, sizeof(*pkt_vec->pkt));
	pkt_vec->pkt_io_vec = calloc(pkt_vec->total_pkt_io_vec, sizeof(*pkt_vec->pkt_io_vec));

	if (pkt_vec->pkt == NULL || pkt_vec->pkt_io_vec == NULL)
	{
		rc = ENOMEM;
		goto error;
	}

	for (a = 0; a < pkt_vec->total_pkt_nr; a++)
	{
		if ((rc = packet_context_create(&pkt_vec->pkt[a], mtu)) != 0)
		{
			goto error;
		}
	}

	return (0);

error:
	packet_vector_destroy(pkt_vec);
	return (rc);
}

void packet_vector_reset(struct packet_vector * pkt_vec)
{
	memset(pkt_vec->pkt_io_vec, 0, sizeof(*pkt_vec->pkt_io_vec) * pkt_vec->total_pkt_io_vec);
	
	pkt_vec->used_pkt_io_vec = 0;
	pkt_vec->used_pkt_nr = 0;
}

int packet_vector_is_full(const struct packet_vector * const pkt_vec)
{
	assert(pkt_vec);
	return (pkt_vec->used_pkt_nr >= pkt_vec->total_pkt_nr);
}

struct packet_ctx * packet_vector_packet_context_get(const struct packet_vector * const pkt_vec)
{
	if (packet_vector_is_full(pkt_vec))
		return (NULL);

	return (&pkt_vec->pkt[pkt_vec->used_pkt_nr]);
}

int packet_vector_next(struct packet_vector * pkt_vec)
{
	if (packet_vector_is_full(pkt_vec))
		return (EAGAIN);

	pkt_vec->used_pkt_nr++;
	pkt_vec->used_pkt_io_vec += 2;

	return (0);
}

void packet_vector_set(struct packet_vector * pkt_vec, struct packet_ctx * pkt_ctx)
{
	assert(pkt_vec);
	assert(pkt_ctx);

	pkt_vec->pkt_io_vec[pkt_vec->used_pkt_io_vec].iov_base = &pkt_ctx->pcap_hdr;
	pkt_vec->pkt_io_vec[pkt_vec->used_pkt_io_vec].iov_len = sizeof(pkt_ctx->pcap_hdr);
	pkt_vec->pkt_io_vec[pkt_vec->used_pkt_io_vec + 1].iov_base = pkt_ctx->buf;
	pkt_vec->pkt_io_vec[pkt_vec->used_pkt_io_vec + 1].iov_len = pkt_ctx->len;
}