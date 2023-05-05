#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "usmart.h"
#include "adc.h"
#include "dac.h"
#include <math.h>
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_dac.h"

/************************************************
 ALIENTEK Mini STM32F103开发板实验15
 ADC模数转换实验-HAL库函数版
 技术支持：www.openedv.com
 淘宝店铺： http://eboard.taobao.com
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司
 作者：正点原子 @ALIENTEK
************************************************/

// 定义常量
#define PI 3.1415926

// 定义变量
u16 adcx;
float temp;
float angle = 0.0;
double voltage = 0;
float step = 0.1;
int waveType = 2;

void Draw_Axis(void)
{
    LCD_DrawLine(30, 180, 230, 180);
    LCD_DrawLine(30, 180, 30, 30);
    LCD_DrawLine(28, 34, 30, 30);
    LCD_DrawLine(32, 34, 30, 30);
    LCD_DrawLine(226, 178, 230, 180);
    LCD_DrawLine(226, 182, 230, 180);
    LCD_ShowString(30, 182, 200, 16, 16, "0V");
    LCD_ShowString(226, 182, 200, 16, 16, "3.3V");
    LCD_ShowString(110, 190, 200, 16, 16, "ADC Voltage (V)");
}
void Draw_Curve(float voltage)
{
    static u16 last_x = 30, last_y = 180;
    u16 x = 0, y = 0;

    x = (u16)(30 + (voltage / 3.3) * 200);        // 计算x坐标
    y = (u16)(180 - ((float)x - 30) / 200 * 150); // 计算y坐标

    // 限制坐标在屏幕内
    if (x < 30)
    {
        x = 30;
    }
    else if (x > 230)
    {
        x = 230;
    }
    if (y < 30)
    {
        y = 30;
    }
    else if (y > 180)
    {
        y = 180;
    }

    if (last_x != 0 && last_y != 0)
    {
        LCD_DrawLine(last_x, last_y, x, y);
    }

    last_x = x;
    last_y = y;
}
int main(void)
{
    DAC_HandleTypeDef hdac;
    DAC_ChannelConfTypeDef sConfig;
    HAL_Init();                     // 初始化HAL库
    Stm32_Clock_Init(RCC_PLL_MUL9); // 设置时钟,72M
    delay_init(72);                 // 初始化延时函数
    uart_init(115200);              // 初始化串口
    usmart_dev.init(84);            // 初始化USMART
    LED_Init();                     // 初始化LED
    LCD_Init();                     // 初始化LCD
    MY_ADC_Init();                  // 初始化ADC1通道1

    // Initialize DAC
    DAC1_Init();

    POINT_COLOR = RED;
    LCD_ShowString(30, 50, 200, 16, 16, "Mini STM32");
    LCD_ShowString(30, 70, 200, 16, 16, "ADC TEST");
    LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK");
    LCD_ShowString(30, 110, 200, 16, 16, "2019/11/15");
    POINT_COLOR = BLUE; // 设置字体为蓝色
    LCD_ShowString(30, 130, 200, 16, 16, "ADC1_CH1_VAL:");
    LCD_ShowString(30, 150, 200, 16, 16, "ADC1_CH1_VOL:0.000V"); // 先在固定位置显示小数点
    LCD_Clear(WHITE);

    while (1)
    {
        switch (waveType)
        {
        case 0: // 三角波
            voltage += step;
            if (voltage > 3.3)
            {
                voltage = 3.3;
                step = -step;
            }
            else if (voltage < 0)
            {
                voltage = 0;
                step = -step;
            }
            break;
        case 1: // 方波
            if (voltage < 1.65)
            {
                voltage = 3.3;
            }
            else
            {
                voltage = 0;
            }
            break;
        case 2:           // 正弦波
            angle += 0.1; // 每次增加0.1弧度
            if (angle >= 2 * PI)
            { // 超过360度，重新开始
                angle -= 2 * PI;
            }
            voltage = 1.65 + 1.65 * sin(angle); // 计算正弦波电压值
            break;
        }
        DAC1_Set_Vol(voltage);                                  // 设置DAC输出电压
        adcx = Get_Adc_Average(0, 20);                          // 修改ADC通道为0
        temp = (float)adcx * (3.3 / 4096);                      // 计算实际电压值
        LCD_ShowxNum(30 + 7 * 12, 130, adcx, 4, 16, 0);         // 在LCD上显示ADC采样值
        LCD_ShowxNum(30 + 11 * 12, 150, temp * 1000, 3, 16, 1); // 在LCD上显示实际电压值
        Draw_Axis();                                            // 绘制坐标轴
        Draw_Curve(temp);                                       // 绘制曲线
        delay_ms(10);
    }
}