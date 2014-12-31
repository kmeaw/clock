#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/iotn2313.h>

/*
 * A - PD0,
 * B - PD1,
 * C - PD4,
 * D - PD5;
 *
 * SH_CP - PD3, (clock)
 * ST_CP - PD2, (latch)
 * !OE - PA0,
 * DS - PA1.    (data)
 */

const uint8_t digits[] = { 0xaf, 0xa0, 0xcd, 0xe9, 0xe2, 0x6b, 0x6f, 0xa1, 0xef, 0xeb };

volatile char ticked = 0;

ISR(TIMER1_COMPA_vect) {
  ticked = 1;
}

void hc(uint8_t d)
{
  char i;
  PORTD = PORTD & ~(1 << 2); /* latch low */
  PORTD = PORTD & ~(1 << 3); /* clock low */
  for(i=0; i < 8; i++)
  {
    if ((d & 1) == 0)
      PORTA = PORTA | (1 << 1); /* data high */
    else
      PORTA = PORTA & ~(1 << 1); /* data low */
    d = d >> 1;
    PORTD = PORTD | (1 << 3); /* clock high */
    PORTD = PORTD & ~(1 << 3); /* clock low */
  }
  PORTD = PORTD | (1 << 2); /* latch high */
}

int main()
{
  DDRD = 0x3f;
  PORTD = 0x11;
  DDRA = 0x03;
  DDRB = 0;
  PORTB = 3;
 
  ACSR |= 1<<ACD;

  OCR1AH = 0xF4; 
  OCR1AL = 0x24;
  TCCR1A = 0x80;
  TCCR1B |= 0x0A; 
  TIMSK |= 1 << OCIE1A; 

  uint8_t x = 0;
  uint8_t h = 0, m = 4;
  uint8_t z = 0, s = 0;
  uint8_t b = 0, q = 0;
  sei();

  while(1)
  {
    b = 3;
    for(x=0; x<4; x++)
    {
      if (x == 0)
      {
	PORTD = (PORTD & ~0x33) | 0x01;
	hc(digits[m % 10]);
	b &= PINB & 3;
      }
      else if (x == 1)
      {
	PORTD = (PORTD & ~0x33) | 0x02;
	hc(digits[m / 10]);
      }
      else if (x == 2)
      {
	PORTD = (PORTD & ~0x33) | 0x10;
	hc(digits[h % 10] | (1 << 4));
      }
      else if (x == 3)
      {
	PORTD = (PORTD & ~0x33) | 0x20;
	hc(digits[h / 10]);
      }
      PORTA = 0; /* enable on */
      _delay_ms(5);
      PORTA = 1; /* enable off */
    }
    if (b != 3)
    {
      if (!q)
      {
	if ((b & 1) == 0)
	  h = (h + 1) % 24;
	if ((b & 2) == 0)
	  m = (m + 1) % 60;
      }
      q = (q + 1) % 8;
    }
    else
      q = 0;
    if (ticked)
    {
      ticked = 0;
      z = z + 1;
      if (z > 1)
      {
	z = 0;
	s = s + 1;
      }
      if (s == 60)
      {
	s = 0;
	m = m + 1;
      }
      if (m == 60)
      {
	m = 0;
	h = h + 1;
      }
      if (h == 24)
      {
	h = 0;
      }
    }
  }
  return 0;
}
