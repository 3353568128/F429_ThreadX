#include "UART.h"
#include "main.h"
#include "tx_api.h"

#define TASK_POOL_SIZE	1024
static uint8_t Taskpool[TASK_POOL_SIZE];
static TX_THREAD TaskHand;

static void UARTTASK(ULONG thread_input)
{
	uint32_t msgLen = 0;
	uint8_t msgAddr[512];
	extern UART_HandleTypeDef huart1;
	
	PrintfInit();		//��ʼ��������ڴ��
	
	while(1)
	{
		msgLen = PopBuff(msgAddr);
		HAL_UART_Transmit(&huart1, (uint8_t *)msgAddr, msgLen, msgLen);
		tx_thread_sleep(1);
	}
}

void UART1TaskCreate()
{
	tx_thread_create(&TaskHand, "UART Task", UARTTASK, NULL, Taskpool, TASK_POOL_SIZE, 30, 30, TX_NO_TIME_SLICE, TX_AUTO_START);
}

void PrintfHex(uint8_t *addr, uint32_t len)
{
	uint8_t temp;
	uint32_t x=0;
	static uint8_t TempBuff[128*3];
	
	for(x=0; x<len; x++)
	{
		temp = addr[x]>>4;
		if(temp < 10)
			TempBuff[3*x+0] = temp + '0';
		else
			TempBuff[3*x+0] = temp - 10 + 'A';
		
		temp = addr[x] & 0x0f;
		if(temp < 10)
			TempBuff[3*x+1] = temp + '0';
		else
			TempBuff[3*x+1] = temp - 10 + 'A';
		
		TempBuff[3*x+2] = ' ';
	}
	
	PushBuff(TempBuff, 3*len);
}

void Printf(uint8_t *addr, uint32_t len)
{
	PushBuff(addr, len);
}

//printf ������������
TX_BLOCK_POOL PDL_BlockPool;
uint8_t PrintfDataListBlockBuf[1024];
//printf ���ݻ�����
TX_BYTE_POOL PD_BytePool;
uint8_t PrintfDataPoolBuf[1024];

typedef struct PrintfPack_t
{
	uint8_t *msgaddr;		//ָ�� �ֽڻ���� �У��������ݵ��׵�ַ
	uint32_t msgLen;
	struct PrintfPack_t * next;
}PrintfPack;
PrintfPack *PDL_First = NULL;

void PrintfInit()
{
	//���� �ڴ�黺��أ����ڱ���ṹ������
	tx_block_pool_create(&PDL_BlockPool, "PDLBlock", sizeof(PrintfPack), PrintfDataListBlockBuf, sizeof(PrintfDataListBlockBuf));	
	//���� �ֽڻ���أ��û�����printf����
	tx_byte_pool_create(&PD_BytePool, "PD_BytePool", PrintfDataPoolBuf, sizeof(PrintfDataPoolBuf));
	
	//����һ�� �ڴ�飬��Ϊ����ͷ����ʼ��
	tx_block_allocate(&PDL_BlockPool, (VOID **)&PDL_First, TX_NO_WAIT);
	PDL_First->msgaddr = (uint8_t *)PrintfDataPoolBuf;
	PDL_First->msgLen = 0;
	PDL_First->next = NULL;
	
}

void PushBuff(uint8_t *addr, uint32_t len)
{
	PrintfPack *PDL_Node = NULL;
	PrintfPack *PDL_Temp = NULL;
	uint8_t *PD_Temp = NULL;
	
	tx_byte_allocate(&PD_BytePool, (void **)&PD_Temp, len, TX_NO_WAIT);		//���ڴ������ len�ֽ� �Ŀռ�
	memcpy(PD_Temp, addr, len);								//�������ݵ��ڴ��
	
	tx_block_allocate(&PDL_BlockPool, (VOID **)&PDL_Temp, TX_NO_WAIT);		//���� 1�� ����ڵ㣬���ڱ��������������Ϣ
	PDL_Temp->msgaddr = (uint8_t *)PD_Temp;		//���������Ϣ
	PDL_Temp->msgLen = len;
	PDL_Temp->next = NULL;
	
	//�� PDL_First ���������ҿ������β��
	PDL_Node = PDL_First;	
	while(PDL_Node->next != NULL)
	{
			PDL_Node = PDL_Node->next;
	}
	PDL_Node->next = PDL_Temp;			//�����β������µĽڵ�
}

uint32_t PopBuff(uint8_t * msgAddr)
{
	uint32_t msgLen = 0;
	PrintfPack *PDL_Node = NULL;
	
	if(PDL_First->next != NULL)		//����ͷ����ҽ��нڵ�
	{
		PDL_Node = PDL_First->next;	//ָ������� �ڶ��� ��Ч�ڵ�
		
		memcpy(msgAddr, PDL_Node->msgaddr, PDL_Node->msgLen);		//�������
		msgLen = PDL_Node->msgLen;
		
		PDL_First->next = PDL_Node->next;			//ʹ����ͷָ�� ������ �ڵ�� NULL
		
		tx_byte_release(PDL_Node->msgaddr);		//�ͷ������ڴ��
		tx_block_release(PDL_Node);						//�ͷ������ڴ��
	}
	return msgLen;
}