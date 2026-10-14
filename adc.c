/* adc.c */
#include <lpc214x.h>
#include "adc.h"
#include "delay.h"

void ADC_Init(void)
{
    // Configure P0.30 as A/D
    PINSEL1 &= ~(3 << 28);
    PINSEL1 |=  (1 << 28);

   
    PCONP |= (1 << 12);

    /* Clock divider & Power-up AD0 module */
    AD0CR = (1 << 3)     |  /* Select AD0.3 (Channel 3) */
            (4 << 8)     |  /* Clock division: CLKDIV = 4 */
            (1 << 21);      /* PDN = 1 (Operational mode) */
}

u16 ADC_Read(u8 channel)
{
    u32 result;

    AD0CR &= ~(0xFF);           // Clear previous channel
    AD0CR |= (1 << channel);    // Select ADC channel
    AD0CR |= (1 << 24);         // Start conversion

    while(!(AD0GDR & (1UL << 31)));    // Wait until conversion complete (DONE bit set)

    result = (AD0GDR >> 6) & 0x3FF; // Read 10-bit ADC result

    AD0CR &= ~(7 << 24);        // Stop conversion

    return (u16)result;
}

float GetTemperature(void)
{
    static float filtered = 0.0f;
    static u8    first    = 1;

    unsigned long sum = 0;
    u16 sample, minv = 1023, maxv = 0;
    u8  i;
    float adc, temp;

    /* 34 samples: drop the highest and lowest, average the other 32 */
    for(i = 0; i < 34; i++)
    {
        sample = ADC_Read(3);

        if(sample < minv) minv = sample;
        if(sample > maxv) maxv = sample;

        sum += sample;
        delay_us(100);              /* let the input settle between samples */
    }

    sum -= (unsigned long)minv + maxv;

    adc  = sum / 32.0f;
    temp = (adc * 3.3f * 100.0f) / 1023.0f;   /* LM35: 10 mV per degree C */

    /* Smooth the result so the LCD value does not jump around */
    if(first)
    {
        filtered = temp;
        first    = 0;
    }
    else
    {
        filtered += 0.1f * (temp - filtered);
    }

    return filtered;
}
