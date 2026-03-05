#include "./BSP/ADC/adc.h"

/* 定义句柄 */
ADC_HandleTypeDef hadc1;

void ADC_Init(void)
{
    /* 定义初始化结构体 */
    ADC_ChannelConfTypeDef sConfig = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  
    /* 配置ADC时钟分频 */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

    /* 开启时钟 */
    __HAL_RCC_ADC1_CLK_ENABLE();
    
    /* 配置ADC参数 */
    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;           /* 禁用扫描模式，单通道 */
    hadc1.Init.ContinuousConvMode = ENABLE;              /*  连续模式，单次转换 */
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;     /* 触发源选择，软件触发 */
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;			  /* 数据对齐模式，右对齐 */
    hadc1.Init.DiscontinuousConvMode = DISABLE;			  /* 间断模式，禁用 */
    hadc1.Init.NbrOfConversion = 1;						  /* 间断模式每次转换的通道数 */
    HAL_ADC_Init(&hadc1);
    
    /* 配置ADC通道参数 */
    sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    /* 打开温度传感器 */
    ADC1->CR2 |= ADC_CR2_TSVREFE;

    /* 校准ADC */
    HAL_ADCEx_Calibration_Start(&hadc1);
    
    /* 启动ADC  */
    HAL_ADC_Start(&hadc1);
}

uint16_t ADC_GetValue(void)
{
    /* 单次非扫描，在init函数中启动了ADC，转换完成后会自动停止，结果存放在DR寄存器中 */
    // HAL_ADC_Start(&hadc1);

    /* 轮询等待ADC转换完成，获取ADC数据 */
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    
    // HAL_ADC_Stop(&hadc1);
    return value;
}