#ifndef	__ETH_HAL_H__
#define __ETH_HAL_H__
#include "main.h"
#include "tx_api.h"
#include "nx_api.h"

#define F429_NX_TXDESC_COUNT   128   
#define F429_NX_RXDESC_COUNT   16

extern ETH_HandleTypeDef HETH;
extern uint8_t  ETH_MAC_Address[6];
extern ETH_DMADescTypeDef  ETH_DMATxDescTab[F429_NX_TXDESC_COUNT];		//ETH����������
extern ETH_DMADescTypeDef  ETH_DMARxDescTab[F429_NX_RXDESC_COUNT];		//ETH����������


typedef struct _F429_NX_DRIVE_INFOMATION
{
	NX_PACKET_POOL  *nx_driver_nx_packet_pool_ptr;							//���� nx_pack ��ָ�룬��������ɺ����¸�����������buffʱ��
	NX_INTERFACE 		*nx_ip_driver_interface_ptr;								//������ʹ�õ� NX ����ʵ������ _nx_F429_recive_to_netx ʹ��
	NX_IP        		*nx_driver_ip_ptr;
	
	ULONG         	nx_driver_rx_nx_pack_size;									//��¼RX_NX_PACK��С
	UINT						receive_current_index;											//ָʾ��ǰ������������ָ��
	NX_PACKET     	*nx_packets_receive[F429_NX_RXDESC_COUNT];	//������ڴ�ط����RX�ṹ�����飬�������б��������buff��ַ��������Ϣδ����
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

