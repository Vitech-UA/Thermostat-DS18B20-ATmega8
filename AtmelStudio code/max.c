#include <mega8.h>
#include <delay.h>
#include <delay.h>
#include <1wire.h>
#include <ds18b20.h> // ��� DS18B20 ����'������ �������������� ���� �� 8 ���!!!

#define _soft_data  PORTC.1
#define _soft_shift PORTC.0
#define _soft_latch PORTC.2

#define MAX7219_MODE_DECODE       0x09
#define MAX7219_MODE_INTENSITY    0x0A
#define MAX7219_MODE_SCAN_LIMIT   0x0B
#define MAX7219_MODE_POWER        0x0C
#define MAX7219_MODE_TEST         0x0F
#define MAX7219_MODE_NOOP         0x00

#define Button_plus PINB.1
#define Button_minus PINB.2
#define RELE PORTD.0

int gisteresis = 1; //  ��������� ����������.
eeprom int dtemper = 25;
int temper = 0;


void write(char data, char adress)
{    
       unsigned char i;
       //�������� CS
       _soft_latch=0;  
       #asm("nop") //latch
 
       //��������� �����
       for(i=0;i<8;i++)
       {        
       if(adress & 0x80)
       {
       _soft_data = 1;
       }
       else
       {
       _soft_data = 0;
       }
         #asm("nop") //shift
         _soft_shift=1;
         #asm("nop")
         _soft_shift=0;  
         adress <<= 1;   
       }  
 
       //��������� ������     
       for(i=0;i<8;i++)
       {        
       if(data & 0x80)
       {
       _soft_data = 1;
       }
       else
       {
       _soft_data = 0;
       }
         #asm("nop") //shift
         _soft_shift=1;
         #asm("nop")
         _soft_shift=0;  
         data <<= 1;   
      }               
       _soft_latch=1;   
}


 void print_val(int a)
 {
    char R_1 = 0;
    char R_2 = 0;
    char R_3 = 0;
    
    R_1 = a / 100;
    R_2 = (a - R_1 * 100) / 10;
    R_3 = (a - R_1 * 100) % 10; 
  
    write(R_1, 0x01);
    write(R_2, 0x02);
    write(R_3, 0x03);
    
 }
void main(void)
{
 
 DDRC = 0xFF; 
 DDRB=(0<<DDB2) | (0<<DDB1) | (0<<DDB0); // ����� ������� ����������� DS18B20 � ������.
 PORTB |= (1<<1) | (1<<2); // ������� ������� ��� �� ������ �� +5�.
 DDRD = 0xFF;              // ����� �� ����
 
 w1_init();
 ds18b20_init(0,0,120,0);
 // ��������� ������
	 write(1, MAX7219_MODE_POWER);          // �������� �������� MAX7219
	 write(0xFF, MAX7219_MODE_DECODE);      // ��� �����������
	 write(0x02, MAX7219_MODE_SCAN_LIMIT);  // �-�� ������� 3
	 write(0x03, MAX7219_MODE_INTENSITY);   //��������� LED-���������� 
	 write(0x00, MAX7219_MODE_TEST);
 //
 
  
  
 


while (1)
    {
     temper = ds18b20_temperature(0);
     print_val(temper);
     
     if(Button_plus == 0)
     {
      dtemper++;
      print_val(dtemper);
      delay_ms(1000);
     }
     if(Button_minus == 0)
     {
      dtemper--;
      print_val(dtemper);
      delay_ms(1000);
     }
     
      if(temper >= dtemper)
      {
       RELE = 0;}
      else if(temper <= dtemper - gisteresis)
      {
       RELE = 1;}
     
     delay_ms(10);
    }
}
