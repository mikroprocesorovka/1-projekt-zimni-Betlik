#include "stm8s.h"
#include "milis.h"
#include "stm8s_i2c.h"
#include "delay.h"
#include "spse_stm8.h"
#include <stdio.h>

#include "../lib/uart.c"

#define _ISOC99_SOURCE
#define _GNU_SOURCE

#define LED_PORT GPIOC
#define LED_PIN  GPIO_PIN_5
#define LED_HIGH   GPIO_WriteHigh(LED_PORT, LED_PIN)
#define LED_LOW  GPIO_WriteLow(LED_PORT, LED_PIN)
#define LED_TOGG GPIO_WriteReverse(LED_PORT, LED_PIN)
/////////////////////////////////////////////////////////////////////////////// stepper motor 
#define A1_PORT GPIOF
#define A1_PIN  GPIO_PIN_7
#define A1_HIGH   GPIO_WriteHigh(A1_PORT, A1_PIN)
#define A1_LOW  GPIO_WriteLow(A1_PORT, A1_PIN)
#define A1_R GPIO_WriteReverse(A1_PORT, A1_PIN)

#define A2_PORT GPIOF
#define A2_PIN  GPIO_PIN_6
#define A2_HIGH   GPIO_WriteHigh(A2_PORT, A2_PIN)
#define A2_LOW  GPIO_WriteLow(A2_PORT, A2_PIN)
#define A2_R GPIO_WriteReverse(A2_PORT, A2_PIN)

#define A3_PORT GPIOF
#define A3_PIN  GPIO_PIN_5
#define A3_HIGH   GPIO_WriteHigh(A3_PORT, A3_PIN)
#define A3_LOW  GPIO_WriteLow(A3_PORT, A3_PIN)
#define A3_R GPIO_WriteReverse(A3_PORT, A3_PIN)

#define A4_PORT GPIOF
#define A4_PIN  GPIO_PIN_4
#define A4_HIGH   GPIO_WriteHigh(A4_PORT, A4_PIN)
#define A4_LOW  GPIO_WriteLow(A4_PORT, A4_PIN)
#define A4_R GPIO_WriteReverse(A4_PORT, A4_PIN)
////////////////////////////////////////////////////////////////////////////////
#define BTN_PORT GPIOE
#define BTN_PIN  GPIO_PIN_4
#define BTN_PUSH (GPIO_ReadInputPin(BTN_PORT, BTN_PIN)==RESET) 

void delay_ms(uint16_t ms) {
    uint16_t  i;
    for (i=0; i<ms; i = i+1){
        _delay_us(250);
        _delay_us(248);
        _delay_us(250);
        _delay_us(250);
    }
}

void ADC_init(void){
// na pinech/vstupech ADC_IN2 (PB2) a ADC_IN3 (PB3) vypneme vstupní buffer
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL2,DISABLE);
ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL3,DISABLE);
// nastavíme clock pro ADC (16MHz / 4 = 4MHz)
ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);
// volíme zarovnání výsledku (typicky vpravo, jen vyjmečně je výhodné vlevo)
ADC2_AlignConfig(ADC2_ALIGN_RIGHT);
// nasatvíme multiplexer na některý ze vstupních kanálů
ADC2_Select_Channel(ADC2_CHANNEL_2);
// rozběhneme AD převodník
ADC2_Cmd(ENABLE);
// počkáme než se AD převodník rozběhne (~7us)
ADC2_Startup_Wait();
}

void TIM2_setup(void){
    TIM2_DeInit();
    TIM2_TimeBaseInit(TIM2_PRESCALER_1, 640 - 1);//25kHz    
    TIM2_OC1Init(TIM2_OCMODE_PWM1, TIM2_OUTPUTSTATE_ENABLE, 200, TIM2_OCPOLARITY_LOW);
    TIM2_Cmd(ENABLE);

}

void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz

    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);

    GPIO_Init(A1_PORT, A1_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(A2_PORT, A2_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(A3_PORT, A3_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(A4_PORT, A4_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    
    init_milis();
    ADC_init();
    TIM2_setup();
 
}

int main(void)
{
    uint32_t time = 0;
    uint32_t cum = 1;
    //init();
    uint16_t adc_valuex;
    uint16_t adc_valuey;

    setup();
    init_uart();

    while (1) {

        //printf("2𓆏\r\n");
        adc_valuex = ADC_get(ADC2_CHANNEL_2); // do adc_value ulož výsledek převodu vstupu ADC_IN2 (PB2)
        adc_valuey = ADC_get(ADC2_CHANNEL_3);

        //printf("x: ");
        printf("x:""%d",adc_valuex );
        printf("  y:""%d",adc_valuey );
        //printf("\r\n");

        if (adc_valuex > 1000) {
            printf("  RIGHT ");  
                  
            //1 step
            A1_HIGH;             
            delay_ms(3);
            //2 step
            A1_LOW; 
            A2_HIGH;
            delay_ms(3); 
            //3 step
            A2_LOW;
            A3_HIGH;
            delay_ms(3);
            //4 step
            A3_LOW;
            A4_HIGH; 
            delay_ms(3);
            A4_LOW;

        }
        if (adc_valuex < 500) {
            printf("  LEFT "); 

            //1 step
            A4_HIGH;             
            delay_ms(3);
            //2 step
            A4_LOW; 
            A3_HIGH;
            delay_ms(3); 
            //3 step
            A3_LOW;
            A2_HIGH;
            delay_ms(3);
            //4 step
            A2_LOW;
            A1_HIGH; 
            delay_ms(3);
            A1_LOW;

        }
        if (adc_valuey < 500) {
            printf("  UP ");   


        }
        if (adc_valuey > 1000) {
            printf("  DOWN ");
              
     
        }
        printf("\r\n");





        /*
        if (milis() - time > 333 && BTN_PUSH) {
            LED_TOGG; 
            time = milis();
            
            printf("                         2𓆏\r\n");
            
        }

        //LED_TOGG; 
        //delay_ms(333);
        //printf("Funguje to!!!\n");
        */
    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
