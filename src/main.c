#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DURATION_MAX    8

uint8_t _buttonStatus;
uint8_t _buttonStatusLastDown;
uint8_t _lastButtonStatus;
uint8_t _lastButtonReleased;

volatile uint8_t duration = 1;
volatile uint8_t trigger;


uint8_t numbers[10] = { 
    0b01011111,
    0b00001100,
    0b00111011,
    0b00111110,
    0b01101100,
    0b01110110,
    0b01110111,
    0b00011100,
    0b01111111,
    0b01111110
    };

// ------------------------------------- Prototypes --------------------
void wait(uint8_t count);
void init_ports(void);
void sevenseg(uint8_t num);
uint8_t queryButtons(void);
void init_int_timer(void);

// ------------------------------------- Implementation ----------------

uint8_t queryButtons(void) {
  _buttonStatus = ~(PIND & 0x0B);
  		
  if (_buttonStatus != _lastButtonStatus) {
    _lastButtonReleased = (_lastButtonStatus ^ _buttonStatus) & ~_buttonStatus;
    _lastButtonStatus = _buttonStatus;
  }
  	
  uint8_t buttonChange = _buttonStatusLastDown ^ _buttonStatus;
  _buttonStatusLastDown = _buttonStatus;
	
  return buttonChange & _buttonStatus;
}


void wait(uint8_t count) {
    uint8_t i;
    if(count == 0) count = 100;
    for(i=0;i<count;i++) {
        _delay_ms(1);
    }
}

void init_ports() {
    // buttons as input
    // button UP is PD0
    // button DOWN is PD1
    // ZCD is PD2
    DDRD &= ~(_BV(PD0) | _BV(PD1) | _BV(PD2));
    // enable pullups
    PORTD |= _BV(PD0) | _BV(PD1) | _BV(PD2);

    // 7-SEG and MOC as Output
    DDRB |= 0x1F;       // b00011111
    DDRD |= 0x70;       // bX1110000

    // switch MOC OFF
    PORTD &= ~_BV(PD4);
}

void sevenseg(uint8_t num) {    
    uint8_t value = numbers[num % 10];
    PORTB = value & 0x1F;                           // ignore PB5-7
    uint8_t moc = PORTD & _BV(PD4);                 // save moc
    
    PORTD = moc | 0x0F | (value & 0x60);    
                    // use 0x0F to keep PD0,1,2,3 pullups on    
}



int main(void) {
    
    init_ports();
    init_int_timer();
    
    uint8_t buttons = queryButtons(); // trigger to skip bad results

    while(1) {

        buttons = queryButtons();

        if(buttons & _BV(PD0) && duration > 1) {
            duration--;
        } else if(buttons & _BV(PD1) && duration < DURATION_MAX) {
            duration++;
        }

        if(buttons & _BV(PD3)) {
            // Trigger
            trigger = 1;
        }

        sevenseg(duration);

        wait(100);   // 100ms
    }
        
	return(0);
} 

void init_int_timer(void) {
    // configure INT0 on any logical change
    MCUCR |= _BV(ISC00);
    
    // enable INT0 Interrupt
    GIMSK |= _BV(INT0);
    
    sei();
}

// ISR on INT0 for Zero Crossing Detection
ISR (INT0_vect) {      
    if(trigger) {
        trigger = 0;
        
        PORTD &= ~_BV(PD4);
        _delay_us(500);
        
        static uint8_t i;
        for(i=0;i<DURATION_MAX-duration;i++) {
            _delay_ms(1);
        }
    
        PORTD |= _BV(PD4);
    
        _delay_us(200);
        PORTD &= ~_BV(PD4);
        
    }
   
}




