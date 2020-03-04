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

TaskHandle_t MagTaskHandle1;
TaskHandle_t MagTaskHandle2;
TaskHandle_t MagTaskHandle3;

TaskHandle_t GyroTaskHandle1;
TaskHandle_t GyroTaskHandle2;
TaskHandle_t GyroTaskHandle3;

TaskHandle_t MagQueTaskHandle;
TaskHandle_t GyroQueTaskHandle;

const TickType_t xBlockTime = portMAX_DELAY;
const int MagMaxCounter = 50;
const int GyroMaxCounter = 75;


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

    xMagQueue = xQueueCreate(5, sizeof(Data_t));
    if(xMagQueue == NULL){
    	PRINTF("Failed in creating queue for magneteomter\r\n");
    	for(;;)
    		;
    }
    vQueueAddToRegistry( xMagQueue, "MagQueue" );

    xGyroQueue = xQueueCreate(5, sizeof(Data_t));
    if(xGyroQueue == NULL){
    	PRINTF("Failed in creating queue for gyroscope\r\n");
    	for(;;)
    		;
    }
    vQueueAddToRegistry( xGyroQueue, "GyroQueue" );


    PRINTF("TESTING MAGNETOMETER WITH SPI\r\n");
    PRINTF("\r\n");

    SensorID magID1 = magnetometer1;
    SensorID magID2 = magnetometer2;
    SensorID magID3 = magnetometer3;
    SensorID gyroID1 = gyroscope1;
    SensorID gyroID2 = gyroscope2;
    SensorID gyroID3 = gyroscope3;

    if (xTaskCreate(magnetometer_task, "magnetometer_task1", configMINIMAL_STACK_SIZE + 128, (void*)magID1, master_task_PRIORITY, &MagTaskHandle1) !=
            pdPASS)
        {
            PRINTF("Magnetometer Task 1 creation failed!.\r\n");
        }
    if (xTaskCreate(magnetometer_dummy_task, "magnetometer_task2", configMINIMAL_STACK_SIZE + 128, (void*)magID2, master_task_PRIORITY, &MagTaskHandle2) !=
            pdPASS)
        {
            PRINTF("MagnetometerTask 2 creation failed!.\r\n");
        }
    if (xTaskCreate(magnetometer_dummy_task, "magnetometer_task3", configMINIMAL_STACK_SIZE + 128, (void*)magID3, master_task_PRIORITY, &MagTaskHandle3) !=
            pdPASS)
        {
            PRINTF("Magnetometer Task 3 creation failed!.\r\n");
        }
    if (xTaskCreate(gyroscope_task, "gyroscope_task1", configMINIMAL_STACK_SIZE + 128, (void*)gyroID1, master_task_PRIORITY, &GyroTaskHandle1) !=
			pdPASS)
		{
			PRINTF("Gyroscope Task 1 creation failed!.\r\n");
		}
    if (xTaskCreate(gyroscope_task, "gyroscope_task2", configMINIMAL_STACK_SIZE + 128, (void*)gyroID2, master_task_PRIORITY, &GyroTaskHandle2) !=
			pdPASS)
		{
			PRINTF("Gyroscpe Task 2 creation failed!.\r\n");
		}
    if (xTaskCreate(gyroscope_task, "gyroscope_task3", configMINIMAL_STACK_SIZE + 128, (void*)gyroID3, master_task_PRIORITY, &GyroTaskHandle3) !=
			pdPASS)
		{
			PRINTF("Gyroscope Task 3 creation failed!.\r\n");
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
	//int magCounter = 0;
	bool magFlag1 = true;
	bool magFlag2 = true;
	bool magFlag3 = true;
	while(magFlag1 || magFlag2 || magFlag3){
		xQueueReceive(xMagQueue, &message, xBlockTime);
		//PRINTF("Received Measurement %d\r\n", counter);
		PRINTF("Received message from magnetometer %d\r\n", message.id);
		if(message.id == magnetometer1 ){
			magFlag1 = message.flag;
			if(magFlag1){
				PRINTF("Received X mag value is %f\r\n", message.x);
				PRINTF("Received Y mag value is %f\r\n", message.y);
				PRINTF("Received Z mag value is %f\r\n", message.z);

				vTaskResume(MagTaskHandle1);
			}else{
				PRINTF("Received finish flag for magnetometer %d\r\n", message.id);
			}
		}
		else if(message.id == magnetometer2 ){
			magFlag2 = message.flag;
			if(magFlag2){
				PRINTF("Received X mag value is %f\r\n", message.x);
				PRINTF("Received Y mag value is %f\r\n", message.y);
				PRINTF("Received Z mag value is %f\r\n", message.z);

				vTaskResume(MagTaskHandle2);
			}else{
				PRINTF("Received finish flag for magnetometer %d\r\n", message.id);
			}
		}else if(message.id == magnetometer3 ){
			magFlag3 = message.flag;
			if(magFlag3){
				PRINTF("Received X mag value is %f\r\n", message.x);
				PRINTF("Received Y mag value is %f\r\n", message.y);
				PRINTF("Received Z mag value is %f\r\n", message.z);
				vTaskResume(MagTaskHandle3);
			}else{
				PRINTF("Received finish flag for magnetometer %d\r\n", message.id);
			}
		}
	}
	PRINTF("Finished Magnetometer Queue Receiver Task\r\n");
	vTaskSuspend(NULL);
}

void gyro_queue_task(void* pvParameters){
	Data_t message;
	bool gyroFlag1 = true;
	bool gyroFlag2 = true;
	bool gyroFlag3 = true;
	while(gyroFlag1 || gyroFlag2 || gyroFlag3){
		xQueueReceive(xGyroQueue, &message, xBlockTime);
		PRINTF("Received message from gyroscope %d\r\n", message.id);
		if(message.id == gyroscope1){
			gyroFlag1 = message.flag;
			if(gyroFlag1){
				PRINTF("Received X gyro value is %f\r\n", message.x);
				PRINTF("Received Y gyro value is %f\r\n", message.y);
				PRINTF("Received Z gyro value is %f\r\n", message.z);
				vTaskResume(GyroTaskHandle1);
			}else{
				PRINTF("Received finish flag for gyroscope %d\r\n", message.id);
			}
		}
		else if(message.id == gyroscope2 ){
			gyroFlag2 = message.flag;
			if(gyroFlag2){
				PRINTF("Received X gyro value is %f\r\n", message.x);
				PRINTF("Received Y gyro value is %f\r\n", message.y);
				PRINTF("Received Z gyro value is %f\r\n", message.z);
				vTaskResume(GyroTaskHandle2);
			}else{
				PRINTF("Received finish flag for gyroscope %d\r\n", message.id);
			}
		}else if(message.id ==gyroscope3 ){
			gyroFlag3 = message.flag;
			if(gyroFlag3){
				PRINTF("Received X gyro value is %f\r\n", message.x);
				PRINTF("Received Y gyro value is %f\r\n", message.y);
				PRINTF("Received Z gyro value is %f\r\n", message.z);
				vTaskResume(GyroTaskHandle3);
			}else{
				PRINTF("Received finish flag for gyroscope %d\r\n", message.id);
			}
		}
	}
	PRINTF("Finished Gyroscope Queue Receiver Task\r\n");
	vTaskSuspend(NULL);
}


