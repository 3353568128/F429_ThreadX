#include "ETH_Drive.h"

UINT _nx_driver_hardware_initialize(NX_IP_DRIVER *driver_req_ptr)
{
	ETH_Init();
	ETH_DMATxDescListInit(&HETH, ETH_DMATxDescTab, F429_NX_TXDESC_COUNT);
	if(ETH_DMARxDescListInit(&HETH, ETH_DMARxDescTab, driver_req_ptr->nx_ip_driver_ptr->nx_ip_default_packet_pool, F429_NX_RXDESC_COUNT) != HAL_OK)
		return HAL_ERROR;
	return HAL_OK;
}

UINT	_nx_driver_hardware_enable(NX_IP_DRIVER *driver_req_ptr)
{
    HAL_ETH_Start(&HETH);
    __HAL_ETH_DMA_ENABLE_IT(&HETH, ETH_DMA_IT_NIS | ETH_DMA_IT_R | ETH_DMA_IT_T);
    return(NX_SUCCESS);
}

UINT	_nx_driver_hardware_disable(NX_IP_DRIVER *driver_req_ptr)
{
	HAL_ETH_Stop(&HETH);
	return(NX_SUCCESS);
}

UINT	_nx_driver_hardware_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{
	ETH->MACFFR |= ETH_MACFFR_PAM;
	return(NX_SUCCESS);
}
UINT	_nx_driver_hardware_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{
	ETH->MACFFR &= ~ETH_MACFFR_PAM;
	return(NX_SUCCESS);
}

UINT _nx_driver_hardware_packet_send(NX_PACKET *packet_ptr)
{
	/* 一个完整的帧，保存在 NX_PACK 链表中 */
	return ETH_TransmitFrame(&HETH, packet_ptr);
}