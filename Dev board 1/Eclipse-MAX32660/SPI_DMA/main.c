/**
 * @file    main.c
 * @brief   SPI DMA Master Demo
 * @details Demonstrates 
 */

/*******************************************************************************
* Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************/

/***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_config.h"
#include "mxc_pins.h"
#include "mxc_delay.h"
#include "spi.h"
#include "dma.h"

/***** Definitions *****/
#define TRANSFER_BYTES			1024
// #define PRINT_DATA				
/***** Globals *****/

volatile unsigned int dma_done_flag = 0;

/***** Functions *****/

void DMA0_IRQHandler(void)
{
  	DMA_Handler(0);
}
void DMA1_IRQHandler(void)
{
	DMA_Handler(1);
}
void DMA2_IRQHandler(void)
{
  	DMA_Handler(2);
}
void DMA3_IRQHandler(void)
{
  	DMA_Handler(3);
}

void DMA_NVIC_Enable(void)
{
	NVIC_EnableIRQ(DMA0_IRQn);
	NVIC_EnableIRQ(DMA1_IRQn);
	NVIC_EnableIRQ(DMA2_IRQn);
	NVIC_EnableIRQ(DMA3_IRQn);
}

void DMA_Callback(int ch, int reason)
{
	dma_done_flag = 1;
}

int main(void)
{
	int rxch, txch, i = 0;
	uint8_t rxbuf;
	uint8_t txbuf[TRANSFER_BYTES];
	volatile uint8_t dma_rxbuf[TRANSFER_BYTES];
	spi_req_t req;
	
	setvbuf(stdout, (char *)NULL, _IOLBF, 0);
	
	printf("*** MAX32660 DMA-driven SPI example ***\n");
	printf("*** Connect P0.4(MISO) & P0.5(MOSI) ***\n\n");

	if (SPI_Init(SPI0A, 0, 5000000) != E_NO_ERROR) 
	{
		printf("Error initializing SPI0\n");
		return E_COMM_ERR;
	} 

	if (DMA_Init() != E_NO_ERROR) 
	{
		printf("Error initializing DMA\n");
		return E_UNINITIALIZED;
	} 
	
	if (((txch = DMA_AcquireChannel()) < 0) ||((rxch = DMA_AcquireChannel()) < 0)) 
	{
		printf("Unable to acquire DMA channels\n");
		return E_BUSY;
	} 	
	
	for(i=0;i<TRANSFER_BYTES;i++)
	{
		txbuf[i] = i;
	}

	req.tx_data = txbuf;
	req.rx_data = &rxbuf;
	req.len = 1;
	req.bits = 8;
	req.width = SPI0_WIDTH_1;
	req.ssel_pol = SPI_POL_LOW;
	req.ssel = 0;
	req.deass = 1;
	req.tx_num = 0;
	req.rx_num = 0;
	req.callback = NULL;

	printf("\nSingle non-DMA transfer to initialize SPI registers\n");
	if (SPI_MasterTrans(SPI0A, &req) != E_NO_ERROR) 
	{
		printf("Transfer error on SPI0\n");
		while (1);
	} 

	printf("\nSwitching to DMA transfers (txch = %u, rxch = %u)\n", txch, rxch);

	/* Disable SPI17Y interrupts -- DMA handles all of this */
	MXC_SPI17Y->int_en = 0;
	
	/* Master: Data filling the RX FIFO will block our TX FIFO, so RX should be higher priority */
	DMA_ConfigChannel(txch, DMA_PRIO_LOW,
			DMA_REQSEL_SPI0TX, DMA_FALSE,
			0 /* Not Used */, DMA_PRESCALE_DISABLE,
			DMA_WIDTH_BYTE, DMA_TRUE /* Increment */,
			DMA_WIDTH_BYTE, DMA_FALSE /* No increment */,
			1 /* Burst size */, DMA_FALSE /* CHDIS Interrupt */,
			DMA_FALSE /* CTZ Interrupt */);
	DMA_ConfigChannel(rxch, DMA_PRIO_HIGH,
			DMA_REQSEL_SPI0RX, DMA_FALSE,
			0 /* Not Used */, DMA_PRESCALE_DISABLE,
			DMA_WIDTH_BYTE, DMA_FALSE /* Increment */,
			DMA_WIDTH_BYTE, DMA_TRUE /* No increment */,
			1 /* Burst size */, DMA_FALSE /* CHDIS Interrupt */,
			DMA_TRUE /* CTZ Interrupt */);

	/* Setup DMA-related items */
	MXC_SPI17Y->dma =
	(1 << MXC_F_SPI17Y_DMA_TX_FIFO_LEVEL_POS) |
	MXC_F_SPI17Y_DMA_TX_FIFO_EN |
	MXC_F_SPI17Y_DMA_TX_DMA_EN |
	(0 << MXC_F_SPI17Y_DMA_RX_FIFO_LEVEL_POS) |
	MXC_F_SPI17Y_DMA_RX_FIFO_EN |
	MXC_F_SPI17Y_DMA_RX_DMA_EN;

	/* DMA setup */
	memset((void *)dma_rxbuf, 0, TRANSFER_BYTES);
	DMA_SetSrcDstCnt(txch, txbuf, NULL /* Unused */, TRANSFER_BYTES);
	DMA_SetSrcDstCnt(rxch, NULL /* Unused */, (void *)dma_rxbuf, TRANSFER_BYTES);
	DMA_SetCallback(rxch, DMA_Callback);
	DMA_EnableInterrupt(rxch);
	DMA_NVIC_Enable();
	
	/* Length of transfer */
	MXC_SPI17Y->ctrl1 = TRANSFER_BYTES;
	
	/* Enable the DMA channels */
	dma_done_flag = 0;
	DMA_Start(txch);
	DMA_Start(rxch);
	
	/* Start the SPI transfer */
	MXC_SPI17Y->ctrl0 |= MXC_F_SPI17Y_CTRL0_START;
	
	while (!dma_done_flag);

	if (memcmp(txbuf, (void *)dma_rxbuf, TRANSFER_BYTES)) 
	{
		/* Mismatch! */
		printf("\nMismatch detected!\n");
		printf("\nTxData	  RxData\n");
		for (i = 0; i < TRANSFER_BYTES; i++) 
		{
			printf("0x%02x	 0x%02x\n", txbuf[i], dma_rxbuf[i]);
		}
		printf(" --- \n");
		while (1);
	} 
	else 
	{
		printf("\nData Transfered\n");

		#ifdef PRINT_DATA
			printf("\nTxData	  RxData\n");
			for (i = 0; i < TRANSFER_BYTES; i++) 
			{
				printf("0x%02x	   0x%02x\n", txbuf[i], dma_rxbuf[i]);
			}
		#endif

		printf(" --- \n");
	}

	return E_SUCCESS;
}
