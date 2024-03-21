#include <avr/io.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <stdint.h>


// Define constants
#define F_CPU 16000000UL
#define BAUD 2400
#define MY_UBRR F_CPU/16/BAUD-1
#define EEPROM_SIZE 1020

// Initializing UART communication
void uart_init(uint16_t ubrr) {
    // Setting the high byte of the UART baud rate register
    UBRR0H = (unsigned char)(ubrr >> 8);

    // Setting the low byte of the UART baud rate register
    UBRR0L = (unsigned char)ubrr;

    // Enabling receiver (RXEN0) and transmitter (TXEN0)
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);

    // Setting the character size to 8 bits (UCSZ00 = 3)
    UCSR0C = (3 << UCSZ00);
}


// Transmitting a character over UART
void uart_transmit(char data) {
    // Waiting until the transmit buffer (UDRE0) is ready for new data
    while (!(UCSR0A & (1 << UDRE0)));

    // Sending the character data to the UART data register (UDR0)
    UDR0 = data;
}


// Receiving a character over UART
char uart_receive(void) {
    // Waiting until the receive complete (RXC0) flag is set
    while (!(UCSR0A & (1 << RXC0)));

    // Returning the received character from the UART data register (UDR0)
    return UDR0;
}


// Storing a string in EEPROM
void store_string_in_eeprom(char* str) {
    // Initializing EEPROM address to 0
    int address = 0;

    // Iterating through the string until the null terminator '*'(delimiter)
    while (*str != '*') {
        // Writing the current character to EEPROM at the specified address
        eeprom_write_byte((uint8_t*)address, *str);

        // Incrementing both the address and moving to the next character in the string
        address++;
        str++;
    }

    // Writing the null terminator '*' to EEPROM to mark the end of the string
    eeprom_write_byte((uint8_t*)address, '*');
}


// Retrieving a string from EEPROM
void retrieve_string_from_eeprom(char* str) {
    // Initializing EEPROM address to 0
    int address = 0;

    // Reading the current character from EEPROM at the specified address
    char current_char = eeprom_read_byte((uint8_t*)address);

    // Iterating through EEPROM until the null terminator '*' is encountered
    while (current_char != '*') {
        // Copying the current character to the destination string
        *str = current_char;

        // Incrementing both the address and moving to the next character in the destination string
        address++;
        str++;

        // Reading the next character from EEPROM at the updated address
        current_char = eeprom_read_byte((uint8_t*)address);
    }

    // Adding the null terminator '*' to mark the end of the retrieved string
    *str = '*';
}


int main(void) {
    // Arrays to store received and stored data
    char received_data[EEPROM_SIZE];
    char stored_data[EEPROM_SIZE];

    // Initializing UART communication
    uart_init(MY_UBRR);

    // Counter variable for indexing received_data array
    int i = 0;

    // Infinite loop for continuous operation
    while (1) {
        // Checking if there is data available in the UART receive buffer
        if (UCSR0A & (1 << RXC0)) {
            // Read the received character from UART
            char received_char = uart_receive();

            // Checking if the received character is not the null terminator '*'
            if (received_char != '*') {
                // Storing the received character in the received_data array
                received_data[i] = received_char;
                i++;
            } else if (received_char == '*') {
                // If the received character is '*', mark the end of the received_data array
                received_data[i] = '*';

                // Storing the received string in EEPROM
                store_string_in_eeprom(received_data);

                // Retrieving the stored string from EEPROM
                retrieve_string_from_eeprom(stored_data);

                // Reset the index counter
                i = 0;

                // Transmitting the stored string over UART
                char* str_ptr = stored_data;
                while (*str_ptr != '*') {
                    uart_transmit(*str_ptr);
                    str_ptr++;
                }

                // Resetting the arrays for new data reception
                received_data[0] = '\0';
                stored_data[0] = '\0';
            }
        }
    }

    return 0;
}

