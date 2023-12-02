
#include "main.h"
#include "stdbool.h"

#include "stdlib.h"

// Static functions
static void SystemClock_Config(void);

// declareting functions
void subscribe(char *p);
// void init_vet();
// void print_queue();
// void att_update_vet(command_message_t *message);// troca

void actualize_sensors(char c);
void print_sensor();

// FreeRTOS tasks
void vTask1(void *pvParameters);
void vTask2(void *pvParameters);
void vTask_Write(void *pvParameters);

// Kernel objects
xSemaphoreHandle xSem1, xSem2, xTask_Pere, xSem_DMA_TC;
xQueueHandle xConsoleQueue, xPublishQueue;

// variables
command_message_t queue;
typedef uint8_t message_t[60];

int count = 0;
uint8_t rx_dma_buffer[8];
uint8_t rx_dma_irq = 0;
uint8_t tx_dma_buffer[SIZE_DMA_CHANNEL_5];
uint8_t sensor[SENSOR_TABLE_SIZE];

void delay_ms(uint32_t milliseconds)
{
	const TickType_t ticks = pdMS_TO_TICKS(milliseconds);
	vTaskDelay(ticks);
}
// Main program
int main()
{
	// Configure System Clock
	SystemClock_Config();

	// Initialize LED pin
	BSP_LED_Init();

	// Initialize the user Push-Button
	BSP_PB_Init();

	// Initialize Debug Console
	BSP_Console_Init();

	// Initialize NVIC
	BSP_NVIC_Init();

	// Set priority level 1 for DMA1_Channel5 interrupts
	// NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, 1);

	// Enable DMA1_Channel5 interrupts
	// NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);
	// Start Trace Recording
	vTraceEnable(TRC_START);
	// Create Queue to hold console messages
	xConsoleQueue = xQueueCreate(10, sizeof(message_t *));
	xPublishQueue = xQueueCreate(10, sizeof(message_t *));

	// Give a nice name to the Queue in the trace recorder
	vTraceSetQueueName(xConsoleQueue, "Console Queue");
	// Create Semaphore object (this is not a 'give')
	xSem1 = xSemaphoreCreateBinary();
	xSem2 = xSemaphoreCreateBinary();
	// xSemTC = xSemaphoreCreateBinary();
	xTask_Pere = xSemaphoreCreateMutex();
	xSem_DMA_TC = xSemaphoreCreateBinary();
	// init_vet();

	xTaskCreate(vTask1, "Task_1", 256, NULL, 1, NULL);
	xTaskCreate(vTask2, "Task_2", 256, NULL, 1, NULL);
	xTaskCreate(vTask_Write, "vTask_Write", 256, NULL, 1, NULL);

	vTaskStartScheduler();
	// my_printf("SYSCLK = %2d Hz \r\n",SystemCoreClock);
	while (1)
	{
		// The program should never be here...
	}
}

void vTask1(void *pvParameters)
{
	message_t carac;
	message_t *pcarac;
	portBASE_TYPE xStatus;
	xSemaphoreTake(xSem1, 0);
	while (1)
	{
		xStatus = xSemaphoreTake(xSem1, 500);
		if (xStatus == pdPASS)
		{

			// subscribe("Arthour ! Pas changer assiette pour fromage !\r\n");
			my_sprintf((char *)carac, "Arthour ! Pas changer assiette pour fromage !\r\n");
			pcarac = &carac;

			// Send message to Console Queue
			// pm = &carac;
			if (xQueueSendToBack(xPublishQueue, &pcarac, 0) == pdTRUE)
			{
				count++;
			}
			// task1_ready = true;
			vTaskDelay(4000 / portTICK_RATE_MS);
			// delay_ms(5000);
		}
		xSemaphoreGive(xSem1);
	}
}

void vTask2(void *pvParameters)
{
	message_t carac;
	message_t *pcarac;
	portBASE_TYPE xStatus;
	xSemaphoreTake(xSem2, 0);
	while (1)
	{
		xStatus = xSemaphoreTake(xSem2, 500);
		if (xStatus == pdPASS)
		{

			//subscribe("Arthour ! Couhillere !\r\n");
			my_sprintf((char *)carac, "Arthour ! Couhillere !\r\n");
			pcarac = &carac;

			// Send message to Console Queue
			// pm = &carac;
			if (xQueueSendToBack(xPublishQueue, &pcarac, 0) == pdTRUE)
			{
				count++;
			}
			// task2_ready = true;
			vTaskDelay(4000 / portTICK_RATE_MS);
			// delay_ms(4000);
		}
		xSemaphoreGive(xSem2);
	}
}
/*
void vTask_Write(void *pvParameters ) {
	portTickType xLastWakeTime;
	portBASE_TYPE xStatus;
	command_message_t *message ;
	xLastWakeTime = xTaskGetTickCount();
	xSemaphoreTake(xTask_Pere,0);
	while(1){
		xStatus = xSemaphoreTake(xTask_Pere, 200);
		if(xStatus == pdPASS){
			BSP_LED_Toggle();
		}
		xSemaphoreGive(xTask_Pere);

		if ( (USART2->ISR & USART_ISR_RXNE) == USART_ISR_RXNE ){
		// Read and report the content of RDR
			char rx_byte = USART2->RDR;
			actualize_sensors(rx_byte);
		}
		if(xQueueReceive(xConsoleQueue, &message, 2)){
				my_printf("[%s]\r\n",message);
				//att_update_vet(message);
				//print_queue();
		}
		else
			my_printf(".");
		vTaskDelayUntil(&xLastWakeTime,(200/portTICK_RATE_MS));
	}
}
*/

// In French, is the definition on TP3.
void vTask_Write(void *pvParameters)
{
	message_t *message;
	uint8_t cnt = 0;

	// Set priority level 1 for DMA1 interrupt -> reading the sensor values is our highest priority
	NVIC_SetPriority(DMA1_Channel4_5_6_7_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1);

	// Enable DMA1 interrupts
	NVIC_EnableIRQ(DMA1_Channel4_5_6_7_IRQn);

	while (1)
	{
		xQueueReceive(xPublishQueue, &message, portMAX_DELAY);
		// The message was taken as expected

		cnt = 0;
		for (uint8_t i = 0; i < SIZE_DMA_CHANNEL_5; i++)
		{
			cnt++;
			if ((*message)[i] != '\0')
				tx_dma_buffer[i] = (*message)[i];
			else
				break;
		}
		// Indiquez au DMA la taille CNDTR = n du transfert. Cette opération doit être faite avec le canal DMA désactivé
		//  Set Memory Buffer size
		DMA1_Channel4->CNDTR = cnt;
		// Activez le canal DMA
		// Enable DMA1 Channel 4
		DMA1_Channel4->CCR |= DMA_CCR_EN;
		// Activez l’appel au DMA (request) de l’USART2 TX
		// Enable USART2 DMA Request on TX
		USART2->CR3 |= USART_CR3_DMAT;

		// Attendez (take) le sémaphore xSem_DMA_TC avec un timeout infini.
		xSemaphoreTake(xSem_DMA_TC, portMAX_DELAY);
		// Désactivez le canal DMA
		//  Disable DMA1 Channel 4
		DMA1_Channel4->CCR &= ~DMA_CCR_EN;
		// Désactivez l’appel au DMA de l’USART2 TX
		//  Disable USART2 DMA Request on TX
		USART2->CR3 &= ~USART_CR3_DMAT;
	}
}

void subscribe(char *p)
{
	if (count < SIZE_DMA_CHANNEL_5)
	{ // Garante que não ultrapasse o tamanho máximo da queue
		/*uint8_t *carac = (uint8_t *) malloc(sizeof(p) * sizeof(uint8_t));
		if (carac == NULL) {
			my_printf("\r\nMemory allocation failed.\r\n");
			return;
		}

		strncpy((char*)carac, p, sizeof(p));

		command_message_t *pm = (command_message_t*) carac;
*/
		// Prepare message
		// uint8_t *carac = (uint8_t *)malloc(sizeof(p) * sizeof(uint8_t));
		message_t carac;
		message_t *pcarac;
		my_sprintf((char *)carac, p);
		pcarac = &carac;

		// Send message to Console Queue
		// pm = &carac;
		if (xQueueSendToBack(xPublishQueue, &pcarac, 0) == pdTRUE)
		{
			count++;
		}
		/*else
		{
			// my_printf("\r\nFailed to send message to the queue.\r\n");
			free(carac); // Free allocated memory in case of failure
		}*/
		/*} else {
			my_printf("\r\nQueue is full, cannot subscribe.\r\n");

		}*/
	}
}

/*
void subscribe(char *p) {

	if (count < SIZE_TABLEAU) {  // Garante que não ultrapasse o tamanho máximo da queue

		//command_message_t	*pm = (command_message_t* )malloc( sizeof(pm));

		uint8_t *carac = (uint8_t *) malloc(sizeof(SV));
		strncpy(carac,p,SV);
		command_message_t *pm  = (command_message_t*) &carac;
		xQueueSendToBack(xConsoleQueue, pm, 0);
		count++;
	} else {
		my_printf("\r\nQueue is full, cannot subscribe.\r\n");
	}
}
*/
/*
void att_update_vet(command_message_t *message){
	if ( queue != '\0')
		strncpy(queue,message,SIZE_DMA_CHANNEL_5);
}*/

/*
void init_vet(){
	for(int i=0;i<SIZE_TABLEAU;i++){
		memset(queue,'\0',SIZE_TABLEAU);
	}
}*/
/*
void print_queue(){
	my_printf("\r\nTable is:\r\n");
	for(int i=0; i< SIZE_TABLEAU;i++)
		my_printf("%2d %2d %2d \r\n",queue[i].sem_id, queue[i].sensor_id,queue[i].sensor_state);

}
*/
/*void actualize_sensors(char c)
{
	switch (c)
	{
	case 'a':
		sensor[0] = 0;
		break;
	case 'b':
		sensor[0] = 1;
		break;
	case 'c':
		sensor[1] = 0;
		break;
	case 'd':
		sensor[1] = 1;
		break;
	}
	print_sensor();
}

void print_sensor()
{
	for (int i = 0; i < SENSOR_TABLE_SIZE - 1; i++)
		my_printf("\r\nSensors state = [ %2d %2d ]\r\n", sensor[i], sensor[i + 1]);
}*/
/*
 * Clock configuration for the Nucleo STM32F072RB board
 * HSE input Bypass Mode            -> 8MHz
 * SYSCLK, AHB, APB1                -> 48MHz
 * PA8 as MCO with /16 prescaler    -> 3MHz
 */

static void SystemClock_Config()
{
	uint32_t HSE_Status;
	uint32_t PLL_Status;
	uint32_t SW_Status;
	uint32_t timeout = 0;

	timeout = 1000000;

	// Start HSE in Bypass Mode
	RCC->CR |= RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;

	// Wait until HSE is ready
	do
	{
		HSE_Status = RCC->CR & RCC_CR_HSERDY_Msk;
		timeout--;
	} while ((HSE_Status == 0) && (timeout > 0));

	// Select HSE as PLL input source
	RCC->CFGR &= ~RCC_CFGR_PLLSRC_Msk;
	RCC->CFGR |= (0x02 << RCC_CFGR_PLLSRC_Pos);

	// Set PLL PREDIV to /1
	RCC->CFGR2 = 0x00000000;

	// Set PLL MUL to x6
	RCC->CFGR &= ~RCC_CFGR_PLLMUL_Msk;
	RCC->CFGR |= (0x04 << RCC_CFGR_PLLMUL_Pos);

	// Enable the main PLL
	RCC->CR |= RCC_CR_PLLON;

	// Wait until PLL is ready
	do
	{
		PLL_Status = RCC->CR & RCC_CR_PLLRDY_Msk;
		timeout--;
	} while ((PLL_Status == 0) && (timeout > 0));

	// Set AHB prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	// Set APB1 prescaler to /1
	RCC->CFGR &= ~RCC_CFGR_PPRE_Msk;
	RCC->CFGR |= RCC_CFGR_PPRE_DIV1;

	// Enable FLASH Prefetch Buffer and set Flash Latency
	FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;

	// Select the main PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait until PLL becomes main switch input
	do
	{
		SW_Status = (RCC->CFGR & RCC_CFGR_SWS_Msk);
		timeout--;
	} while ((SW_Status != RCC_CFGR_SWS_PLL) && (timeout > 0));

	// Update SystemCoreClock global variable
	SystemCoreClockUpdate();
}
