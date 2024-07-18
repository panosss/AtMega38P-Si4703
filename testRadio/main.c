#include <avr/io.h>
#include <util/delay.h>
#include "i2cmaster.h"

#define F_CPU 8000000UL // Ορίζουμε τη συχνότητα του μικροελεγκτή (π.χ. 16MHz)
#define SI4703_ADDR 0x10 // I2C διεύθυνση του SI4703

#define SI4703_RESET_PIN PB2 // Το pin 10 του Arduino Mini Pro (PB2)

void init_i2c(void) {
	i2c_init(); // Εκκίνηση της I2C διεπαφής
}

void si4703_write(uint8_t reg, uint16_t value) {
	int i;
	//i=i2c_start((SI4703_ADDR << 1) + I2C_WRITE);
	
	///////////////////////////////////////
	//ret = i2c_start((SI4703_ADDR << 1) + I2C_WRITE);       // set device address and write mode
	if (i2c_start((SI4703_ADDR << 1) + I2C_WRITE)) {
		/* failed to issue start condition, possibly no device found */
			uart_print(" failed to issue start condition, possibly no device found\n");			
		}else {
			uart_print("device found!!\n");
	}
	/////////////////////////////////////
	
	
	
	i2c_write(reg);
	i2c_write((value >> 8) & 0xFF);
	i2c_write(value & 0xFF);
	i2c_stop();
}

void si4703_init(void) {
	// Αρχικοποίηση του SI4703
	// Κάνουμε reset τον SI4703
	DDRB |= (1 << SI4703_RESET_PIN); // Ορίζουμε το pin ως έξοδο
	PORTB &= ~(1 << SI4703_RESET_PIN); // Τοποθετούμε το pin σε LOW
	_delay_ms(1); // Καθυστέρηση για 1ms
	PORTB |= (1 << SI4703_RESET_PIN); // Τοποθετούμε το pin σε HIGH
	_delay_ms(1); // Καθυστέρηση για 1ms

	
	////////////////////////////////
	// Attempt to write to the Si4703
	if (i2c_start(SI4703_ADDR | I2C_WRITE)) {
		uart_print("Failed to access Si4703.\n");
		} else {
		uart_print("Si4703 accessed.\n");

		// Example: Write 0x12 to register 0x02
		
		// Διαδικασία αρχικοποίησης
		//si4703_write(0x02, 0x4001); // Power up
		//_delay_ms(110); // Αναμονή για το power up
		
		/*if (i2c_write(0x02, 0x4001)) {
			uart_print("Failed to write register address.\n");
			} else if (i2c_write(0x12)) {
			uart_print("Failed to write data.\n");
			} else {
			uart_print("Data written to Si4703.\n");
		}
		*/
		
	}
	////////////////////////////////
	
	// Διαδικασία αρχικοποίησης	
	si4703_write(0x02, 0x4001); // Power up
	_delay_ms(110); // Αναμονή για το power up
	uart_print("Power up\n");

	si4703_write(0x07, 0x8100); // Enable the oscillator
	_delay_ms(500); // Αναμονή για τον ταλαντωτή
	uart_print("Enable the oscillator\n");
}

void si4703_set_frequency(uint16_t frequency) {
	uint16_t channel = (frequency - 875) * 10; // Υπολογισμός του καναλιού
	si4703_write(0x03, 0x8000 | channel); // Ρύθμιση του καναλιού
}



// Function to initialize the UART for serial communication
void uart_init(unsigned int ubrr) {
	// Set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	// Set frame format: 8 data bits, 1 stop bit
	UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
}

// Function to transmit a single character via UART
void uart_transmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1<<UDRE0)));
	// Put data into buffer, sends the data
	UDR0 = data;
}

// Function to print a string via UART
void uart_print(const char* str) {
	while (*str) {
		uart_transmit(*str++);
	}
}

int main(void) {
	
	// Initialize UART for debugging
	uart_init(51); // 9600 baud rate with 8 MHz clock
	uart_print("Starting...\n");
	
	
	init_i2c();
	
	// Αρχικοποίηση της I2C διεπαφής
	i2c_init();
	uart_print("I2C initialized.\n");

	// Αρχικοποίηση του SI4703
	si4703_init();

	// Ρύθμιση της συχνότητας στα 101.1 MHz
	si4703_set_frequency(9010);

	while (1) {
		// Κύκλος αναμονής
		_delay_ms(1000);
	}

	return 0;
}
