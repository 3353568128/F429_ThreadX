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
	
	PrintfInit();		//初始化链表和内存池
	
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

//printf 数据链表缓存区
TX_BLOCK_POOL PDL_BlockPool;
uint8_t PrintfDataListBlockBuf[1024];
//printf 数据缓存区
TX_BYTE_POOL PD_BytePool;
uint8_t PrintfDataPoolBuf[1024];

typedef struct PrintfPack_t
{
	uint8_t *msgaddr;		//指向 字节缓存池 中，保存数据的首地址
	uint32_t msgLen;
	struct PrintfPack_t * next;
}PrintfPack;
PrintfPack *PDL_First = NULL;

void PrintfInit()
{
	//创建 内存块缓存池，用于保存结构体链表
	tx_block_pool_create(&PDL_BlockPool, "PDLBlock", sizeof(PrintfPack), PrintfDataListBlockBuf, sizeof(PrintfDataListBlockBuf));	
	//创建 字节缓存池，用户保存printf数据
	tx_byte_pool_create(&PD_BytePool, "PD_BytePool", PrintfDataPoolBuf, sizeof(PrintfDataPoolBuf));
	
	//申请一个 内存块，作为链表头并初始化
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
	
	tx_byte_allocate(&PD_BytePool, (void **)&PD_Temp, len, TX_NO_WAIT);		//从内存池申请 len字节 的空间
	memcpy(PD_Temp, addr, len);								//拷贝数据到内存池
	
	tx_block_allocate(&PDL_BlockPool, (VOID **)&PDL_Temp, TX_NO_WAIT);		//申请 1个 链表节点，用于保存输入的数据信息
	PDL_Temp->msgaddr = (uint8_t *)PD_Temp;		//填充数据信息
	PDL_Temp->msgLen = len;
	PDL_Temp->next = NULL;
	
	//从 PDL_First 遍历链表，找块链表的尾部
	PDL_Node = PDL_First;	
	while(PDL_Node->next != NULL)
	{
			PDL_Node = PDL_Node->next;
	}
	PDL_Node->next = PDL_Temp;			//链表的尾部添加新的节点
}

uint32_t PopBuff(uint8_t * msgAddr)
{
	uint32_t msgLen = 0;
	PrintfPack *PDL_Node = NULL;
	
	if(PDL_First->next != NULL)		//链表头后面挂接有节点
	{
		PDL_Node = PDL_First->next;	//指向链表的 第二个 有效节点
		
		memcpy(msgAddr, PDL_Node->msgaddr, PDL_Node->msgLen);		//输出数据
		msgLen = PDL_Node->msgLen;
		
		PDL_First->next = PDL_Node->next;			//使链表头指向 第三个 节点或 NULL
		
		tx_byte_release(PDL_Node->msgaddr);		//释放数据内存池
		tx_block_release(PDL_Node);						//释放链表内存块
	}
	return msgLen;
}