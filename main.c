#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define SERVO_PIN    PB1 // Arduino pin 9 (OC1A)
#define TRIG_PIN     PB4 // Arduino pin 12
#define ECHO_PIN     PB3 // Arduino pin 11
#define BUZZER_PIN   PB0 // Arduino pin 8

void uart_init(unsigned int baud)
{
    unsigned int ubrr = F_CPU/16/baud-1;
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_send_char(char c)
{
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_send_string(const char* s)
{
    while (*s) uart_send_char(*s++);
}

void print_distance(uint8_t angle, uint16_t distance)
{
    char buf[20];
    int len = snprintf(buf, sizeof(buf), "%d,%d.\r\n", angle, distance);
    for (int i = 0; i < len; i++)
        uart_send_char(buf[i]);
}

uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void servo_init()
{
    DDRB |= (1 << SERVO_PIN);
    
    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
    

    ICR1 = 39999;
}

void servo_write(uint8_t angle)
{

    uint16_t pulse = 2000 + ((uint32_t)angle * 2000) / 180;
    OCR1A = pulse;
}

void sr04_init()
{
    DDRB |= (1 << TRIG_PIN);   // Trigger output
    DDRB &= ~(1 << ECHO_PIN);  // Echo input
}

uint16_t sr04_read_distance()
{
    PORTB &= ~(1 << TRIG_PIN);
    _delay_us(2);
    PORTB |= (1 << TRIG_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIG_PIN);

    uint16_t timeout = 0;
    while (!(PINB & (1 << ECHO_PIN)))
    {
        timeout++;
        if(timeout > 30000) return 0;
    }

    // Measure pulse width
    uint32_t count = 0;
    while (PINB & (1 << ECHO_PIN))
    {
        count++;
        if(count > 30000) break;
    }
    
    return (uint16_t)(count / 58);
}

void delay_ms_var(uint16_t ms)
{
    for(uint16_t i = 0; i < ms; i++)
    {
        _delay_ms(1);
    }
}

void beep(uint16_t duration_ms)
{
    PORTB |= (1 << BUZZER_PIN);
    delay_ms_var(duration_ms);
    PORTB &= ~(1 << BUZZER_PIN);
    _delay_ms(1);
}

int main(void)
{
    uart_init(9600);
    servo_init();
    sr04_init();
    DDRB |= (1 << BUZZER_PIN);
    
    uint16_t distance;
    
    servo_write(90);
    _delay_ms(1000);
    
    while(1)
    {
        for (uint8_t i = 0; i <= 180; i++)
        {
            servo_write(i);
            _delay_ms(30);
            distance = sr04_read_distance();
            print_distance(i, distance);
            
            if(distance > 0 && distance < 40)
            {
                uint16_t beepInterval = map(distance, 40, 0, 50, 1);
                beep(beepInterval);
            }
        }
        
        for (int16_t i = 180; i >= 0; i--)
        {
            servo_write(i);
            _delay_ms(30);
            distance = sr04_read_distance();
            print_distance(i, distance);
            
            if(distance > 0 && distance < 40)
            {
                uint16_t beepInterval = map(distance, 40, 0, 50, 1);
                beep(beepInterval);
            }
        }
    }
}