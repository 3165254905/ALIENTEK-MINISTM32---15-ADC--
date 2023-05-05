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
 ALIENTEK Mini STM32F103������ʵ��15
 ADCģ��ת��ʵ��-HAL�⺯����
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

// ���峣��
#define PI 3.1415926

// �������
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

    x = (u16)(30 + (voltage / 3.3) * 200);        // ����x����
    y = (u16)(180 - ((float)x - 30) / 200 * 150); // ����y����

    // ������������Ļ��
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
    HAL_Init();                     // ��ʼ��HAL��
    Stm32_Clock_Init(RCC_PLL_MUL9); // ����ʱ��,72M
    delay_init(72);                 // ��ʼ����ʱ����
    uart_init(115200);              // ��ʼ������
    usmart_dev.init(84);            // ��ʼ��USMART
    LED_Init();                     // ��ʼ��LED
    LCD_Init();                     // ��ʼ��LCD
    MY_ADC_Init();                  // ��ʼ��ADC1ͨ��1

    // Initialize DAC
    DAC1_Init();

    POINT_COLOR = RED;
    LCD_ShowString(30, 50, 200, 16, 16, "Mini STM32");
    LCD_ShowString(30, 70, 200, 16, 16, "ADC TEST");
    LCD_ShowString(30, 90, 200, 16, 16, "ATOM@ALIENTEK");
    LCD_ShowString(30, 110, 200, 16, 16, "2019/11/15");
    POINT_COLOR = BLUE; // ��������Ϊ��ɫ
    LCD_ShowString(30, 130, 200, 16, 16, "ADC1_CH1_VAL:");
    LCD_ShowString(30, 150, 200, 16, 16, "ADC1_CH1_VOL:0.000V"); // ���ڹ̶�λ����ʾС����
    LCD_Clear(WHITE);

    while (1)
    {
        switch (waveType)
        {
        case 0: // ���ǲ�
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
        case 1: // ����
            if (voltage < 1.65)
            {
                voltage = 3.3;
            }
            else
            {
                voltage = 0;
            }
            break;
        case 2:           // ���Ҳ�
            angle += 0.1; // ÿ������0.1����
            if (angle >= 2 * PI)
            { // ����360�ȣ����¿�ʼ
                angle -= 2 * PI;
            }
            voltage = 1.65 + 1.65 * sin(angle); // �������Ҳ���ѹֵ
            break;
        }
        DAC1_Set_Vol(voltage);                                  // ����DAC�����ѹ
        adcx = Get_Adc_Average(0, 20);                          // �޸�ADCͨ��Ϊ0
        temp = (float)adcx * (3.3 / 4096);                      // ����ʵ�ʵ�ѹֵ
        LCD_ShowxNum(30 + 7 * 12, 130, adcx, 4, 16, 0);         // ��LCD����ʾADC����ֵ
        LCD_ShowxNum(30 + 11 * 12, 150, temp * 1000, 3, 16, 1); // ��LCD����ʾʵ�ʵ�ѹֵ
        Draw_Axis();                                            // ����������
        Draw_Curve(temp);                                       // ��������
        delay_ms(10);
    }
}