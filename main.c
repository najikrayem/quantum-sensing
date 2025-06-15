/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024.12.5. STMicroelectronics-RongChen.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ads1115.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

#define MAGNETIC_MODULE_1 0        // 磁传感检测模块1
#define MAGNETIC_MODULE_2 1        // 磁传感检测模块2
#define PERSON_NOT_DETECTED 0      // 毫米波传感器   0 没检测到人
#define PERSON_DETECTED 1          // 毫米波传感器   1 检测到人
#define ALARM_THRESHOLD_100 100    // 警报数目阈值为 100
#define ALARM_NUMBER_0 0           // 警报数目为 0
#define IGNORE_PERSON_DETECT 0     // 忽略毫米波传感器
#define SET_NUM_0 0                //
#define SENSOR_COUNT_THRESHOLD_5 5 //

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern uint8_t ADS1115_rawValue[2];

// 新增变量yan
static uint16_t USART_RX_STA;    // 接收状态标记
static uint8_t USART_RX_BUF[50]; // 接收缓冲,最大50字节.
extern uint8_t aRxBuffer[1];     // HAL库使用的串口接收缓冲
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
float KalmanFilter(float inData);
float KalmanFilter_2(float inData);
/* USER CODE BEGIN PFP */
// static void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
卡尔曼滤波函数

*/
float KalmanFilter(float inData)
{
    static float prevFilteredData = 4840.0; // 2420
    static float estErrCov = 10, procNoiseCov = 0.009, measNoiseCov = 0.009, kGain = 0;
    estErrCov = estErrCov + procNoiseCov;
    kGain = estErrCov / (estErrCov + measNoiseCov);
    inData = prevFilteredData + (kGain * (inData - prevFilteredData));
    estErrCov = (1 - kGain) * estErrCov;
    prevFilteredData = inData;
    return inData;
}
float KalmanFilter_2(float inData)
{
    static float prevFilteredData = 4840.0; // 2420
    static float estErrCov = 10, procNoiseCov = 0.009, measNoiseCov = 0.009, kGain = 0;
    estErrCov = estErrCov + procNoiseCov;
    kGain = estErrCov / (estErrCov + measNoiseCov);
    inData = prevFilteredData + (kGain * (inData - prevFilteredData));
    estErrCov = (1 - kGain) * estErrCov;
    prevFilteredData = inData;
    return inData;
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
    /* USER CODE BEGIN 1 */
    static uint8_t sBuf[500];
    static unsigned char sLen = 0;
    static unsigned char channelFlag = MAGNETIC_MODULE_1;
    static unsigned char humDetectFlag = PERSON_NOT_DETECTED;
    static uint16_t alarmCountAndFlag = 0; // 剩余alarm num
    static uint8_t time100msCount, time500msCount, time1sCount, time10sCount;
    static unsigned char enHUM = 0; // 拨码开关
    float voltageRead;
    uint8_t configDevial = 0x0;

    static uint8_t sensor_1_count = 0;
    static uint8_t sensor_2_count = 0;

    static float adcVol1;
    static float adcBase1 = 0;
    static float adcSum1 = 0;
    static float adcDevial = 60;

    static float adcVol2;
    static float adcBase2 = 0;
    static float adcSum2 = 0;
    static float adcDevia2 = 60;

    uint8_t len;
    uint8_t t;
    uint16_t received_value;
    uint8_t RX_num;
    uint8_t print_flag = 0;
    static float adcVol3;
    static float adcVol4;
    uint8_t waveform_test = 0;
    uint8_t buzzer_test = 0;
    uint8_t buzzer_test_count = 0;

    //  static uint16_t adcBuf[4];
    //  static uint16_t adcTmp1[8];
    //  static uint16_t adcBase1 = 0;
    //  static uint32_t adcSum1 = 0;
    //  static uint32_t adcDeviation = 80;
    //  uint16_t adcValue1 = adcBuf[0];
    //  uint16_t adcValue2 = adcBuf[1];
    //  uint16_t adcValue3 = adcBuf[2];
    //  uint16_t adcValue4 = adcBuf[3];

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    MX_ADC1_Init();
    MX_I2C1_Init();
    /* USER CODE BEGIN 2 */
    //

    // start system
    sprintf((char *)sBuf, "Starting system ...\n");
    sLen = strlen((char *)sBuf);
    HAL_UART_Transmit(&huart1, sBuf, sLen, 30);
    // 拉低所有led
    HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BUZZER2_GPIO_Port, BUZZER2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_GREEN_GPIO_Port, LED2_GREEN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_RED_GPIO_Port, LED2_RED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_SET);
    // 测试 buzzer
    HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUZZER2_GPIO_Port, BUZZER2_Pin, GPIO_PIN_RESET);
    HAL_Delay(400);
    HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BUZZER2_GPIO_Port, BUZZER2_Pin, GPIO_PIN_SET);

    // 调试人体
    //  while(1)
    //  {
    //    if(HAL_GPIO_ReadPin(HUM_GPIO_Port, HUM_Pin))
    //    {
    //      HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_RESET);
    //    }else{
    //      HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_SET);
    //    }
    //  }

    HAL_Delay(500);
    // 初始化ads1115
    if (ADS1115_Init(&hi2c1, ADS1115_DATA_RATE_475, ADS1115_PGA_ONE) == HAL_OK)
    {
        // Device found.
        ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
        HAL_Delay(300);
        ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
    }
    else
    {
        // Device cannot found.
        while (1)
        {
            // 死循环报错，ad异常
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED2_RED_GPIO_Port, LED2_RED_Pin, GPIO_PIN_RESET);
            HAL_Delay(300);
            HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED2_RED_GPIO_Port, LED2_RED_Pin, GPIO_PIN_SET);
            HAL_Delay(300);
        }
    }

    // 获取配置跳线
    configDevial = HAL_GPIO_ReadPin(CONF2_GPIO_Port, CONF2_Pin);
    configDevial = configDevial << 1;
    configDevial = configDevial + HAL_GPIO_ReadPin(CONF1_GPIO_Port, CONF1_Pin);
    configDevial = configDevial << 1;
    configDevial = configDevial + HAL_GPIO_ReadPin(CONF0_GPIO_Port, CONF0_Pin);
    enHUM = !HAL_GPIO_ReadPin(CONF3_GPIO_Port, CONF3_Pin);
    switch (configDevial)
    {
    case 0x00:
        adcDevial = 36864;
        break;
    case 0x01:
        adcDevial = 28224;
        break;
    case 0x02:
        adcDevial = 20736;
        break;
    case 0x03:
        adcDevial = 14400;
        break;
    case 0x04:
        adcDevial = 6000;
        break;
    case 0x05:
        adcDevial = 5000;
        break;
    case 0x06:
        adcDevial = 4000; //[(6n)^2]*16
        break;
    case 0x07:
        adcDevial = 3000;
        break;
    default:
        adcDevial = 4608; // 几何平均
    }

    sprintf((char *)sBuf, "Config Devial: %#x, ADC Devial: %f\n", configDevial, adcDevial);
    sLen = strlen((char *)sBuf);
    HAL_UART_Transmit(&huart1, sBuf, sLen, 100);

    // 等待TMR上电稳定，灯光秀
    for (uint8_t i = 0; i < 8; i++) // 加一次循环
    {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED2_GREEN_GPIO_Port, LED2_GREEN_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED2_GREEN_GPIO_Port, LED2_GREEN_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED2_RED_GPIO_Port, LED2_RED_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LED2_RED_GPIO_Port, LED2_RED_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
    }

    // 获取多个当前环境稳定值，作为基准参考
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_RESET);
    for (uint8_t i = 0; i < 20; i++)
    {
        // HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        // HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_RESET);
        // HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_RESET);
        HAL_Delay(100);
        //    ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
        //    ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
        if (ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1) == HAL_OK)
        {
            adcSum1 = adcSum1 + adcVol1;
        }
        HAL_Delay(100);
        //    ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
        //    ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
        if (ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2) == HAL_OK)
        {
            adcSum2 = adcSum2 + adcVol2;
        }
        //    HAL_ADC_Start(&hadc1);
        //    HAL_ADC_PollForConversion(&hadc1, 10);
        //    adcValue1 = (uint16_t)HAL_ADC_GetValue(&hadc1);
        //    HAL_ADC_Stop(&hadc1);
        //    adcSum1 = adcSum1+adcValue1;
        // HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_SET);
        // HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_SET);
        HAL_Delay(100);
    }
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_SET);

    // 通道1的稳定
    adcBase1 = adcSum1 / 20;
    adcSum1 = SET_NUM_0;
    sprintf((char *)sBuf, "ADC_Base1 = %f mV\r\n", adcBase1);
    sLen = strlen((char *)sBuf);
    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
    // 通道2的稳定
    adcBase2 = adcSum2 / 20;
    adcSum2 = SET_NUM_0;
    sprintf((char *)sBuf, "ADC_Base2 = %f mV\r\n", adcBase2);
    sLen = strlen((char *)sBuf);
    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);

    // 采集基准完毕鸣叫一下
    HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(BUZZER2_GPIO_Port, BUZZER2_Pin, GPIO_PIN_RESET);
    HAL_Delay(1000); // 延长完毕叫声
    HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(BUZZER2_GPIO_Port, BUZZER2_Pin, GPIO_PIN_SET);

    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {
        if (1)
        {
        /*****************串口调试****************/
        begin:                         // 测试用
            if (USART_RX_STA & 0x8000) // 串口接收到数据
            {
                len = USART_RX_STA & 0x3fff; // 得到此次接收到的数据长度
                sprintf((char *)sBuf, "RCV:");
                sLen = strlen((char *)sBuf);
                HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                HAL_UART_Transmit(&huart1, (uint8_t *)USART_RX_BUF, len, 300);
                USART_RX_STA = 0;
                RX_num = 2; // 计算串口收到的值,从第3个数组开始
                while (RX_num < 6)
                {
                    received_value = received_value * 10 + (USART_RX_BUF[RX_num] - 0x30);
                    RX_num++;
                }
                sprintf((char *)sBuf, "-value=%d-\r\n", received_value);
                sLen = strlen((char *)sBuf);
                HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                switch (USART_RX_BUF[0]) // 匹配输入指令
                {
                case 0x41: // A=修改adcDevial
                    adcDevial = received_value;
                    sprintf((char *)sBuf, "adcDevial=%d\r\n", received_value);
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                case 0x42: // B=修改adcBase1
                    adcBase1 = received_value;
                    sprintf((char *)sBuf, "adcBase1=%d\r\n", received_value);
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                case 0x43: // C=修改adcBase2
                    adcBase2 = received_value;
                    sprintf((char *)sBuf, "adcBase2=%d\r\n", received_value);
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                case 0x44: // D=开启波形打印
                    print_flag = 1;
                    sprintf((char *)sBuf, "print_wave\r\n");
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                case 0x53: // S=关闭波形打印
                    print_flag = 0;
                    sprintf((char *)sBuf, "print_stop\r\n");
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                case 0x57: // W=开启波形测试
                    waveform_test = 1;
                    sprintf((char *)sBuf, "waveform_test_ON\r\n");
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                case 0x45: // E=关闭波形测试
                    waveform_test = 0;
                    sprintf((char *)sBuf, "waveform_test_OFF\r\n");
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                case 0x55: // U=开启蜂鸣器测试
                    buzzer_test = 1;
                    sprintf((char *)sBuf, "buzzer_test_ON\r\n");
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                case 0x49: // I=关闭蜂鸣器测试
                    buzzer_test = 0;
                    sprintf((char *)sBuf, "buzzer_test_OFF\r\n");
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    break;
                default:;
                }
                received_value = 0;                            // 清空接收值
                memset(USART_RX_BUF, 0, sizeof(USART_RX_BUF)); // 清空数组
            }
            if (print_flag == 1)
            {
                sprintf((char *)sBuf, "l1=%d,l2=%d,l3=%d,l4=%d,", (int)adcVol1, (int)adcVol2, (int)(adcBase1 + adcDevial), (int)(adcBase2 - adcDevial));
                sLen = strlen((char *)sBuf);
                HAL_UART_Transmit(&huart1, sBuf, sLen, 100);
            }
            if (buzzer_test == 1)
            {
                buzzer_test_count++;
                if (buzzer_test_count == 50)
                {
                    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_RESET);
                }
                if (buzzer_test_count == 100)
                {
                    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_SET);
                    buzzer_test_count = 0;
                }
            }
            while (waveform_test == 1)
            {
                ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                HAL_Delay(2);
                adcVol2 = KalmanFilter(adcVol1); // 卡尔曼滤波
                ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol3);
                HAL_Delay(2);
                adcVol4 = KalmanFilter(adcVol3); // 卡尔曼滤波
                sprintf((char *)sBuf, "l1=%d,l2=%d,l3=%d,l4=%d,", (int)adcVol1, (int)adcVol2, (int)adcVol3, (int)adcVol4);
                sLen = strlen((char *)sBuf);
                HAL_UART_Transmit(&huart1, sBuf, sLen, 100);
                goto begin;
            }
            /*****************串口调试****************/
            /***************/
            if (HAL_GPIO_ReadPin(HUM_GPIO_Port, HUM_Pin) && (humDetectFlag == PERSON_NOT_DETECTED))
            {
                // 循环检测到人，或者配置成一直检测
                humDetectFlag = PERSON_DETECTED;
            }
            else if (!HAL_GPIO_ReadPin(HUM_GPIO_Port, HUM_Pin && humDetectFlag == PERSON_DETECTED))
            {
                // 循环没检测到人
                humDetectFlag = PERSON_NOT_DETECTED;
            }

            if ((PERSON_NOT_DETECTED == humDetectFlag) && enHUM && (ALARM_NUMBER_0 == alarmCountAndFlag))
            {
                // 循环没检测到人也没报警
                HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_SET); // G
                HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_SET);
                //
                //		 for(uint8_t i=0; i<20; i++)
                //			{
                //				if(ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1) == HAL_OK)
                //				{
                //					adcSum1 = adcSum1 + adcVol1;
                //				}
                //				HAL_Delay(10);
                //				if(ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2) == HAL_OK)
                //				{
                //					adcSum2 = adcSum2 + adcVol2;
                //				}
                //			}
                //
                //			// 通道1的稳定
                //			adcBase1 = adcSum1/20;
                //			adcSum1 = 0;
                //			sprintf((char*)sBuf,"ADC_Base1 = %f mV\r\n", adcBase1);
                //			sLen=strlen((char*)sBuf);
                //			HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                //			// 通道2的稳定
                //			adcBase2 = adcSum2/20;
                //			adcSum2 = 0;
                //			sprintf((char*)sBuf,"ADC_Base2 = %f mV\r\n", adcBase2);
                //			sLen=strlen((char*)sBuf);
                //			HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
            }

            // 条件1: 检测到人 或者 enHUM 为 0
            // 条件2: alarmCountAndFlag 为 0
            // 条件3: channelFlag 为磁模块1

            if ((PERSON_DETECTED == humDetectFlag || IGNORE_PERSON_DETECT == enHUM) &&
                (ALARM_NUMBER_0 == alarmCountAndFlag) && (MAGNETIC_MODULE_1 == channelFlag))
            {
                // 通道1开启检测，非报警状态
                HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_RESET); // GREEN
                HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_RESET);

                sensor_1_count++;
                // 通道1，读取两遍防止切换通道异常
                //      ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                //      HAL_Delay(1);
                //      ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                //      ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                if (ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1) == HAL_OK)
                {
                    adcVol1 = KalmanFilter(adcVol1); // 卡尔曼滤波
                    // remove shake
                    adcSum1 = (adcBase1 - adcVol1) * (adcBase1 - adcVol1) + adcSum1;
                    //          sprintf((char*)sBuf,"ADC1 = %f mV\r\n", adcVol1);
                    //          sLen=strlen((char*)sBuf);
                    //          HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                }
                else
                {
                    // error
                    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_SET); // GREEN
                    HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_RESET);
                }

                if (sensor_1_count == SENSOR_COUNT_THRESHOLD_5)
                {
                    channelFlag = MAGNETIC_MODULE_2;
                }

                if (sensor_1_count == SENSOR_COUNT_THRESHOLD_5)
                { // 10
                    // count detect 60 time
                    sensor_1_count = SET_NUM_0;
                    humDetectFlag = PERSON_NOT_DETECTED;
                    // HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_SET);
                    //  通道1 Alarm and out
                    if (adcSum1 / 60 > adcDevial)
                    {
                        alarmCountAndFlag = ALARM_THRESHOLD_100;
                        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
                        // HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_RESET);
                        // HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_SET);
                        HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_RESET);

                        sprintf((char *)sBuf, "A Base1 = %f mV ADCbase=%f mV ADC1 = %f mV\r\n", adcBase1, adcSum1, adcSum1 / 60);
                        sLen = strlen((char *)sBuf);
                        HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    }
                    adcSum1 = 0;
                }
            }

            if ((PERSON_NOT_DETECTED == humDetectFlag) && (ALARM_NUMBER_0 == alarmCountAndFlag) && (MAGNETIC_MODULE_1 == channelFlag))
            {
                // 通道1开启modification，非报警状态
                sensor_1_count++;
                if (ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1) == HAL_OK)
                {
                    adcVol1 = KalmanFilter(adcVol1); // 卡尔曼滤波+强化学习(dog)
                    // remove shake
                    adcSum1 = (adcBase1 - adcVol1) * (adcBase1 - adcVol1) + adcSum1;
                }
                if (sensor_1_count == 15) // 10 < 15 < 20
                {
                    channelFlag = MAGNETIC_MODULE_2;
                    sensor_1_count = 0;
                    adcBase1 = adcBase1 * 2 / 3 + adcSum1 / 15 / 3; // update the base1
                    adcSum1 = 0;
                }
            }

            if ((PERSON_DETECTED == humDetectFlag || IGNORE_PERSON_DETECT == enHUM) &&
                (ALARM_NUMBER_0 == alarmCountAndFlag) && (MAGNETIC_MODULE_2 == channelFlag))
            {
                // 通道2开启检测，非报警状态
                HAL_GPIO_WritePin(LED1_YELLOW_GPIO_Port, LED1_YELLOW_Pin, GPIO_PIN_RESET); // GREEN
                HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_RESET);

                sensor_2_count++;

                // 通道2
                //      ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
                //      HAL_Delay(1);
                //      ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
                //      ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
                if (ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2) == HAL_OK)
                {
                    adcVol2 = KalmanFilter_2(adcVol2); // 卡尔曼滤波
                    // remove shake
                    adcSum2 = (adcBase2 - adcVol2) * (adcBase2 - adcVol2) + adcSum2;
                }
                else
                {
                    // error
                    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(LED2_YELLOW_GPIO_Port, LED2_YELLOW_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(LED2_RED_GPIO_Port, LED2_RED_Pin, GPIO_PIN_RESET);
                }

                if (sensor_2_count == SENSOR_COUNT_THRESHOLD_5)
                {
                    channelFlag = MAGNETIC_MODULE_1;
                }

                if (sensor_2_count == SENSOR_COUNT_THRESHOLD_5)
                { // 10
                    // count detect 60 time
                    sensor_2_count = SET_NUM_0;
                    humDetectFlag = PERSON_NOT_DETECTED;

                    // 通道2 Alarm and out
                    if (adcSum2 / 60 > adcDevial)
                    {
                        alarmCountAndFlag = ALARM_THRESHOLD_100;
                        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
                        // HAL_GPIO_WritePin(LED2_RED_GPIO_Port, LED2_RED_Pin, GPIO_PIN_RESET);
                        // HAL_GPIO_WritePin(LED2_GREEN_GPIO_Port, LED2_GREEN_Pin, GPIO_PIN_SET);
                        // HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(BUZZER2_GPIO_Port, BUZZER2_Pin, GPIO_PIN_RESET);

                        sprintf((char *)sBuf, "A Base2 = %f mV ADC2base=%f mV ADC2 = %f mV\r\n", adcBase2, adcSum2, adcSum2 / 60);
                        sLen = strlen((char *)sBuf);
                        HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                    }
                    adcSum2 = 0;
                }
            }

            if ((PERSON_NOT_DETECTED == humDetectFlag) && (ALARM_NUMBER_0 == alarmCountAndFlag) && (MAGNETIC_MODULE_2 == channelFlag))
            {
                // 通道2开启modification，非报警状态
                sensor_2_count++;
                if (ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2) == HAL_OK)
                {
                    adcVol2 = KalmanFilter(adcVol2); // 卡尔曼滤波+强化学习(cat)
                    // remove shake
                    adcSum2 = (adcBase2 - adcVol2) * (adcBase2 - adcVol2) + adcSum2;
                }
                if (sensor_2_count == 15) // 10 < 15 < 20
                {
                    channelFlag = MAGNETIC_MODULE_1;
                    sensor_2_count = SET_NUM_0;
                    adcBase2 = adcBase2 * 2 / 3 + adcSum2 / 15 / 3; // update the base2
                    adcSum2 = SET_NUM_0;
                }
            }

            // 处于报警状态
            if (alarmCountAndFlag > ALARM_NUMBER_0)
            {
                HAL_Delay(4); // 延长报警时间0.7s
                alarmCountAndFlag--;
                // 达到报警时长
                if (ALARM_NUMBER_0 == alarmCountAndFlag)
                {
                    // 清空sum
                    adcSum1 = SET_NUM_0;
                    adcSum2 = SET_NUM_0;
                    sensor_1_count = SET_NUM_0;
                    sensor_2_count = SET_NUM_0;
                    // close led buzzer
                    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(LED1_RED_GPIO_Port, LED1_RED_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(BUZZER1_GPIO_Port, BUZZER1_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(LED1_GREEN_GPIO_Port, LED1_GREEN_Pin, GPIO_PIN_RESET);

                    HAL_GPIO_WritePin(LED2_RED_GPIO_Port, LED2_RED_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(BUZZER2_GPIO_Port, BUZZER2_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(LED2_GREEN_GPIO_Port, LED2_GREEN_Pin, GPIO_PIN_RESET);
                    HAL_Delay(100);
                    // wait next detection
                    //        HAL_Delay(1000);
                    //        HAL_Delay(1000);
                }
            }
            /***************/
        }

        HAL_Delay(1);
        time100msCount++;

        if (time100msCount == 100)
        {
            time100msCount = 0;
            time1sCount++;
        }
        if (time1sCount == 10)
        {
            time1sCount = 0;
            if (enHUM == 3)
            {
                // 通道1
                ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                HAL_Delay(1);
                ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                if (ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1) == HAL_OK)
                {
                    sprintf((char *)sBuf, "T ADC1 = %f mV\r\n", adcVol1);
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                }
                // HAL_Delay(10);
                if (ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1) == HAL_OK)
                {
                    sprintf((char *)sBuf, "T ADC1 = %f mV\r\n", adcVol1);
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                }
                HAL_Delay(10);

                // 通道2
                ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
                HAL_Delay(1);
                ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
                ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2);
                if (ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2) == HAL_OK)
                {
                    sprintf((char *)sBuf, "T ADC2 = %f mV\r\n", adcVol2);
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                }
                // HAL_Delay(10);
                if (ADS1115_readSingleEnded(ADS1115_AIN2, &adcVol2) == HAL_OK)
                {
                    sprintf((char *)sBuf, "T ADC2 = %f mV\r\n", adcVol2);
                    sLen = strlen((char *)sBuf);
                    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                }
                HAL_Delay(10);
                // 通道1
                // ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                // HAL_Delay(1);
                // ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                // ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1);
                // if(ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1) == HAL_OK)
                //{
                //    sprintf((char*)sBuf,"T ADC1 = %f mV\r\n", adcVol1);
                //    sLen=strlen((char*)sBuf);
                //    HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                //}
                // HAL_Delay(10);
                // if(ADS1115_readSingleEnded(ADS1115_AIN0, &adcVol1) == HAL_OK)
                //{
                //    sprintf((char*)sBuf,"T ADC1 = %f mV\r\n", adcVol1);
                //   sLen=strlen((char*)sBuf);
                //   HAL_UART_Transmit(&huart1, sBuf, sLen, 300);
                //}
                // HAL_Delay(10);
                // HAL_Delay(100);
            }
        }

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
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

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /** Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{

    /* USER CODE BEGIN ADC1_Init 0 */

    /* USER CODE END ADC1_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC1_Init 1 */

    /* USER CODE END ADC1_Init 1 */

    /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
     */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = ENABLE;
    hadc1.Init.ContinuousConvMode = ENABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
     */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC1_Init 2 */

    /* USER CODE END ADC1_Init 2 */
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000;
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void)
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
    HAL_UART_Receive_IT(&huart1, (uint8_t *)aRxBuffer, 1); // 该函数会开启接收中断
    /* USER CODE END USART1_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /* USER CODE BEGIN MX_GPIO_Init_1 */
    /* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, BUZZER2_Pin | LED2_RED_Pin | BUZZER1_Pin | LED1_GREEN_Pin | LED1_YELLOW_Pin | LED1_RED_Pin | LED2_GREEN_Pin | LED2_YELLOW_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : LED_Pin */
    GPIO_InitStruct.Pin = LED_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : HUM_Pin */
    GPIO_InitStruct.Pin = HUM_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(HUM_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : CONF0_Pin CONF1_Pin CONF2_Pin CONF3_Pin */
    GPIO_InitStruct.Pin = CONF0_Pin | CONF1_Pin | CONF2_Pin | CONF3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : BUZZER2_Pin LED2_RED_Pin BUZZER1_Pin LED1_GREEN_Pin
                             LED1_YELLOW_Pin LED1_RED_Pin LED2_GREEN_Pin LED2_YELLOW_Pin */
    GPIO_InitStruct.Pin = BUZZER2_Pin | LED2_RED_Pin | BUZZER1_Pin | LED1_GREEN_Pin | LED1_YELLOW_Pin | LED1_RED_Pin | LED2_GREEN_Pin | LED2_YELLOW_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USER CODE BEGIN MX_GPIO_Init_2 */
    /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/**
 * @brief 串口1接收
 * @param None
 * @retval None
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) // 如果是串口1
    {
        if ((USART_RX_STA & 0x8000) == 0) // 接收未完成
        {
            if (USART_RX_STA & 0x4000) // 接收到了0x0d
            {
                if (aRxBuffer[0] != 0x0a)
                    USART_RX_STA = 0; // 接收错误,重新开始
                else
                    USART_RX_STA |= 0x8000; // 接收完成了
            }
            else // 还没收到0X0D
            {
                if (aRxBuffer[0] == 0x0d)
                    USART_RX_STA |= 0x4000;
                else
                {
                    USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer[0];
                    USART_RX_STA++;
                    if (USART_RX_STA > (49))
                        USART_RX_STA = 0; // 接收数据错误,重新开始接收
                }
            }
        }
    }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
