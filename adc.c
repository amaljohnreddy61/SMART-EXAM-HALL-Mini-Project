/* adc.c */
#include <lpc214x.h>
#include "adc.h"

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
    unsigned long sum=0;
	int i;
	float voltage;
	float temp;
	float adc;
	for(i=0;i<32;i++)
	{
	sum+=ADC_Read(3);
	}
	adc=sum/32.0f;
	voltage =(adc*3.3f)/1023.0f;
	temp=voltage*100.0f;
	return temp;
}
