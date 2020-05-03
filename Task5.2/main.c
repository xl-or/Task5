#define F_CPU 8000000UL
#define True 1
#define False 0
#include <avr/io.h>
#include <avr/interrupt.h>

unsigned char number[10] = {
    0x3F,           //0
    0x06,           //1
    0x5B,           //2
    0x4F,           //3
    0x66,           //4
    0x6D,           //5
    0x7D,           //6
    0x07,           //7
    0x7F,           //8
    0x6F            //9
};
long int i = 50000;
long int temp;
unsigned char digit_on = 0;
unsigned char but_state = False;
unsigned char seg_light = False;
unsigned char btn_flag = False;
unsigned char btn_count = 0b00000;

ISR(TIMER0_OVF_vect)
{   
    seg_light = True; 
}

ISR(TIMER2_OVF_vect)
{
    btn_flag = True;
}

int main(void)
{
    DDRA = 0xFF;
    DDRD = 0xFF;
    DDRE = 0x00;
    PORTE = 0x01;
    PORTD = 0xFF; 
    temp = i;
    
    // Timer0 - dynamic indication
    TCCR0 = 0x04;       // 0x04 - F_CPU / 64
    TCNT0 = 255;
    TIMSK |= (1 << TOIE0);
    
    // Timer2 - button click
    TCCR2 |=0x00;       // 0x05 - F_CPU / 1024
    TIMSK |= (1 << TOIE2);
    sei();
    
    
    
    while (1) 
    {
        if (seg_light){
            PORTA = 0x00;
            PORTD = 0xFF & ~(1 << digit_on);
            PORTA = number[temp % 10];
            TCNT0 = 46;   
            temp /= 10;
            digit_on++;
            if (digit_on == 6){
                digit_on = 0;
                temp = i;
            }
            seg_light = False;
        }
        
        if (( but_state ^ (~PINE & 0x01)) && !(TCCR2)){
            TCNT2 = 255;
            TCCR2 = 0x05;                               //Timer2 ON
        }   
        
        if (btn_flag){
            if ( but_state ^ (~PINE & 0x01)){
                btn_count = (btn_count << 1) | 1;
            }
            else{
                btn_count = (btn_count << 1);
            }
            if (btn_count == 0b11111){
                but_state = !but_state;
                if ((!but_state) && (i != 0)){
                    i--;
                }
                TCCR2 = 0x00;                           //Timer2 OFF
                btn_count = 0b00000;
            }
            else if (btn_count == 0b00000){
                TCCR2 = 0x00;                           //Timer2 OFF
            }
            else{
                TCNT2 = 216;                //5ms
            }
            btn_flag = False;
        }  
        
    }      
}

