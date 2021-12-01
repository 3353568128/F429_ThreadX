#include "ETH_HAL.h"

ETH_HandleTypeDef HETH;
ETH_DMADescTypeDef  ETH_DMATxDescTab[F429_NX_TXDESC_COUNT];		//ETH����������
ETH_DMADescTypeDef  ETH_DMARxDescTab[F429_NX_RXDESC_COUNT];		//ETH����������
uint8_t  ETH_MAC_Address[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x56};

void ETH_Init()
{
	HETH.Instance = ETH;
  HETH.Init.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
  HETH.Init.Speed = ETH_SPEED_100M;
  HETH.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  HETH.Init.PhyAddress = DP83848_PHY_ADDRESS;
  HETH.Init.MACAddr = &ETH_MAC_Address[0];
  HETH.Init.RxMode = ETH_RXINTERRUPT_MODE;		//ѭ����� ��Ϊ �жϽ���
  HETH.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
  HETH.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;

  if (HAL_ETH_Init(&HETH) != HAL_OK)
  {
    Error_Handler();
  }
}

/* 1����ӷ������ʱ�ж�
 * 2��ע��Ϊ Buffer1Addr ���� buff
 * 3��ע�͹��� ETH_CHECKSUM_BY_HARDWARE �Ĵ���
 * 4����ʼ���������� nx_packets_send��ʹ�䶼ָ�� NX_NULL
 */
extern F429_NX_DRIVE_INFOMATION f429_nx_driver_information;
HAL_StatusTypeDef ETH_DMATxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMATxDescTab, uint32_t TxBuffCount)
{
  uint32_t i = 0U;
  ETH_DMADescTypeDef *dmatxdesc;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Set the DMATxDescToSet pointer with the first one of the DMATxDescTab list */
  heth->TxDesc = DMATxDescTab;
  
  /* Fill each DMATxDesc descriptor with the right values */   
  for(i=0U; i < TxBuffCount; i++)
  {
    /* Get the pointer on the ith member of the Tx Desc list */
    dmatxdesc = DMATxDescTab + i;
    
		dmatxdesc->Status = ETH_DMATXDESC_TCH | ETH_DMATXDESC_IC;
//    /* Set Second Address Chained bit */
//    dmatxdesc->Status = ETH_DMATXDESC_TCH;  
    
//    /* Set Buffer1 address pointer */
//    dmatxdesc->Buffer1Addr = (uint32_t)(&TxBuff[i*ETH_TX_BUF_SIZE]);
    
//    if ((heth->Init).ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)
//    {
//      /* Set the DMA Tx descriptors checksum insertion */
//      dmatxdesc->Status |= ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL;
//    }
    
    /* Initialize the next descriptor with the Next Descriptor Polling Enable */
    if(i < (TxBuffCount-1U))
    {
      /* Set next descriptor address register with next descriptor base address */
      dmatxdesc->Buffer2NextDescAddr = (uint32_t)(DMATxDescTab+i+1U);
    }
    else
    {
      /* For last descriptor, set next descriptor address register equal to the first descriptor base address */ 
      dmatxdesc->Buffer2NextDescAddr = (uint32_t) DMATxDescTab;  
    }
		
		f429_nx_driver_information.nx_packets_send[i] = NX_NULL;
  }
  
  /* Set Transmit Descriptor List Address Register */
  (heth->Instance)->DMATDLAR = (uint32_t) DMATxDescTab;
  
  /* Set ETH HAL State to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}

/* 1��ʹ�� NX_PACK_POOL ���� RxBuff���� NX_PACK_POOL ������ NX_PACK�������RX������
 *		a��ע��HALԭ���� ControlBufferSize��Buffer1Addr �����߼�
 * 		b��ע�͹��� ETH_RXINTERRUPT_MODE �Ĵ���
 * 		c���� NX_PACK_POOL ���� NX_PACK �������� ��ַ�ʹ�С �����RX������
 * 2��ʹ�� nx_packets_receive[] ���� NX_PACK��
 * ע��heth->RxDesc = DMARxDescTab; �����⣬��Ҫע�͵�
 */
HAL_StatusTypeDef ETH_DMARxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMARxDescTab, NX_PACKET_POOL *RecPackPool, uint32_t RxBuffCount)
{
	NX_PACKET					*packet_ptr;
  uint32_t i = 0U;
  ETH_DMADescTypeDef *DMARxDesc;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
//  /* Set the Ethernet RxDesc pointer with the first one of the DMARxDescTab list */
//  heth->RxDesc = DMARxDescTab; 
  
  /* Fill each DMARxDesc descriptor with the right values */
  for(i=0U; i < RxBuffCount; i++)
  {
    /* Get the pointer on the ith member of the Rx Desc list */
    DMARxDesc = DMARxDescTab+i;
    
    /* Set Own bit of the Rx descriptor Status */
    DMARxDesc->Status = ETH_DMARXDESC_OWN;
    
//    /* Set Buffer1 size and Second Address Chained bit */
//    DMARxDesc->ControlBufferSize = ETH_DMARXDESC_RCH | ETH_RX_BUF_SIZE;  
//    
//    /* Set Buffer1 address pointer */
//    DMARxDesc->Buffer1Addr = (uint32_t)(&RxBuff[i*ETH_RX_BUF_SIZE]);
//    
//    if((heth->Init).RxMode == ETH_RXINTERRUPT_MODE)
//    {
//      /* Enable Ethernet DMA Rx Descriptor interrupt */
//      DMARxDesc->ControlBufferSize &= ~ETH_DMARXDESC_DIC;
//    }
		if (nx_packet_allocate(RecPackPool, &packet_ptr, NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS)
		{
			packet_ptr -> nx_packet_prepend_ptr += 2;
			DMARxDesc->Buffer1Addr = (uint32_t) packet_ptr -> nx_packet_prepend_ptr;
			
			DMARxDesc->ControlBufferSize = ETH_DMARXDESC_RCH | (packet_ptr -> nx_packet_data_end - packet_ptr -> nx_packet_data_start);  
    
			f429_nx_driver_information.nx_packets_receive[i] = packet_ptr;
		}
		else
			return HAL_ERROR;

    
    /* Initialize the next descriptor with the Next Descriptor Polling Enable */
    if(i < (RxBuffCount-1U))
    {
      /* Set next descriptor address register with next descriptor base address */
      DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab+i+1U); 
    }
    else
    {
      /* For last descriptor, set next descriptor address register equal to the first descriptor base address */ 
      DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab); 
    }
  }
  
	f429_nx_driver_information.nx_driver_rx_nx_pack_size = packet_ptr -> nx_packet_data_end - packet_ptr -> nx_packet_data_start;
	
  /* Set Receive Descriptor List Address Register */
  (heth->Instance)->DMARDLAR = (uint32_t) DMARxDescTab;
  
  /* Set ETH HAL State to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}


/* 1��ʹ�� TX_NX_PACK ��������FrameLength
 *		a������ packet_ptr -> nx_packet_next �Ƿ�ΪNULL���ж�ͷ��/�����/β��
 * 2��β����������� �ж� ���ܣ�ʹ���ڷ�����һ֡�󣬲��������ж�
 * 3��send_nx_pack����
 */
HAL_StatusTypeDef ETH_TransmitFrame(ETH_HandleTypeDef *heth, NX_PACKET *packet_ptr)
{
	NX_PACKET       *pktIdx;
	ULONG           curIdx = f429_nx_driver_information.send_current_index;
  uint32_t bufcount = 0U;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
	for (pktIdx = packet_ptr; 		pktIdx != NX_NULL; 		pktIdx = pktIdx -> nx_packet_next)
	{
		bufcount++;
		
		if(((heth->TxDesc)->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
		{  
			heth->State = HAL_ETH_STATE_BUSY_TX;
			__HAL_UNLOCK(heth);  
			return HAL_ERROR;
		}
	
		/* Clear FIRST and LAST segment bits */
		heth->TxDesc->Status &= ~(ETH_DMATXDESC_FS | ETH_DMATXDESC_LS);
		
		if (pktIdx == packet_ptr) 	//ͷ��
			heth->TxDesc->Status |= ETH_DMATXDESC_FS;  
		else
			curIdx = (curIdx + 1) & (F429_NX_TXDESC_COUNT - 1);			//��������ͷ�� = �Լ�1
		
		if (pktIdx -> nx_packet_next == NX_NULL)		//β��
			heth->TxDesc->Status |= ETH_DMATXDESC_LS | ETH_DMATXDESC_IC;		//����жϱ�־λ��ʹ���ڷ�����һ֡�󣬲���һ���ж�
		
		heth->TxDesc->Buffer1Addr = (ULONG)packet_ptr->nx_packet_prepend_ptr;
		heth->TxDesc->ControlBufferSize = ((packet_ptr -> nx_packet_append_ptr - packet_ptr->nx_packet_prepend_ptr) & ETH_DMATXDESC_TBS1);
		
		heth->TxDesc->Status |= ETH_DMATXDESC_OWN;
		heth->TxDesc = (ETH_DMADescTypeDef *)(heth->TxDesc->Buffer2NextDescAddr);
	}
  
	f429_nx_driver_information.nx_packets_send[curIdx] = packet_ptr;		//����֡ͷ����ǰָ��ָ������飬���Ǳ�������ݲ���������������
	f429_nx_driver_information.send_current_index = (curIdx + 1) & (F429_NX_TXDESC_COUNT - 1);
	
	f429_nx_driver_information.send_in_use_index += bufcount;
	
  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if (((heth->Instance)->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
  {
    /* Clear TBUS ETHERNET DMA flag */
    (heth->Instance)->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    (heth->Instance)->DMATPDR = 0U;
  }
  
  /* Set ETH HAL State to Ready */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}
