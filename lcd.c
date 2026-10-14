/* lcd.c */

#include <lpc214x.h>
#include "lcd.h"
#include "delay.h"

static void LCD_EnablePulse(void);
void LCD_Init(void)

{

    IODIR0 |= LCD_D;

    IODIR0 |= RS | EN;


    delay_ms(20);        // wait >15ms after Vcc stable


    LCD_Command(0x38);   // function set

    delay_ms(5);         // wait >4.1ms

    LCD_Command(0x38);   // function set again

    delay_us(150);       // wait >100us

    LCD_Command(0x38);   // function set again


    LCD_Command(0x38);   // now safe to configure normally

    LCD_Command(0x08);   // display off

    LCD_Command(0x01);   // clear display

    delay_ms(2);

    LCD_Command(0x06);   // entry mode

    LCD_Command(0x0C);   // display on

}
void LCD_Command(u8 cmd)
{
    IOCLR0 = LCD_D;
    IOCLR0 = RS;

    IOSET0 = ((u32)cmd << 4);

    LCD_EnablePulse();

    delay_ms(2);
}

void LCD_Data(u8 data)
{
    IOCLR0 = LCD_D;

    IOSET0 = RS;

    IOSET0 = ((u32)data << 4);

    LCD_EnablePulse();

    delay_ms(2);
}

void LCD_String(const char *str)
{
    while(*str)
    {
        LCD_Data(*str++);
    }
}

void LCD_Clear(void)
{
    LCD_Command(0x01);
    delay_ms(2);
}

void LCD_GotoXY(u8 row,u8 col)
{
    if(row == 0)
        LCD_Command(0x80 + col);
    else
        LCD_Command(0xC0 + col);
}

void LCD_Integer(s32 num)
{
    char buf[12];
    int i=0,j;
    char temp;

    if(num==0)
    {
        LCD_Data('0');
        return;
    }

    if(num<0)
    {
        LCD_Data('-');
        num=-num;
    }

    while(num)
    {
        buf[i++] = (num%10)+'0';
        num/=10;
    }

    buf[i]='\0';

    for(j=0;j<i/2;j++)
    {
        temp=buf[j];
        buf[j]=buf[i-j-1];
        buf[i-j-1]=temp;
    }

    LCD_String(buf);
}

static void LCD_EnablePulse(void)
{
    IOSET0 = EN;
    delay_us(50);

    IOCLR0 = EN;
    delay_us(50);
}
