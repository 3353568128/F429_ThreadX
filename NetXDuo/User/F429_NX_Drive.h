#ifndef	__F429_NX_DRIVE_H__
#define __F429_NX_DRIVE_H__
#include "ETH_Drive.h"

#define NX_DRIVER_ERROR                         90
//MTU¥Û–°
#define NX_DRIVER_ETHERNET_MTU                  1514
#define NX_DRIVER_ETHERNET_FRAME_SIZE           14

#define NX_DRIVER_ETHERNET_IP                   0x0800
#define NX_DRIVER_ETHERNET_IPV6                 0x86dd
#define NX_DRIVER_ETHERNET_ARP                  0x0806
#define NX_DRIVER_ETHERNET_RARP                 0x8035


VOID  nx_driver_stm32f429(NX_IP_DRIVER *driver_req_ptr);

VOID	_nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr);
VOID	_nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr);
VOID	_nx_driver_enable(NX_IP_DRIVER *driver_req_ptr);
VOID	_nx_driver_disable(NX_IP_DRIVER *driver_req_ptr);
VOID	_nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr);
VOID	_nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr);
VOID	_nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr);

VOID	_nx_driver_packet_transmitted(VOID);
VOID	_nx_driver_packet_received(VOID);
void  _nx_F429_recive_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr);
#endif

