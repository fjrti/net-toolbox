#ifndef	__ETHERNET_DISSECTOR_ETHERNET_H__
#define	__ETHERNET_DISSECTOR_ETHERNET_H__

#include <netcore-ng/dissector/ethernet/dissector.h>

int dissector_ethernet_insert(void);
int dissector_ethernet_print_set(const enum display_type);

#endif	/* __ETHERNET_DISSECTOR_ETHERNET_H__ */