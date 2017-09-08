
#include "uart.h"

static unsigned char
fifo[8];

static unsigned char
fifo_rdptr = 0;

static unsigned char
fifo_wrptr = 0;

static unsigned char
sx_queue[8];
static unsigned char
sx_enqptr = 0, sx_deqptr = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  // put your setup code here, to run once:
  uart_init(19200);
  uart_putstr("hello.\n");

}

void loop() {
  int ch;
  // put your main code here, to run repeatedly:
  if ( (ch = uart_getchar()) != -1 ) {
    uart_putnum_u16(fifo_wrptr, 3);
    
    uart_putstr("> ");
    do {
      uart_putchar((char)ch);
    } while ( (ch = uart_getchar()) != -1 );
    uart_putstr("\n");
  }
}


static void uart_push(unsigned char c) {
  if (fifo_rdptr != (fifo_wrptr + 1)) {
    fifo[fifo_wrptr++] = c;
    fifo_wrptr &= 7;
  }
}


void uart_init(unsigned long baud) {
  //duint32_t baud = 19200;
    cli();
    // Macro to determine the baud prescale rate see table 22.1 in the Mega datasheet

    UBRR0 = (((F_CPU / (baud * 16UL))) - 1);         // Set the baud rate prescale rate register
    UCSR0B = ((1<<RXEN0)|(1<<TXEN0));  // Enable receiver and transmitter and Rx interrupt
    UCSR0C = ((0<<USBS0)|(1 << UCSZ01)|(1<<UCSZ00));  // Set frame format: 8data, 1 stop bit. See Table 22-7 for details

    uart_enable_rxint();
    uart_enable_txint();
    sei();
}

unsigned char uart_tx(unsigned char data) {
    //while the transmit buffer is not empty loop
    while(!(UCSR0A & (1<<UDRE0)));

    //when the buffer is empty write data to the transmitted
    UDR0 = data;
    uart_enable_txint)
    return data;
}

unsigned char uart_rx(void) {
  /* Wait for data to be received */
  while (!(UCSR0A & (1<<RXC0)));
  /* Get and return received data from buffer */
  return UDR0;
}

ISR(USART0_RX_vect)
{
  uart_push(UDR0);
  uart_disable_txint();
}


ISR(USART0_TX_vect)
{
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

void uart_putchar(unsigned char c){
//  unsigned short rc = uart_tx(c);
//  if (0 != rc) uart_push(rc);
  uart_tx(c);
}

void uart_putstr(char * str) {
  while (*str != 0) {
    uart_putchar((unsigned char)*str++);
  }
}

int uart_getchar(void) {
  if (fifo_rdptr == fifo_wrptr) return -1;
  cli();
  int rc = fifo[fifo_rdptr++];
  fifo_rdptr &= 7;
  sei();
  return rc;
}

int uart_peek(void) {
  return (fifo_wrptr - fifo_rdptr) & 7;
}

static void
put_halfhex
(unsigned char c)
{
  if (c < 10) uart_putchar('0' + c);
  else uart_putchar('A' - 10 + c);
}

void
uart_puthex
(unsigned char c)
{
  put_halfhex(c >> 4);
  put_halfhex(c & 15);
}

void
uart_putnum_u16
(unsigned short n, int digit)
{
  unsigned short d = 10000;
  if (digit > 0) {
    d = 1;
    for (digit--; digit > 0; digit--) d *= 10;
  }
  do {
    int num = n / d;
    n = n % d;
    d /= 10;
    uart_putchar('0' + num);
  } while (0 != d);
}

void
uart_puts
(char *s)
{
  while (0 != *s) uart_putchar(*s++);
}

void
uart_putsln
(char *s)
{
  uart_puts(s);
  uart_puts("\r\n");
}
