#ifndef	__ETH_DRIVE_H__
#define __ETH_DRIVE_H__
#include "ETH_HAL.h"

UINT _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr);
UINT	_nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr);
UINT	_nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr);
UINT	_nx_driver_hardware_multicast_join(NX_IP_DRIVER *driver_req_ptr);
UINT	_nx_driver_hardware_multicast_leave(NX_IP_DRIVER *driver_req_ptr);
UINT _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr);

#endif

