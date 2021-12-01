#include "F429_NX_Drive.h"

F429_NX_DRIVE_INFOMATION f429_nx_driver_information;

VOID  nx_driver_stm32f429(NX_IP_DRIVER *driver_req_ptr)
{
	switch (driver_req_ptr -> nx_ip_driver_command)
	{
		case NX_LINK_INTERFACE_ATTACH:		_nx_driver_interface_attach(driver_req_ptr);					break;
		case NX_LINK_INITIALIZE:					_nx_driver_initialize(driver_req_ptr);								break;
		case NX_LINK_ENABLE:							_nx_driver_enable(driver_req_ptr);										break;
		case NX_LINK_DISABLE:							_nx_driver_disable(driver_req_ptr);										break;
		
		case NX_LINK_ARP_SEND:
    case NX_LINK_ARP_RESPONSE_SEND:
    case NX_LINK_PACKET_BROADCAST:
    case NX_LINK_RARP_SEND:
		case NX_LINK_PACKET_SEND:					_nx_driver_packet_send(driver_req_ptr);								break;
		case NX_LINK_MULTICAST_JOIN:			_nx_driver_multicast_join(driver_req_ptr);						break;
		case NX_LINK_MULTICAST_LEAVE:			_nx_driver_multicast_leave(driver_req_ptr);						break;
		default:				driver_req_ptr -> nx_ip_driver_status =  NX_UNHANDLED_COMMAND;					break;
	}
}


VOID  _nx_driver_interface_attach(NX_IP_DRIVER *driver_req_ptr)
{
	f429_nx_driver_information.nx_ip_driver_interface_ptr = driver_req_ptr -> nx_ip_driver_interface;
	driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}

VOID	_nx_driver_initialize(NX_IP_DRIVER *driver_req_ptr)
{
	UINT            status;
	NX_INTERFACE    *interface_ptr = driver_req_ptr -> nx_ip_driver_interface;	
	
	f429_nx_driver_information.nx_driver_nx_packet_pool_ptr = driver_req_ptr -> nx_ip_driver_ptr -> nx_ip_default_packet_pool;
	f429_nx_driver_information.nx_driver_ip_ptr = NX_NULL;
	f429_nx_driver_information.receive_current_index = 0;
	f429_nx_driver_information.send_current_index = 0;
	f429_nx_driver_information.send_release_index = 0;
	f429_nx_driver_information.send_in_use_index = 0;
	
	status =  _nx_driver_hardware_initialize(driver_req_ptr);
	
	if (status == NX_SUCCESS)
	{
		//设置IP信息
		f429_nx_driver_information.nx_driver_ip_ptr = driver_req_ptr -> nx_ip_driver_ptr;
		//设置链接最大传输单元。
		interface_ptr->nx_interface_ip_mtu_size =  NX_DRIVER_ETHERNET_MTU - NX_DRIVER_ETHERNET_FRAME_SIZE;

		//设置此IP实例的物理地址。
		interface_ptr->nx_interface_physical_address_msw = (ULONG)((ETH_MAC_Address[0]<<8)  | (ETH_MAC_Address[1]));
		interface_ptr->nx_interface_physical_address_lsw = (ULONG)((ETH_MAC_Address[2]<<24) | (ETH_MAC_Address[3]<<16) | (ETH_MAC_Address[4]<<8)  | (ETH_MAC_Address[5]));

		//向IP软件指示需要IP到物理映射
		interface_ptr -> nx_interface_address_mapping_needed =  NX_TRUE;
		driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
	}
	else
	{
		driver_req_ptr -> nx_ip_driver_status =   NX_DRIVER_ERROR;
	}
}

VOID	_nx_driver_enable(NX_IP_DRIVER *driver_req_ptr)
{
	UINT            status;
	NX_IP					  *ip_ptr = driver_req_ptr -> nx_ip_driver_ptr;
	
	status =  _nx_driver_hardware_enable(driver_req_ptr);
	
	if (status == NX_SUCCESS)
	{
		driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
		ip_ptr -> nx_ip_driver_link_up =  NX_TRUE;	//将IP实例标记为link up
	}
	else
	{
		driver_req_ptr -> nx_ip_driver_status =   NX_DRIVER_ERROR;
	}	
}

VOID	_nx_driver_disable(NX_IP_DRIVER *driver_req_ptr)
{
	UINT            status;
	NX_IP					  *ip_ptr = driver_req_ptr -> nx_ip_driver_ptr;
	
	status =  _nx_driver_hardware_disable(driver_req_ptr);
	
	if (status == NX_SUCCESS)
	{
		ip_ptr -> nx_ip_driver_link_up =  NX_FALSE;
		driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
	}
	else
	{
		driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
	}
}

VOID	_nx_driver_packet_send(NX_IP_DRIVER *driver_req_ptr)
{
	UINT            status;
	NX_IP           *ip_ptr;
	NX_PACKET       *packet_ptr;
	ULONG           *ethernet_frame_ptr;
	
	ip_ptr =  driver_req_ptr -> nx_ip_driver_ptr;
	packet_ptr =  driver_req_ptr -> nx_ip_driver_packet;

	/* 调整前置指针.  */
	packet_ptr -> nx_packet_prepend_ptr =  packet_ptr -> nx_packet_prepend_ptr - NX_DRIVER_ETHERNET_FRAME_SIZE;

	/* 调整包长度.  */
	packet_ptr -> nx_packet_length =  packet_ptr -> nx_packet_length + NX_DRIVER_ETHERNET_FRAME_SIZE;

	/* 设置以太网帧指针来构建以太网帧。备份另外2个字节以获得32位字对齐。.  */
	/*Lint -e{927} -e{826}禁止指针对指针的强制转换，因为这是必要的  */
	ethernet_frame_ptr =  (ULONG *)(packet_ptr -> nx_packet_prepend_ptr - 2);

	/* 建立以太网帧。  */
	*ethernet_frame_ptr       =  driver_req_ptr -> nx_ip_driver_physical_address_msw;
	*(ethernet_frame_ptr + 1) =  driver_req_ptr -> nx_ip_driver_physical_address_lsw;
	*(ethernet_frame_ptr + 2) =  (ip_ptr -> nx_ip_arp_physical_address_msw << 16) | (ip_ptr -> nx_ip_arp_physical_address_lsw >> 16);
	*(ethernet_frame_ptr + 3) =  (ip_ptr -> nx_ip_arp_physical_address_lsw << 16);
	
	if ((driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_SEND) || (driver_req_ptr -> nx_ip_driver_command == NX_LINK_ARP_RESPONSE_SEND))
		*(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_ARP;
	else if(driver_req_ptr -> nx_ip_driver_command == NX_LINK_RARP_SEND)
		*(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_RARP;  
	else if(packet_ptr -> nx_packet_ip_version == NX_IP_VERSION_V6)
		*(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IPV6;
	else
		*(ethernet_frame_ptr + 3) |= NX_DRIVER_ETHERNET_IP;
	
	/* 如果定义了NX_LITTLE_ENDIAN，则端序交换.  */
	NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr));
	NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 1));
	NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 2));
	NX_CHANGE_ULONG_ENDIAN(*(ethernet_frame_ptr + 3));
	
	//长度超过MTU
	if (packet_ptr -> nx_packet_length > NX_DRIVER_ETHERNET_MTU)
	{
		//恢复前置指针
		packet_ptr -> nx_packet_prepend_ptr =  packet_ptr -> nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE; 
		//恢复包长度
		packet_ptr -> nx_packet_length =  packet_ptr -> nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;

		driver_req_ptr -> nx_ip_driver_status =  NX_SIZE_ERROR;
		//释放掉该包
		nx_packet_transmit_release(packet_ptr);
		return;
	}
	
	status = _nx_driver_hardware_packet_send(packet_ptr);
	
	if (status != NX_SUCCESS)
	{
		//去除添加的头部 数据和长度
		packet_ptr -> nx_packet_prepend_ptr =  packet_ptr -> nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE; 
		packet_ptr -> nx_packet_length =  packet_ptr -> nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;  

		driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
		//释放掉该包
		nx_packet_transmit_release(packet_ptr);
	}
	else
		driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;	
}

VOID	_nx_driver_multicast_join(NX_IP_DRIVER *driver_req_ptr)
{
	UINT        status;

	status =  _nx_driver_hardware_multicast_join(driver_req_ptr);

	if (status != NX_SUCCESS)
		driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
	else   
		driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}

VOID	_nx_driver_multicast_leave(NX_IP_DRIVER *driver_req_ptr)
{
	UINT        status;

	status =  _nx_driver_hardware_multicast_leave(driver_req_ptr);

	if (status != NX_SUCCESS)
		driver_req_ptr -> nx_ip_driver_status =  NX_DRIVER_ERROR;
	else   
		driver_req_ptr -> nx_ip_driver_status =  NX_SUCCESS;
}



void  _nx_F429_recive_to_netx(NX_IP *ip_ptr, NX_PACKET *packet_ptr)
{
	USHORT    packet_type;
	/* 拾取包头以确定包需要发送到哪里。 */
	packet_type = (((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 12))) << 8) | ((UINT)(*(packet_ptr->nx_packet_prepend_ptr + 13)));
	
	/* 设置接口指针。  */
  packet_ptr -> nx_packet_address.nx_packet_interface_ptr = f429_nx_driver_information.nx_ip_driver_interface_ptr ;
	
	/*根据以太网类型路由传入的数据包。  */
	/* RAM驱动程序同时接受IPv4和IPv6帧。. */
	if ((packet_type == NX_DRIVER_ETHERNET_IP) || (packet_type == NX_DRIVER_ETHERNET_IPV6))
	{
		/* 清除以太网头。  */
		packet_ptr->nx_packet_prepend_ptr =  packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE;
		/* 调整数据包长度.  */
		packet_ptr->nx_packet_length 			=  packet_ptr->nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;

		_nx_ip_packet_receive(ip_ptr, packet_ptr);
	}
	else if (packet_type == NX_DRIVER_ETHERNET_ARP)
	{
		packet_ptr->nx_packet_prepend_ptr =  packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE;
		packet_ptr->nx_packet_length 			=  packet_ptr->nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;
		_nx_arp_packet_deferred_receive(ip_ptr, packet_ptr);
	}
	else if (packet_type == NX_DRIVER_ETHERNET_RARP)
	{
		packet_ptr->nx_packet_prepend_ptr = packet_ptr->nx_packet_prepend_ptr + NX_DRIVER_ETHERNET_FRAME_SIZE;
		packet_ptr->nx_packet_length 			= packet_ptr->nx_packet_length - NX_DRIVER_ETHERNET_FRAME_SIZE;
		_nx_rarp_packet_deferred_receive(ip_ptr, packet_ptr);
	}
	else
	{
		nx_packet_release(packet_ptr);
	}
}

VOID	_nx_driver_packet_transmitted(VOID)
{
	ULONG numOfBuf =  f429_nx_driver_information.send_in_use_index;
	ULONG idx =       f429_nx_driver_information.send_release_index;
	
	while (numOfBuf--)
	{
		if (f429_nx_driver_information.nx_packets_send[idx] == NX_NULL) 	//在 nx_packets_send 找到刚才发送的帧头
		{		
				idx = (idx + 1) & (F429_NX_TXDESC_COUNT - 1);
				continue;
		}
		
		if ((ETH_DMATxDescTab[idx].Status & ETH_DMATXDESC_OWN) == 0)			//idx也指向 帧尾 的描述符，该判断语句表示帧尾发送完成，表明整个帧都发送完成
		{
			NX_DRIVER_ETHERNET_HEADER_REMOVE(f429_nx_driver_information.nx_packets_send[idx]);
			nx_packet_transmit_release(f429_nx_driver_information.nx_packets_send[idx]);
			f429_nx_driver_information.nx_packets_send[idx] = NX_NULL;
			
			idx = (idx + 1) & (F429_NX_TXDESC_COUNT - 1);
			f429_nx_driver_information.send_in_use_index = numOfBuf;
			f429_nx_driver_information.send_release_index = idx;
		}
		else
			break;
	}
}

VOID	_nx_driver_packet_received(VOID)
{
	NX_PACKET     *packet_ptr;
	ULONG          bd_count = 0;
	INT            i;
	ULONG          idx;
	ULONG          temp_idx;
	ULONG          first_idx = f429_nx_driver_information.receive_current_index;
	NX_PACKET     *received_packet_ptr = f429_nx_driver_information.nx_packets_receive[first_idx];
	
	 for (first_idx = idx = f429_nx_driver_information.receive_current_index; (ETH_DMARxDescTab[idx].Status & ETH_DMARXDESC_OWN) == 0; idx = (idx + 1) & (F429_NX_RXDESC_COUNT - 1))
	{
		if (ETH_DMARxDescTab[idx].Status & ETH_DMARXDESC_LS)
		{
			f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_next = NX_NULL;
			f429_nx_driver_information.nx_packets_receive[first_idx] -> nx_packet_length = ((ETH_DMARxDescTab[idx].Status & ETH_DMARXDESC_FL) >> ETH_DMARXDESC_FRAME_LENGTHSHIFT) - 4;
						
			/* Adjust nx_packet_append_ptr with the size of the data in this buffer.  */
			f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_append_ptr = f429_nx_driver_information.nx_packets_receive[idx]->nx_packet_prepend_ptr
																																															 + f429_nx_driver_information.nx_packets_receive[first_idx]->nx_packet_length
																																															 - bd_count * f429_nx_driver_information.nx_driver_rx_nx_pack_size;
			
			if (idx != first_idx)
			{

				/* No, this BD is not the first BD of the frame, frame data starts at the aligned address.  */
				f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr -= 2;

				if (f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr >=f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_append_ptr)
				{

					temp_idx = (idx - 1) & (F429_NX_RXDESC_COUNT - 1);
					f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_next = NX_NULL;
					f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_append_ptr -= f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr >= f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_append_ptr;
					ETH_DMARxDescTab[idx].Status = ETH_DMARXDESC_OWN;
					f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr = f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_data_start + 2;
					bd_count--;
				}
			}
			
			for (i = bd_count; i >= 0; i--)
			{

				temp_idx = (first_idx + i) & (F429_NX_RXDESC_COUNT - 1);

				/* Allocate a new packet from the packet pool.  */
				if (nx_packet_allocate(f429_nx_driver_information.nx_driver_nx_packet_pool_ptr, &packet_ptr, NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS)
				{
					/* Adjust the new packet and assign it to the BD.  */
					packet_ptr -> nx_packet_prepend_ptr += 2;
					ETH_DMARxDescTab[temp_idx].Buffer1Addr = (ULONG)packet_ptr->nx_packet_prepend_ptr;
					ETH_DMARxDescTab[temp_idx].Status = ETH_DMARXDESC_OWN;
					f429_nx_driver_information.nx_packets_receive[temp_idx] = packet_ptr;
				}
				else
				{
					break;
				}
			}
			
			if (i >= 0)
			{
					
					/* At least one packet allocation was failed, release the received packet.  */
					nx_packet_release(f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_next);
					
					for (; i >= 0; i--)
					{
							
							/* Free up the BD to ready state. */
							temp_idx = (first_idx + i) & (F429_NX_RXDESC_COUNT - 1);
							ETH_DMARxDescTab[temp_idx].Status = ETH_DMARXDESC_OWN;
							f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_prepend_ptr = f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_data_start + 2;
					}
			}
			else
			{
					
					/* Everything is OK, transfer the packet to NetX.  */
					_nx_F429_recive_to_netx(f429_nx_driver_information.nx_driver_ip_ptr, received_packet_ptr);
			}

			/* Set the first BD index for the next packet.  */
			first_idx = (idx + 1) & (F429_NX_RXDESC_COUNT - 1);

			/* Update the current receive index.  */
			f429_nx_driver_information.receive_current_index = first_idx;

			received_packet_ptr = f429_nx_driver_information.nx_packets_receive[first_idx];

			bd_count = 0;

			}
			else
			{

				/* This BD is not the last BD of a frame. It is a intermediate descriptor.  */

				f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_next = f429_nx_driver_information.nx_packets_receive[(idx + 1) & (F429_NX_RXDESC_COUNT - 1)];

				f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_append_ptr = f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_data_end;

				if (idx != first_idx)
				{
						
						f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr = f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_data_start;
				}

				bd_count++;
			}
		if ((ETH->DMASR & ETH_DMASR_RBUS) != (ULONG)RESET)  
    {
        
        /* Clear RBUS ETHERNET DMA flag */
        ETH->DMASR = ETH_DMASR_RBUS;
        /* Resume DMA reception */
        ETH->DMARPDR = 0;
    }
	}
}
//VOID	_nx_driver_packet_received(VOID)
//{
//	ETH_DMADescTypeDef *dmatxdesc;
//	int  RxDecCoun = HETH.RxFrameInfos.SegCount;
//	
//	NX_PACKET     *packet_ptr;
//	ULONG          bd_count = 0;
//	INT            i;
//	ULONG          idx;
//	ULONG          temp_idx;
//	ULONG          first_idx = f429_nx_driver_information.receive_current_index;
//	NX_PACKET     *received_packet_ptr = f429_nx_driver_information.nx_packets_receive[first_idx];
//	
//	if(HAL_ETH_GetReceivedFrame_IT(&HETH) == HAL_OK)
//	{
//		dmatxdesc = HETH.RxFrameInfos.FSRxDesc;		//指向头包
//		for (idx = first_idx; 	(RxDecCoun--) > 0; 	idx = (idx + 1) & (F429_NX_RXDESC_COUNT - 1))
//		{
//			if(dmatxdesc != HETH.RxFrameInfos.LSRxDesc)			//非尾包
//			{
//				f429_nx_driver_information.nx_packets_receive[idx]->nx_packet_next = f429_nx_driver_information.nx_packets_receive[(idx + 1) & (F429_NX_RXDESC_COUNT - 1)];
//				f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_append_ptr = f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_data_end;
//				if(dmatxdesc != HETH.RxFrameInfos.FSRxDesc)		//身体包
//					f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr = f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_data_start;
//				bd_count = 0;
//				
//				dmatxdesc = (ETH_DMADescTypeDef *)dmatxdesc->Buffer2NextDescAddr;
//			}
//			else	//尾包
//			{
//				f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_next = NX_NULL;
//				f429_nx_driver_information.nx_packets_receive[first_idx] -> nx_packet_length = ((ETH_DMARxDescTab[idx].Status & ETH_DMARXDESC_FL) >> ETH_DMARXDESC_FRAME_LENGTHSHIFT) - 4;
//				f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_append_ptr = f429_nx_driver_information.nx_packets_receive[idx]->nx_packet_prepend_ptr
//																																															 + f429_nx_driver_information.nx_packets_receive[first_idx]->nx_packet_length
//																																															 - bd_count * f429_nx_driver_information.nx_driver_rx_nx_pack_size;
//				if(HETH.RxFrameInfos.SegCount == 1)
//				{
//					f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr -= 2;

//					if (f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr >=f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_append_ptr)
//					{

//						temp_idx = (idx - 1) & (F429_NX_RXDESC_COUNT - 1);
//						f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_next = NX_NULL;
//						f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_append_ptr -= f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr >= f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_append_ptr;
//						ETH_DMARxDescTab[idx].Status = ETH_DMARXDESC_OWN;
//						f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_prepend_ptr = f429_nx_driver_information.nx_packets_receive[idx] -> nx_packet_data_start + 2;
//						bd_count--;
//					}
//				}
//				
//				for (i = bd_count; i >= 0; i--)
//				{

//					temp_idx = (first_idx + i) & (F429_NX_RXDESC_COUNT - 1);

//					/* Allocate a new packet from the packet pool.  */
//					if (nx_packet_allocate(f429_nx_driver_information.nx_driver_nx_packet_pool_ptr, &packet_ptr, NX_RECEIVE_PACKET, NX_NO_WAIT) == NX_SUCCESS)
//					{
//						/* Adjust the new packet and assign it to the BD.  */
//						packet_ptr -> nx_packet_prepend_ptr += 2;
//						ETH_DMARxDescTab[temp_idx].Buffer1Addr = (ULONG)packet_ptr->nx_packet_prepend_ptr;
//						ETH_DMARxDescTab[temp_idx].Status = ETH_DMARXDESC_OWN;
//						f429_nx_driver_information.nx_packets_receive[temp_idx] = packet_ptr;
//					}
//					else
//					{
//						break;
//					}
//				}
//				
//				if(i >= 0)
//				{
//					nx_packet_release(f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_next);
//					
//					for (; i >= 0; i--)
//					{
//							
//							/* Free up the BD to ready state. */
//							temp_idx = (first_idx + i) & (F429_NX_RXDESC_COUNT - 1);
//							ETH_DMARxDescTab[temp_idx].Status = ETH_DMARXDESC_OWN;
//							f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_prepend_ptr = f429_nx_driver_information.nx_packets_receive[temp_idx] -> nx_packet_data_start + 2;
//					}
//				}
//				else
//				{
//					_nx_F429_recive_to_netx(f429_nx_driver_information.nx_driver_ip_ptr, received_packet_ptr);
//				}
//				
//				first_idx = (idx + 1) & (F429_NX_RXDESC_COUNT - 1);

//				/* Update the current receive index.  */
//				f429_nx_driver_information.receive_current_index = first_idx;

//				received_packet_ptr = f429_nx_driver_information.nx_packets_receive[first_idx];

//				bd_count = 0;
//				
//			}
//		}
//	}
//}

void ETH_IRQHandler(void)
{
	ULONG status;
	status = ETH->DMASR;

	/* Clear the Ethernet DMA Rx IT pending bits */
	ETH->DMASR = ETH_DMA_IT_R | ETH_DMA_IT_T | ETH_DMA_IT_NIS;

	if(status & ETH_DMA_IT_T)
		_nx_driver_packet_transmitted();
	if(status & ETH_DMA_IT_R)
		_nx_driver_packet_received();
}	