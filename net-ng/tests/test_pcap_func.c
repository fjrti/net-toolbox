#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <net-ng/pcap.h>
#include <net-ng/macros.h>

static const uint8_t icmp_dns[] =
{ 0x00, 0x1e, 0x65, 0x93, 0x1b, 0x6c, 0x00, 0x1d, 0x19, 0x84, 0x9c, 0xdc, 0x08, 
  0x00, 0x45, 0x00, 0x00, 0x54, 0xdb, 0x46, 0x00, 0x00, 0x38, 0x01, 0x4d, 0x41, 
  0x08, 0x08, 0x08, 0x08, 0xc0, 0xa8, 0x89, 0x69, 0x00, 0x00, 0xce, 0x1a, 0x12, 
  0x2d, 0x00, 0x02, 0xb7, 0xeb, 0xba, 0x4c, 0x00, 0x00, 0x00, 0x00, 0xee, 0xaa, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 
  0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 
  0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 
  0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 };

static const char test_path[] = "res.pcap";

int test_pcap_write(const int fd, const uint8_t * const payload, const size_t len)
{
	struct tpacket_hdr header = { 0 };
	struct timeval ts;

	gettimeofday(&ts, NULL);

	header.tp_sec = ts.tv_sec;
	header.tp_usec = ts.tv_usec;
	header.tp_snaplen = len;
	header.tp_len = len;
		
	if (pcap_write_payload(fd, &header, (const struct ethhdr *)payload) != len)
	{
		return (-1);
	}

	return (0);
}

int main (void)
{
	int ret;
	int fd;
	int rc = EXIT_FAILURE;

	/* Create PCAP file */
	if ((fd = pcap_create(test_path)) < 0)
	{
		err("Cannot create pcap");
		goto out;
	}

	/* Write payload */
	if (test_pcap_write(fd, icmp_dns, sizeof(icmp_dns)))
	{
		err("Error when writing payload");
		goto out;
	}

	if (pcap_close(fd))
	{
		err("Cannot close pcap");
		goto out;
	}

	if ((fd = pcap_open(test_path, O_RDONLY)) < 0)
	{
		err("Cannot open pcap");
		goto out;
	}

	ret = pcap_has_packets(fd);
	if (ret <= 0)
	{
		err("pcap should have packet");
		goto out;
	}

	if (pcap_close(fd))
	{
		err("Cannot close pcap");
		goto out;
	}

	if ((fd = pcap_open(test_path, O_RDWR | O_APPEND)) < 0)
	{
		err("Cannot open/append pcap");
		goto out;
	}

	/* Write payload */
	if (test_pcap_write(fd, icmp_dns, sizeof(icmp_dns)))
	{
		err("Error when writing payload");
		goto out;
	}

	if (pcap_close(fd))
	{
		err("Cannot close pcap");
		goto out;
	}

	rc = EXIT_SUCCESS;
out:

	pcap_destroy(fd, test_path);

	return (rc);
}
