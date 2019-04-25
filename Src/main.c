/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#if defined(__GNUC__)
#include <errno.h>
#include <sys/unistd.h> /* STDOUT_FILENO, STDERR_FILENO */
#endif
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define LCD_FRAME_BUFFER_LAYER0                 (LCD_FRAME_BUFFER+0x130000)
#define LCD_FRAME_BUFFER_LAYER1                 LCD_FRAME_BUFFER
#define CONVERTED_FRAME_BUFFER                  (LCD_FRAME_BUFFER+0x260000)
#define INPUT_IMAGE_WIDTH						28
#define INPUT_IMAGE_HEIGHT 						28
#define LCD_INPUT_IMAGE_WIDTH					240
#define LCD_INPUT_IMAGE_HEIGHT 					240
#define NUM_CLASSES								100
#define LCD_INPUT_IMAGE_SHIFT					210
#define DRAW_IMGAE_X1							0  +10
#define DRAW_IMGAE_X2							240-10
#define DRAW_IMGAE_Y1							0  +10
#define DRAW_IMGAE_Y2							240-10
#define CLEAR_BUTTON_X1							175
#define CLEAR_BUTTON_Y1							290
#define CLEAR_BUTTON_X2						    CLEAR_BUTTON_X1 + 64
#define CLEAR_BUTTON_Y2							CLEAR_BUTTON_Y1 + 50
#define GAME_TIMER_TIME							10
#define GAME_RANDOM_TIME						5
#define START_BUTTON_X1							80
#define START_BUTTON_X2							171
#define START_BUTTON_Y1							150
#define START_BUTTON_Y2							210
typedef struct{
	ai_float prob;
	char* class;
}id_prob;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;

LTDC_HandleTypeDef hltdc;

SPI_HandleTypeDef hspi5;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart1;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */
bool is_main_menu = true;
bool is_random_page = false;
bool is_game_page = false;
bool is_score_page = false;
uint8_t counter = 0;
bool game_over = false;
TS_StateTypeDef screen_state;
const char *dict[] = { "snake", "syringe", "television", "octagon", "diamond", "line", "square", "knife", "campfire",
					   "ear", "lantern", "jail", "airplane", "guitar", "bicycle", "star", "suitcase", "crab", "steak",
					   "cloud", "whale", "zigzag", "donut", "hat", "lighter", "bridge", "truck", "broccoli", "fork",
					   "swan", "leaf", "vase", "cookie", "tree", "sweater", "tent", "bird", "teapot", "dolphin", "chair",
					   "squirrel", "stitches", "hexagon", "foot", "pillow", "key", "banana", "tooth", "door", "boomerang",
					   "sun", "stop sign", "leg", "t-shirt", "microwave", "spider", "belt", "umbrella", "bed", "butterfly",
					   "toilet", "stairs", "oven", "motorbike", "onion", "violin", "cat", "fish", "bee", "nose", "apple",
					   "clock", "toe", "lightning", "dog", "arm", "compass", "stove", "brain", "lollipop", "skull",
					   "triangle", "camera", "finger", "snail", "radio", "axe", "scissors", "cake", "basket", "helmet",
					   "tiger", "sock", "swing set", "flower", "circle", "sink", "spoon", "purse", "mountain"};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA2D_Init(void);
static void MX_LTDC_Init(void);
static void MX_FMC_Init(void);
void MX_USART1_UART_Init(void);
static void MX_I2C3_Init(void);
static void MX_SPI5_Init(void);
static void MX_CRC_Init(void);
static void MX_GFXSIMULATOR_Init(void);
static void MX_TIM6_Init(void);
void Draw_First_Page(void);
void Draw_Random_Page(const char* str);
/* USER CODE BEGIN PFP */
static void Lcd_Init();
static void Touch_init();
void Draw_Menu();
void Reset_Pred();
//void Print_Image(ai_float *image);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance==TIM6){
		if(is_random_page){
			counter--;
			char c[1];
			sprintf(c, "%d", counter);
			BSP_LCD_SetFont(&Font24);
			BSP_LCD_DisplayStringAt(0,220,(uint8_t*) c, CENTER_MODE);
			if(counter == 0 ){
				counter = GAME_TIMER_TIME;
				is_random_page = false;
				is_game_page = true;
				Draw_Menu();
			}
		}
		else if(is_game_page){
				counter--;
				char c[1];
				sprintf(c, "%d", counter);
				BSP_LCD_SetFont(&Font24);
				BSP_LCD_SetBackColor(LCD_COLOR_RED);
				BSP_LCD_DisplayStringAt(0,300,(uint8_t*) c, LEFT_MODE);
				if(counter == 0){
					counter = 0;
					is_game_page = false;
					is_score_page = true;
			}
		}
		BSP_LED_Toggle(LED3);
	}
}


int cmp_ptr(const void *a, const void *b)
{
    const int **left  = (const float **)a;
    const int **right = (const float **)b;

    return (**left < **right) - (**right < **left);
}

size_t * order_int(const float *a, size_t n)
{
    const float **pointers = malloc(n * sizeof(const float *));

    for (size_t i = 0; i < n; i++) pointers[i] = a + i;

    qsort(pointers, n, sizeof(const float *), cmp_ptr);

    size_t *indices = malloc(n * sizeof(size_t));

    for (size_t i = 0; i < n; i++) indices[i] = pointers[i] - a;

    free(pointers);

    return indices;
}



void Reset_Pred(ai_float** in_data, id_prob* first_guess, id_prob* second_guess){
	memset(in_data, 0.0, sizeof(in_data[0][0]) * INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT);
	second_guess->class = '-';
	second_guess->prob = 0.0;
	first_guess->class = '-';
	first_guess->prob = 0.0;

}
int _write(int fd, const void *buff, int count)
{
    HAL_StatusTypeDef status;

    if ((count < 0) && (fd != STDOUT_FILENO) && (fd != STDERR_FILENO)) {
        errno = EBADF;
        return -1;
    }

    status = HAL_UART_Transmit(&huart1, (uint8_t *)buff, count,
            HAL_MAX_DELAY);

    return (status == HAL_OK ? count : 0);
}



void Print_Image(ai_float *image[INPUT_IMAGE_WIDTH][INPUT_IMAGE_HEIGHT][1]){
	printf("\e[1;1H\e[2J");
	for(int i = 0; i < INPUT_IMAGE_WIDTH; ++i) {
		for (int j = 0; j < INPUT_IMAGE_HEIGHT;  ++j) {
			BSP_LCD_DrawPixel(i,j+240,(int)image[i][j][0]);
			printf("%0.2f,", image[i][j][0]);
		}
		printf("\r\n");
	}
}

void Draw_First_Page(){
  BSP_LCD_Clear(LCD_COLOR_ORANGE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&Font20);
  BSP_LCD_DisplayStringAt(0,128,(uint8_t*) "Quick , Draw!", CENTER_MODE);
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_SetBackColor(LCD_COLOR_ORANGE);
  BSP_LCD_DrawRect(80,171,70,30);
  BSP_LCD_DisplayStringAt(0,180,(uint8_t*) "START", CENTER_MODE);
}

void Draw_Random_Page(const char* str){
  BSP_LCD_Clear(LCD_COLOR_ORANGE);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_SetFont(&Font24);
  BSP_LCD_DisplayStringAt(0,128,(uint8_t*) "Draw", CENTER_MODE);
  BSP_LCD_SetFont(&Font20);
  BSP_LCD_SetBackColor(LCD_COLOR_ORANGE);
  BSP_LCD_DisplayStringAt(0,180,(uint8_t*) str, CENTER_MODE);
}


void Draw_Menu(){
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
  BSP_LCD_SetFont(&Font12);
  BSP_LCD_DisplayStringAt(0,245,(uint8_t*) "I SEE:", CENTER_MODE);
  //BSP_LCD_DisplayStringAt(0,270,(uint8_t*) "2st Guess:", LEFT_MODE);
  BSP_LCD_DrawRect(210,240,28,28);
  BSP_LCD_SetTextColor(LCD_COLOR_LIGHTCYAN);
  BSP_LCD_FillRect(175,290,65,50);
  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_SetFont(&Font16);
  BSP_LCD_DisplayStringAt(179,300,(uint8_t*) "CLEAR", LEFT_MODE);
  BSP_LCD_DrawRect(2,2,237,237);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
}

void Lcd_Init(){
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER_LAYER1);
  BSP_LCD_SelectLayer(1);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  BSP_LCD_SetColorKeying(1, LCD_COLOR_DARKCYAN);
  BSP_LCD_SetLayerVisible(1, DISABLE);
  BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER_LAYER0);
  BSP_LCD_SelectLayer(0);
  BSP_LCD_DisplayOn();
  HAL_Delay(100);
  BSP_LCD_Clear(LCD_COLOR_WHITE);
  HAL_Delay(100);
}
void Touch_init(){
  if (BSP_TS_Init(240, 320) == TS_ERROR) {
	printf("BSP_TS_Init error\r\n");
  }
  else {
	printf("BSP_TS_Init OK!\r\n");
  }
  HAL_Delay(100);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  Lcd_Init();
  BSP_PB_Init(BUTTON_KEY,0);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
//  MX_GPIO_Init();
//  MX_DMA2D_Init();
//  MX_LTDC_Init();
//  MX_FMC_Init();
//  MX_I2C3_Init();
//  MX_SPI5_Init();
  MX_CRC_Init();
  MX_X_CUBE_AI_Init();
  //MX_GFXSIMULATOR_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  Touch_init();
  /* initialize input/output buffer handlers */
  ai_float in_data[28][28]= {{0}};
  id_prob first_guess;
  id_prob second_guess;
  first_guess.prob = 0.0;
  second_guess.prob = 0.0;
  char first_guess_str[30];
  char second_guess_str[30];
  ai_float out_data[NUM_CLASSES];
  BSP_LED_Init(LED3);
  HAL_TIM_Base_Start_IT(&htim6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //Draw_Menu();
  Draw_First_Page();
  while (1){

	      BSP_TS_GetState(&screen_state);

	  	  if(screen_state.TouchDetected){
	  	  if((screen_state.X > START_BUTTON_X1 && screen_state.X < START_BUTTON_X2) && (screen_state.Y > START_BUTTON_Y1 && screen_state.Y < START_BUTTON_Y2 ) && is_main_menu){
	  		  counter = GAME_RANDOM_TIME;
	  		  is_main_menu = false;
	  		  time_t t;
	  		  srand((unsigned) HAL_GetTick());
	  		  int random_n = rand() % 100;
	  		  Draw_Random_Page(dict[random_n]);
	  		  is_random_page = true;
	  	  }
	  	  else if((screen_state.X > DRAW_IMGAE_X1 && screen_state.X < DRAW_IMGAE_X2) && (screen_state.Y > DRAW_IMGAE_Y1 && screen_state.Y < DRAW_IMGAE_Y2 ) && is_game_page){
		  		  int x =screen_state.Y*((float)28/240);
		  		  int y = screen_state.X*((float)28/240);
		  		  in_data[x][y] = 0.97;
//		  		  BSP_LCD_DrawPixel(y+1 + LCD_INPUT_IMAGE_SHIFT,LCD_INPUT_IMAGE_HEIGHT+x+1,255);
//		  		  BSP_LCD_DrawPixel(y-1 + LCD_INPUT_IMAGE_SHIFT,LCD_INPUT_IMAGE_HEIGHT+x-1,255);
//		  		  BSP_LCD_DrawPixel(y+1 + LCD_INPUT_IMAGE_SHIFT,LCD_INPUT_IMAGE_HEIGHT+x,255);
//		  		  BSP_LCD_DrawPixel(y-1 + LCD_INPUT_IMAGE_SHIFT,LCD_INPUT_IMAGE_HEIGHT+x,255);
		  		  BSP_LCD_DrawPixel(y + LCD_INPUT_IMAGE_SHIFT,LCD_INPUT_IMAGE_HEIGHT + x, 255);
//		  		  BSP_LCD_DrawPixel(y + LCD_INPUT_IMAGE_SHIFT,LCD_INPUT_IMAGE_HEIGHT+x+1,255);
//		  		  BSP_LCD_DrawPixel(y + LCD_INPUT_IMAGE_SHIFT, LCD_INPUT_IMAGE_HEIGHT+x-1,255);
//		  		  BSP_LCD_DrawPixel(y + 1 + LCD_INPUT_IMAGE_SHIFT,LCD_INPUT_IMAGE_HEIGHT+x-1,255);
//		  		  BSP_LCD_DrawPixel(y - 1 + LCD_INPUT_IMAGE_SHIFT,LCD_INPUT_IMAGE_HEIGHT+x+1,255);
		  		  BSP_LCD_FillCircle(screen_state.X, screen_state.Y, 3);
	  		  }
	  		  else if ((screen_state.X > CLEAR_BUTTON_X1 && screen_state.X < CLEAR_BUTTON_X2) && (screen_state.Y > CLEAR_BUTTON_Y1 && screen_state.Y < CLEAR_BUTTON_Y2 ) ){
	  			Draw_Menu();
	  			Reset_Pred(&in_data,&first_guess,&second_guess);


	  		  }


	  	  }
	  	  if(is_score_page){
	  		HAL_Delay(100);
	  		MX_X_CUBE_AI_Process(in_data,out_data,1);
		    size_t N = sizeof(out_data) / sizeof(*out_data);
		    size_t *indices = order_int(out_data, N);
		    //for (size_t i = 0; i < N; i++) printf("%f \n\r", out_data[indices[i]]);
			sprintf(first_guess_str ,"%s , %s", dict[indices[0]] , dict[indices[1]]);
			sprintf(second_guess_str,"%s , %s , %s", dict[indices[2]] , dict[indices[3]],dict[indices[4]]);
			BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
			BSP_LCD_SetFont(&Font12);
			BSP_LCD_DisplayStringAt(10,270,(uint8_t*) first_guess_str, LEFT_MODE);
			BSP_LCD_DisplayStringAt(10,280,(uint8_t*) second_guess_str, LEFT_MODE);
		    free(indices);
			Reset_Pred(&in_data,&first_guess,&second_guess);
			is_score_page = false;
	  	  }


    /* USER CODE END WHILE */

  //MX_X_CUBE_AI_Process();
    /* USER CODE BEGIN 3 */
	if( BSP_PB_GetState(BUTTON_KEY)){
		printf("\e[1;1H\e[2J");
		//time = HAL_GetTick();
//		for(int i = 0 ; i < 28; ++i){
//			for(int j = 0 ; j < 28; ++j){
//				printf("%f,",in_data[i][j]);
//			}
//			printf("\n\r");
//		}
		HAL_Delay(1);
		MX_X_CUBE_AI_Process(in_data,out_data,1);
		for(int i = 0; i < NUM_CLASSES; ++i){
			printf("class %d  = %f\n\r",i, out_data[i]);
			if(first_guess.prob < out_data[i]){
				second_guess.class = first_guess.class;
				second_guess.prob = first_guess.prob;
				first_guess.prob = out_data[i];
				first_guess.class = dict[i];
				//printf("-%c %f\n\r", first_guess.class , first_guess.prob);
				//printf("-%c %f\n\r", second_guess.class, second_guess.prob);
			}
			else if(second_guess.prob < out_data[i]){
				second_guess.class = dict[i];
				second_guess.prob = out_data[i];
			}
		}
		//time = HAL_GetTick() - time;
		//printf("FINAL %s %f\n\r", first_guess.class , first_guess.prob);
		//printf("FINAL %s%f\n\r", second_guess.class, second_guess.prob);
	    size_t N = sizeof(out_data) / sizeof(*out_data);
	    size_t *indices = order_int(out_data, N);
	    //for (size_t i = 0; i < N; i++) printf("%f \n\r", out_data[indices[i]]);
		sprintf(first_guess_str ,"%s , %s", dict[indices[0]] , dict[indices[1]]);
		sprintf(second_guess_str,"%s , %s , %s", dict[indices[2]] , dict[indices[3]],dict[indices[4]]);
		BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
		BSP_LCD_SetFont(&Font12);
		BSP_LCD_DisplayStringAt(10,270,(uint8_t*) first_guess_str, LEFT_MODE);
		BSP_LCD_DisplayStringAt(10,280,(uint8_t*) second_guess_str, LEFT_MODE);
	    free(indices);
		Reset_Pred(&in_data,&first_guess,&second_guess);

	 }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode 
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  hdma2d.LayerCfg[1].InputOffset = 0;
  hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[1].InputAlpha = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_DMA2D_ConfigLayer(&hdma2d, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief GFXSIMULATOR Initialization Function
  * @param None
  * @retval None
  */
static void MX_GFXSIMULATOR_Init(void)
{

  /* USER CODE BEGIN GFXSIMULATOR_Init 0 */

  /* USER CODE END GFXSIMULATOR_Init 0 */

  /* USER CODE BEGIN GFXSIMULATOR_Init 1 */

  /* USER CODE END GFXSIMULATOR_Init 1 */
  /* USER CODE BEGIN GFXSIMULATOR_Init 2 */

  /* USER CODE END GFXSIMULATOR_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 100000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};
  LTDC_LayerCfgTypeDef pLayerCfg1 = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 7;
  hltdc.Init.VerticalSync = 3;
  hltdc.Init.AccumulatedHBP = 14;
  hltdc.Init.AccumulatedVBP = 5;
  hltdc.Init.AccumulatedActiveW = 654;
  hltdc.Init.AccumulatedActiveH = 485;
  hltdc.Init.TotalWidth = 660;
  hltdc.Init.TotalHeigh = 487;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 0;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 0;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 0;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg.FBStartAdress = 0;
  pLayerCfg.ImageWidth = 0;
  pLayerCfg.ImageHeight = 0;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg1.WindowX0 = 0;
  pLayerCfg1.WindowX1 = 0;
  pLayerCfg1.WindowY0 = 0;
  pLayerCfg1.WindowY1 = 0;
  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg1.Alpha = 0;
  pLayerCfg1.Alpha0 = 0;
  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
  pLayerCfg1.FBStartAdress = 0;
  pLayerCfg1.ImageWidth = 0;
  pLayerCfg1.ImageHeight = 0;
  pLayerCfg1.Backcolor.Blue = 0;
  pLayerCfg1.Backcolor.Green = 0;
  pLayerCfg1.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */

  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 40000;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 2500;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{
  FMC_SDRAM_TimingTypeDef SdramTiming;

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_1;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_DISABLE;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 16;
  SdramTiming.ExitSelfRefreshDelay = 16;
  SdramTiming.SelfRefreshTime = 16;
  SdramTiming.RowCycleDelay = 16;
  SdramTiming.WriteRecoveryTime = 16;
  SdramTiming.RPDelay = 16;
  SdramTiming.RCDDelay = 16;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ACP_RST_GPIO_Port, ACP_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, RDX_Pin|WRX_DCX_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, LD3_Pin|LD4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NCS_MEMS_SPI_Pin CSX_Pin OTG_FS_PSO_Pin */
  GPIO_InitStruct.Pin = NCS_MEMS_SPI_Pin|CSX_Pin|OTG_FS_PSO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : B1_Pin MEMS_INT1_Pin MEMS_INT2_Pin TP_INT1_Pin */
  GPIO_InitStruct.Pin = B1_Pin|MEMS_INT1_Pin|MEMS_INT2_Pin|TP_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ACP_RST_Pin */
  GPIO_InitStruct.Pin = ACP_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ACP_RST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : OTG_FS_OC_Pin */
  GPIO_InitStruct.Pin = OTG_FS_OC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OTG_FS_OC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : OTG_HS_ID_Pin OTG_HS_DM_Pin OTG_HS_DP_Pin */
  GPIO_InitStruct.Pin = OTG_HS_ID_Pin|OTG_HS_DM_Pin|OTG_HS_DP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF12_OTG_HS_FS;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : VBUS_HS_Pin */
  GPIO_InitStruct.Pin = VBUS_HS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VBUS_HS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TE_Pin */
  GPIO_InitStruct.Pin = TE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : RDX_Pin WRX_DCX_Pin */
  GPIO_InitStruct.Pin = RDX_Pin|WRX_DCX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : LD3_Pin LD4_Pin */
  GPIO_InitStruct.Pin = LD3_Pin|LD4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
