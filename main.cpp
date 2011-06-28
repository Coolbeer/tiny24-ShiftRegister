#include <avr/io.h>
#include <avr/interrupt.h>

/*
Inputs:
    PB0 = SCK
    PB1 = RCK
    PB2 = SER

Outputs:
    PA 0-7
*/

#define SPI_DDR DDRB
#define SPI_PIN PINB
#define SPI_SCK PB0
#define SPI_SER PB2
#define SPI_RCK PB1

void init(void)
{
    GIMSK |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT8) | (1 << PCINT9);
    DDRA = 0xFF;
    sei();      //Enable Interrupts
}

int main(void)
{
    init();
	ADCSRA &= (1 << ADEN);
	PRR |= (1 << PRTIM1) | (1 << PRTIM0) | (1 << PRUSI) | (1 << PRADC);
	MCUCR |= (1 << SE);
    while(1)
		asm volatile ( "SLEEP" );

    return 0;
}

ISR(PCINT1_vect)
{
    bool volatile static lastSCK;
    bool volatile static lastRCK;
    uint8_t volatile static data;

    if((SPI_PIN & (1 << SPI_SCK)) && !lastSCK)                     //SCK Clocked on
    {
        lastSCK = true;
        data = data >> 1;
        if(SPI_PIN & (1 << SPI_SER))
            data |= (1 << 7);
    }
    else if((SPI_PIN & (1 << SPI_RCK)) && !lastRCK)                //RCK Clocked on
    {
        lastRCK = true;
        PORTA = data;
    }
    else if(!(SPI_PIN & (1 << SPI_SCK)) && lastSCK)               //SCK Clocked off
    {
        lastSCK = false;
    }
    else if(!(SPI_PIN & (1 << SPI_RCK)) && lastRCK)               //SCK Clocked off
    {
        lastRCK = false;
    }
}
