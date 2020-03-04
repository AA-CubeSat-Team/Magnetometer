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
#include "magnetometer_task.h"
#include "gyroscope_task.h"
#include "structDef.h"
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


/*******************************************************************************
 * Variables
 ******************************************************************************/
QueueHandle_t xMagQueue = NULL;
QueueHandle_t xGyroQueue = NULL;

TaskHandle_t MagTaskHandle;
TaskHandle_t GyroTaskHandle;
TaskHandle_t MagQueTaskHandle;
TaskHandle_t GyroQueTaskHandle;

const TickType_t xBlockTime = portMAX_DELAY;
const int MagMaxCounter = 100;
const int GyroMaxCounter = 150;


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */
#define master_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Forward Declaration
 ******************************************************************************/
void mag_queue_task(void *pvParameters);
void gyro_queue_task(void* pvParameters);

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

    NVIC_SetPriority(EXAMPLE_LPSPI_MASTER_IRQN, 3);

    xMagQueue = xQueueCreate(10, sizeof(Data_t));
    if(xMagQueue == NULL){
    	PRINTF("Failed in creating queue for magneteomter\r\n");
    	for(;;)
    		;
    }
    vQueueAddToRegistry( xMagQueue, "MagQueue" );

    xGyroQueue = xQueueCreate(10, sizeof(Data_t));
    if(xGyroQueue == NULL){
    	PRINTF("Failed in creating queue for gyroscope\r\n");
    	for(;;)
    		;
    }
    vQueueAddToRegistry( xGyroQueue, "GyroQueue" );


    PRINTF("TESTING MAGNETOMETER WITH SPI\r\n");
    PRINTF("\r\n");

    int magID = magnetometer1;
    int gyroID = gyroscope1;
    //PRINTF("Test1 %d\r\n", magID);
    //PRINTF("Test2 %d\r\n", gyroID);

    if (xTaskCreate(magnetometer_task, "magnetometer_task1", configMINIMAL_STACK_SIZE + 128, (void*)&magID, master_task_PRIORITY, &MagTaskHandle) !=
            pdPASS)
        {
            PRINTF("Task creation failed!.\r\n");
        }
    if (xTaskCreate(gyroscope_task, "gyroscope_task", configMINIMAL_STACK_SIZE + 128, (void*)&gyroID, master_task_PRIORITY, &GyroTaskHandle) !=
                pdPASS)
            {
                PRINTF("Task creation failed!.\r\n");
            }
    if(xTaskCreate(mag_queue_task, "mag_queue_task", configMINIMAL_STACK_SIZE + 80, NULL, master_task_PRIORITY, &MagQueTaskHandle) !=
            pdPASS)
    {
        PRINTF("Queue Receiver task creation failed!.\r\n");
    }
    if(xTaskCreate(gyro_queue_task, "gyro_queue_task", configMINIMAL_STACK_SIZE + 80, NULL, master_task_PRIORITY, &GyroQueTaskHandle) !=
                pdPASS)
	{
		PRINTF("Queue Receiver task creation failed!.\r\n");
	}

    //vTaskSuspend(QueTaskHandle);
    vTaskStartScheduler();
    for(;;)
    	;

}

void mag_queue_task(void* pvParameters){
	Data_t message;
	int magCounter = 0;
	int counter[3];
	while(magCounter < MagMaxCounter){
		xQueueReceive(xMagQueue, &message, xBlockTime);
		//PRINTF("Received Measurement %d\r\n", counter);
		PRINTF("Received message from magnetometer %d\r\n", message.id);

		if(message.id == magnetometer1 ){
			PRINTF("Received X mag value is %f\r\n", message.x);
			PRINTF("Received Y mag value is %f\r\n", message.y);
			PRINTF("Received Z mag value is %f\r\n", message.z);
			magCounter++;
			vTaskResume(MagTaskHandle);
		}
		else if(message.id == magnetometer2 ){
			PRINTF("Received X mag value is %f\r\n", message.x);
			PRINTF("Received Y mag value is %f\r\n", message.y);
			PRINTF("Received Z mag value is %f\r\n", message.z);
			//vTaskResume(MagTaskHandle);
		}else if(message.id == magnetometer3 ){
			PRINTF("Received X mag value is %f\r\n", message.x);
			PRINTF("Received Y mag value is %f\r\n", message.y);
			PRINTF("Received Z mag value is %f\r\n", message.z);
			//vTaskResume(MagTaskHandle);
		}
		//vTaskSuspend(NULL);
	}
	PRINTF("Finished Magnetometer Queue Receiver Task\r\n");
	vTaskSuspend(NULL);
}

void gyro_queue_task(void* pvParameters){
	Data_t message;
	int gyroCounter = 0;
	int counter[3];
	while(gyroCounter < GyroMaxCounter){
		xQueueReceive(xGyroQueue, &message, xBlockTime);
		//PRINTF("Received Measurement %d\r\n", counter);
		PRINTF("Received message from gyroscope %d\r\n", message.id);
		if(message.id == gyroscope1 ){
			PRINTF("Received X gyro value is %f\r\n", message.x);
			PRINTF("Received Y gyro value is %f\r\n", message.y);
			PRINTF("Received Z gyro value is %f\r\n", message.z);
			gyroCounter++;
			vTaskResume(GyroTaskHandle);
		}
		else if(message.id == gyroscope2 ){
			PRINTF("Received X gyro value is %f\r\n", message.x);
			PRINTF("Received Y gyro value is %f\r\n", message.y);
			PRINTF("Received Z gyro value is %f\r\n", message.z);
			//vTaskResume(MagTaskHandle);
		}else if(message.id ==gyroscope3 ){
			PRINTF("Received X gyro value is %f\r\n", message.x);
			PRINTF("Received Y gyro value is %f\r\n", message.y);
			PRINTF("Received Z gyro value is %f\r\n", message.z);
			//vTaskResume(MagTaskHandle);
		}
		//vTaskSuspend(NULL);
	}
	PRINTF("Finished Gyroscope Queue Receiver Task\r\n");
	vTaskSuspend(NULL);
}


