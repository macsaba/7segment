#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

//7 segment LED display anodes

#define B PD0
#define A PD1
#define F PD2
#define G PD3
#define E PD4
#define DP PD5
#define C PD6
#define D PD7

#define DIG_1 (1 << B)|(1 << C)
#define DIG_2 (1 << A)|(1 << B)|(1 << G)|(1 << E)|(1 << D)
#define DIG_3 (1 << A)|(1 << B)|(1 << G)|(1 << C)|(1 << D)
#define DIG_4 (1 << F)|(1 << G)|(1 << B)|(1 << C)
#define DIG_5 (1 << A)|(1 << F)|(1 << G)|(1 << C)|(1 << D)
#define DIG_6 (1 << A)|(1 << D)|(1 << C)|(1 << F)|(1 << E)|(1 << G)
#define DIG_7 (1 << A)|(1 << B)|(1 << C)
#define DIG_8 (1 << A)|(1 << B)|(1 << C)|(1 << D)|(1 << E)|(1 << F)|(1 << G)
#define DIG_9 (1 << A)|(1 << B)|(1 << C)|(1 << D)|(1 << F)|(1 << G)
#define DIG_0 (1 << A)|(1 << B)|(1 << C)|(1 << D)|(1 << E)|(1 << F)
#define DIG_P (1 << DP)

//7 segment LED display cathod digits

#define VOLT_10  PC2
#define VOLT_1   PC3
#define VOLT_01  PC4
#define VOLT_001 PC5

#define AMP_10  PB0
#define AMP_1   PB7
#define AMP_01  PB6
#define AMP_001 PB1

#define VOLT_PORT PORTC
#define AMP_PORT PORTB

//SPI

#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define CS_AMP   PB2 // Chip Select pin current (shunt resistor's voltage)
#define CS_VOLT  PC0 // Chip Select pin voltage
 
 //multiplex delay [ms]
#define del 1

int digits[10] = {DIG_0, DIG_1, DIG_2, DIG_3, DIG_4, DIG_5, DIG_6, DIG_7, DIG_8, DIG_9};



void m_delay_10ms(unsigned char val) {
    unsigned char i;
    for(i=0;i<val;i++) {
        _delay_ms(10);
    }
}

//write to voltage display
void print_volt(int v)
{
	int d_10 = v/1000;
    int d_1  = (v - d_10*1000)/100;
    int d_01   = (v - d_10*1000-d_1*100)/10;
    int d_001  = (v - d_10*1000-d_1*100 -d_01*10);
	
	PORTD = digits[d_10];
	VOLT_PORT |= (1 << VOLT_10);
	_delay_ms(del);
	VOLT_PORT &= ~(1 << VOLT_10);
	PORTD = digits[d_1];
	VOLT_PORT |= (1 << VOLT_1);
	_delay_ms(del);
	VOLT_PORT &= ~(1 << VOLT_1);
	PORTD = digits[d_01];
	VOLT_PORT |= (1 <<VOLT_01);
	_delay_ms(del);
	VOLT_PORT &= ~(1 << VOLT_01);
	PORTD = digits[d_001];
	VOLT_PORT |= (1 << VOLT_001);
	_delay_ms(del);
	VOLT_PORT &= ~(1 <<VOLT_001);
}

//write to current display
void print_amp(int a)
{
	int d_10 = a/1000;
    int d_1  = (a - d_10*1000)/100;
    int d_01   = (a - d_10*1000-d_1*100)/10;
    int d_001  = (a - d_10*1000-d_1*100 -d_01*10);
	
	PORTD = digits[d_10];
	AMP_PORT |= (1 << AMP_10);
	_delay_ms(del);
	AMP_PORT &= ~(1 << AMP_10);
	PORTD = digits[d_1];
	AMP_PORT |= (1 << AMP_1);
	_delay_ms(del);
	AMP_PORT &= ~(1 << AMP_1);
	PORTD = digits[d_01];
	AMP_PORT |= (1 << AMP_01);
	_delay_ms(del);
	AMP_PORT &= ~(1 << AMP_01);
	PORTD = digits[d_001];
	AMP_PORT |= (1 << AMP_001);
	_delay_ms(del);
	AMP_PORT &= ~(1 << AMP_001);
}

//initialize SPI
void init_SPI(){
	
  // turn CS pins high
  AMP_PORT |= (1<<CS_AMP);
  VOLT_PORT |= (1<<CS_VOLT);
  // SPI enable, Master mod, SPI clock
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0)|(1<<SPR1);
  //SPSR = (1<<SPI2X);
}
void try (int CS){
	m_delay_10ms(100);
	SPI_PORT &= ~(1<<CS);
	_delay_us(1);
	// Adattranszfer elinditasa (MOSI)
	SPDR = 0b10101010;
	// Varakozas amig az adattranszfer befejezodik
	while(!(SPSR & (1<<SPIF)));
	// Adattranszfer elinditasa (MOSI)
	SPDR = 0b10101010;
	// Varakozas amig az adattranszfer befejezodik
	while(!(SPSR & (1<<SPIF)));	
	// Adattranszfer elinditasa (MOSI)
	SPDR = 0b10101010;
	// Varakozas amig az adattranszfer befejezodik
	while(!(SPSR & (1<<SPIF)));
	// Adattranszfer elinditasa (MOSI)
	SPDR = 0b10101010;
	// Varakozas amig az adattranszfer befejezodik
	while(!(SPSR & (1<<SPIF)));
	m_delay_10ms(100);
	SPI_PORT |= (1<<CS);


}
unsigned char AD_write_read(int CS)
{
  unsigned char adatbe_h;
  unsigned char adatbe_l;
 
  // MCP3202 SPI bekapcsolasa (CS logikai alacsony szint)
  SPI_PORT &= ~(1<<CS);
  _delay_us(1);
  // Adattranszfer elinditasa (MOSI)
  SPDR = 0b10101010;
  // Varakozas amig az adattranszfer befejezodik
  while(!(SPSR & (1<<SPIF)));
 // Adattranszfer elinditasa (MOSI)
  SPDR = 0b10101010;
  // Varakozas amig az adattranszfer befejezodik
  while(!(SPSR & (1<<SPIF)));
  // Beerkezett adat;
  adatbe_h = SPDR;
 // Adattranszfer elinditasa (MOSI)
  SPDR = 0b10101010;
  // Varakozas amig az adattranszfer befejezodik
  while(!(SPSR & (1<<SPIF)));
  // Beerkezett adat;
  adatbe_l = SPDR;
  // MCP3202 SPI kikapcsolasa (CS logikai magas szint)
  m_delay_10ms(100);
  SPI_PORT |= (1<<CS);
  // A beerkezett adatokbol az ADC eredmeny eloallitasa
  // A 12 bites eredmeny atkonvertalasa 8 bit-esre(osztas 16-al)
  return 1;
}

int main(void) {
	m_delay_10ms(100);
	
	DDRC   = (1 << VOLT_10)|(1 << VOLT_1)|(1 << VOLT_01)|(1 << VOLT_001)|(1<<CS_VOLT);					// VOLT DIGITS
	DDRB   = (1 << AMP_10)|(1 << AMP_1)|(1 << AMP_01)|(1 << AMP_001)|(1<<PB3)|(1<<PB5)|(1<<CS_AMP);		// AMP DIGITS
	DDRD = 255; 					// digits
	
	init_SPI();
	//AMP_PORT &= ~(1<<CS_AMP);
	//AD_write_read(CS_AMP);
	try(CS_AMP);

    while(1)
    {       
		print_volt(4392);
		print_amp(1880);
		_delay_ms(5);
    }
    return 0;
}

