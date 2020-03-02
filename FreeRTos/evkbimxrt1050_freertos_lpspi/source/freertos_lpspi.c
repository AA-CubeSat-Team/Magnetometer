/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "fsl_lpspi.h"
#include "fsl_lpspi_freertos.h"
#include "board.h"

#include "fsl_common.h"
#include "pin_mux.h"
//#include "peripherals.h"
#include "magnetometer_tasks.h"
#if ((defined FSL_FEATURE_SOC_INTMUX_COUNT) && (FSL_FEATURE_SOC_INTMUX_COUNT))
#include "fsl_intmux.h"
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Master related */
#define EXAMPLE_LPSPI_MASTER_BASEADDR (LPSPI3)
#define EXAMPLE_LPSPI_MASTER_IRQN (LPSPI3_IRQn)

#define EXAMPLE_LPSPI_MASTER_PCS_FOR_INIT (kLPSPI_Pcs0)
#define EXAMPLE_LPSPI_MASTER_PCS_FOR_TRANSFER (kLPSPI_MasterPcs0)


/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/* LPSPI user callback */

/*******************************************************************************
 * Variables
 ******************************************************************************/
QueueHandle_t xQueue = NULL;
TaskHandle_t MagTaskHandle;
TaskHandle_t QueTaskHandle;
const TickType_t xBlockTime = pdMS_TO_TICKS( 50 );
const int MagCounter = 100;

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
#define master_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

void queue_receiver_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitBootPeripherals();

    /*Set clock source for LPSPI*/
    //CLOCK_SetMux(kCLOCK_LpspiMux, EXAMPLE_LPSPI_CLOCK_SOURCE_SELECT);
    //CLOCK_SetDiv(kCLOCK_LpspiDiv, EXAMPLE_LPSPI_CLOCK_SOURCE_DIVIDER);

    NVIC_SetPriority(EXAMPLE_LPSPI_MASTER_IRQN, 3);

    xQueue = xQueueCreate(10, sizeof(float));
    if(xQueue == NULL){
    	PRINTF("Failed in creating queue\r\n");
    	for(;;)
    		;
    }
    vQueueAddToRegistry( xQueue, "MagQueue" );

    PRINTF("TESTING MAGNETOMETER WITH SPI\r\n");
    PRINTF("\r\n");

    if (xTaskCreate(magnetometer_task, "magnetometer_task", configMINIMAL_STACK_SIZE + 128, NULL, master_task_PRIORITY, &MagTaskHandle) !=
            pdPASS)
        {
            PRINTF("Task creation failed!.\r\n");
        }
    if(xTaskCreate(queue_receiver_task, "receive_task", configMINIMAL_STACK_SIZE + 128, NULL, master_task_PRIORITY-1, &QueTaskHandle) !=
            pdPASS)
    {
        PRINTF("Queue Receiver task creation failed!.\r\n");
    }

    vTaskSuspend(QueTaskHandle);
    vTaskStartScheduler();
    for(;;)
    	;

}

void queue_receiver_task(void* pvParameters){
	float xVal;
	float yVal;
	float zVal;
	int counter = 0;
	while(counter < MagCounter){
		xQueueReceive( xQueue, &xVal, xBlockTime );
		xQueueReceive( xQueue, &yVal, xBlockTime );
		xQueueReceive( xQueue, &zVal, xBlockTime );

		PRINTF("Received Measurement %d\r\n", counter);
		PRINTF("Received X mag value is %f\r\n", xVal);
		PRINTF("Received Y mag value is %f\r\n", yVal);
		PRINTF("Received Z mag value is %f\r\n", zVal);
		counter++;
		vTaskResume(MagTaskHandle);
		vTaskSuspend(NULL);

	}
	PRINTF("Finished Queue Receiver Task\r\n");
	vTaskSuspend(NULL);
}


