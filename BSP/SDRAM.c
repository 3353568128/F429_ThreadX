#include <SDRAM.h>
#include "main.h"

void SDRAM_delay(__IO uint32_t nCount)
{
  __IO uint32_t index = 0; 
	
  for(index = (100000 * nCount); index != 0; index--);

}


void SDRAM_Initialization_Sequence(SDRAM_HandleTypeDef *hsdram, FMC_SDRAM_CommandTypeDef *Command)
{
  __IO uint32_t tmpmrd = 0;
  
  /* Configure a clock configuration enable command */
  Command->CommandMode 					= FMC_SDRAM_CMD_CLK_ENABLE;	// 开启SDRAM时钟 
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK; 	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);	// 发送控制指令
  SDRAM_delay(1);		// 延时等待
  
  /* Configure a PALL (precharge all) command */ 
  Command->CommandMode 					= FMC_SDRAM_CMD_PALL;		// 预充电命令
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);  // 发送控制指令
  
  /* Configure a Auto-Refresh command */ 
  Command->CommandMode 					= FMC_SDRAM_CMD_AUTOREFRESH_MODE;	// 使用自动刷新
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;          // 选择要控制的区域
  Command->AutoRefreshNumber			= 8;                                // 自动刷新次数
  Command->ModeRegisterDefinition 	= 0;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);	// 发送控制指令
  
  /* Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
                     SDRAM_MODEREG_CAS_LATENCY_3           |
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command->CommandMode					= FMC_SDRAM_CMD_LOAD_MODE;	// 加载模式寄存器命令
  Command->CommandTarget 				= FMC_COMMAND_TARGET_BANK;	// 选择要控制的区域
  Command->AutoRefreshNumber 			= 1;
  Command->ModeRegisterDefinition 	= tmpmrd;
  
  HAL_SDRAM_SendCommand(hsdram, Command, SDRAM_TIMEOUT);	// 发送控制指令
  
  hsdram->Instance->SDRTR |= ((uint32_t)((1386)<< 1));	// 设置刷新计数器 
}
