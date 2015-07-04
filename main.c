/*
 * main.c
 */
//******************************************************************************
#include "msp430g2553.h"

#define     LED1                  BIT0
#define     LED2                  BIT6
#define     LED_DIR               P1DIR
#define     LED_OUT               P1OUT

/*long VolMeasured[8];
unsigned char VolMeasuredPosition=0;
long VolAverage;
*/

void InitializeLeds(void);             //初始化LED，使LED2和LED1都灭
void InitializeClocks(void);           //初始化时钟
void ConfigureAdc(void);               //配置ADC

void main(void)
{
  InitializeLeds();
  InitializeClocks();
  ConfigureAdc();

  while(1)
  {
	  ADC10CTL0 |= ENC + ADC10SC;               //开始采样和转换
	  __bis_SR_register(CPUOFF + GIE);          //进入低功耗模式
	 /* VolMeasured[VolMeasuredPosition++] = ADC10MEM;
	     if (VolMeasuredPosition == 8)
	       VolMeasuredPosition = 0;
	     VolAverage = 0;
	     for (i = 0; i < 8; i++)
	       VolAverage += VolMeasured[i];
	     VolAverage >>= 3;
      */
	     if(ADC10MEM>0X2ff)                    //若电压大于2.67V，LED2和LED1全亮
	     {
	    	  LED_OUT |= (LED1 + LED2);
	     }
	     else if(ADC10MEM>0X0ff)              //若电压小于2.67V且大于0.889V，LED2亮LED1灭
	     {
	    	  LED_OUT &= ~LED1 ;
	    	  LED_OUT |=  LED2 ;
	     }
	     else                                 //若电压小于0.889V，LED2灭LED1亮
		 {
	    	  LED_OUT &= ~LED2 ;
	    	  LED_OUT |=  LED1 ;
		 }
  }
}

void InitializeLeds(void)
{
  LED_DIR |= LED1 + LED2;
  LED_OUT &= ~(LED1 + LED2);
}

void InitializeClocks(void)
{
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL = CALDCO_1MHZ;
  BCSCTL2 &= ~(DIVS_3);
}

void ConfigureAdc(void)
{
 /* P1SEL|=BIT4;
  P1DIR&=~BIT4; */
  ADC10CTL1 = INCH_4 + ADC10DIV_3;           // 选择P1.4做采样输入
  ADC10CTL0 = ADC10SHT_3 + ADC10ON + ADC10IE;//设置采样保持时间、打开ADC10.中断使能
 // __delay_cycles(1000);                     // Wait for ADC Ref to settle
/*  ADC10CTL0 |= ENC + ADC10SC;               // Sampling and conversion start
  __bis_SR_register(CPUOFF + GIE);          // LPM0 with interrupts enabled
  tempCalibrated = ADC10MEM;
  for (i=0; i < 8; i++)
    tempMeasured[i] = tempCalibrated;
  tempAverage = tempCalibrated;
*/
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
  __bic_SR_register_on_exit(CPUOFF);        // 退出低功耗
}
