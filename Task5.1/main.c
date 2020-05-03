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
unsigned char but_state = False;
unsigned char btn_flag = False;
unsigned char btn_count = 0b00000;

void DispNumber6(long int n){
    PORTD = number[n % 10] | 0x80;
    PORTE = number[(n / 10) - (n / 100)*10];
    PORTF = number[(n / 100) - (n / 1000)*10];
    PORTC = number[(n / 1000) - (n / 10000)*10];
    PORTB = number[(n / 10000) - (n / 100000)*10];
    PORTA = number[(n / 1000000)];
}

ISR(TIMER0_OVF_vect)
{
    btn_flag = True;
}

int main(void)
{
    DDRA = 0xFF;
    DDRB = 0xFF;
    DDRC = 0xFF;
    DDRD = 0x7F;
    DDRE = 0xFF;
    DDRF = 0xFF;
    DispNumber6(i);
    
    TIMSK |= (1 << TOIE0);
    TCCR0 = 0x00;       //F_CPU / 1024
    sei();
    
    while (1) 
    {
        if (( but_state ^ ((~PIND & 0x80) >> 7)) && !(TCCR0)){
            TCNT0 = 255;
            TCCR0 = 0x07; 
        }
        
        if (btn_flag){
            if ( but_state ^ ((~PIND & 0x80) >> 7)){
                btn_count = (btn_count << 1) | 1;
            }
            else{
                btn_count = (btn_count << 1);
            }
            if (btn_count == 0b11111){
                but_state = !but_state;
                if ((!but_state) && (i != 0)){
                    i--;
                    DispNumber6(i);
                }
                TCCR0 = 0x00;                           //Timer0 OFF
                btn_count = 0b00000;
            }
            else if (btn_count == 0b00000){
                TCCR0 = 0x00;                           //Timer0 OFF
            }
            else{
                TCNT0 = 216;                //5ms
            }
            btn_flag = False;
        }
    }
}

