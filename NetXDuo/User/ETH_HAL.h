#ifndef	__ETH_HAL_H__
#define __ETH_HAL_H__
#include "main.h"
#include "tx_api.h"
#include "nx_api.h"

#define F429_NX_TXDESC_COUNT   128   
#define F429_NX_RXDESC_COUNT   16

extern ETH_HandleTypeDef HETH;
extern uint8_t  ETH_MAC_Address[6];
extern ETH_DMADescTypeDef  ETH_DMATxDescTab[F429_NX_TXDESC_COUNT];		//ETH发送描述符
extern ETH_DMADescTypeDef  ETH_DMARxDescTab[F429_NX_RXDESC_COUNT];		//ETH接收描述符


typedef struct _F429_NX_DRIVE_INFOMATION
{
	NX_PACKET_POOL  *nx_driver_nx_packet_pool_ptr;							//保存 nx_pack 池指针，供发送完成后，重新给描述符分配buff时用
	NX_INTERFACE 		*nx_ip_driver_interface_ptr;								//保存在使用的 NX 驱动实例，供 _nx_F429_recive_to_netx 使用
	NX_IP        		*nx_driver_ip_ptr;
	
	ULONG         	nx_driver_rx_nx_pack_size;									//记录RX_NX_PACK大小
	UINT						receive_current_index;											//指示当前接收描述符的指针
	NX_PACKET     	*nx_packets_receive[F429_NX_RXDESC_COUNT];	//保存从内存池分配的RX结构体数组，描述符列表仅保存了buff地址，其他信息未保存
	UINT						send_current_index;
	UINT						send_release_index;
	UINT						send_in_use_index;	
	NX_PACKET     	*nx_packets_send[F429_NX_TXDESC_COUNT];		
} F429_NX_DRIVE_INFOMATION;


void ETH_Init();
HAL_StatusTypeDef ETH_DMATxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMATxDescTab, uint32_t TxBuffCount);
HAL_StatusTypeDef ETH_DMARxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMARxDescTab, NX_PACKET_POOL *RecPackPool, uint32_t RxBuffCount);
HAL_StatusTypeDef ETH_TransmitFrame(ETH_HandleTypeDef *heth, NX_PACKET *packet_ptr);
#endif

