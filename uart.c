#include <msp430.h>
#include <legacymsp430.h>
#include <stdio.h>

#include "uart.h"
#include "fifo.h"

// uart tx/rx FIFO
fifo tx;
fifo rx;

// define putchar for link stdio with the UART
int putchar(int c)
{
  uart_putc(c);
  return 1;
}

// init hardware uart
void uart_init(void)
{
  // init FIFO
  fifo_init(&tx);
  fifo_init(&rx);
  // Rx = P1.1 and Tx = P1.2
  P1SEL     = BIT1 | BIT2;                       
  P1SEL2    = BIT1 | BIT2;
  // select SMCLK                 
  UCA0CTL1 |= UCSSEL_2;
  // baudrate 9600 (SMCLK = 1 MHz)
  UCA0BR0   = 104;
  UCA0BR1   = 0;
  // Modulation UCBRSx = 1
  UCA0MCTL  = UCBRS0;
  // Initialize USCI state machine
  UCA0CTL1 &= ~UCSWRST;
  // Enable USCI_A0 Rx interrupt
  IE2      |= UCA0RXIE; 
}

unsigned char uart_getc()
{
  return fifo_getc(&rx);
}

void uart_putc(unsigned char c)
{
  // add char in fifo, wait if tx fifo is full
  while (fifo_putc(&tx, c) == EOF);
  // start send (set tx interrupt on)
  IE2 |= UCA0TXIE;
}

// wait tx buffer is clean
void uart_wait_tx(void)
{
  while(fifo_size(&tx) != 0);
}

/*** tx/rx ISR area ***/

/* NOTICE: call subroutines on interrupt can do some stack problem so a large
part of fifo management code is include here and in fifo.c get/put functions. */

// interrupt service routine : Rx UART/I2C
interrupt(USCIAB0RX_VECTOR) USCI0RX_ISR(void)
{  
  // USCI A0 UART interrupt
  if (UC0IFG & UCA0RXIFG) {
    __disable_interrupt();
    char c = UCA0RXBUF;
    // store char in FIFO
    if (rx.size < FIFO_BUFFER_SIZE) {
      rx.data[rx.waddr] = c;
      rx.size++;
      if (rx.waddr < (FIFO_BUFFER_SIZE - 1))
        rx.waddr++;
      else
        rx.waddr =0;
    }
    __enable_interrupt();
  }
  // USCI BO I2C interrupt (not in use here)
}

// interrupt service routine : Tx UART/I2C
interrupt(USCIAB0TX_VECTOR) USCI0TX_ISR(void)
{  
  // USCI A0 UART interrupt
  if (IFG2 & UCA0TXIFG) {
    __disable_interrupt();
    // check fifo level
    if (tx.size == 0) {
      // empty fifo: stop tx interrupt
      IE2 &= ~UCA0TXIE;
    } else {
      // read current value
      UCA0TXBUF = tx.data[tx.raddr];
      tx.size--;
      // set read pointer
      if (tx.raddr < (FIFO_BUFFER_SIZE - 1))
        tx.raddr++;
      else
        tx.raddr = 0;
    }
    __enable_interrupt();
  }  
  // USCI BO I2C interrupt (not in use here)
}
