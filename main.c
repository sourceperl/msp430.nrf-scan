/*
  simple scanner with Nordic semiconductor nRF24L01+  
  code for MSP430G2553

  test every nrf channels from 2400 MHz to 2525 MHz.
  - display result on console like it :

  channel number in hex on 1 column
          
  hit for this channel (max 9) on 1 column


  UART set to 9600,N,8,1

  code for msp430-gcc
  MSP430G2553 on internal DCO at 1 MHz

  I/O map
  ******* 
  # P1.2 UART tx of MSP430
  # P1.3 SPI CS nrf24L01+ (Chip Select)
  # P1.4     CE nrf24L01+ (Chip Enable)
  # P1.5 SPI Clock
  # P1.6 SPI MISO (master in)
  # P1.7 SPI MOSI (master out)

  code in public domain
  share, it's happiness !
*/

#include <msp430.h>
#include <legacymsp430.h>
#include <stdio.h>
#include "uart.h"
#include "fifo.h"
#include "spi.h"
#include "nrf.h"
#include "nRF24L01P.h"

// some const
#define NRF_CHANNELS 126

// prototypes
void _nrf_scan(void); 

// some global vars
unsigned char i;
unsigned char channel[NRF_CHANNELS];

/*
 * main routines
 */
int main(void)
{
  // stop watchdog
  WDTCTL = WDTPW | WDTHOLD;
  // init internal digitally controlled oscillator DCO
  BCSCTL1 = CALBC1_1MHZ;
  DCOCTL  = CALDCO_1MHZ;
  // IO setup (launchpad : bit 0 -> red led, bit 6 -> green led, bit4 -> CE nRF24L01+)
  P1DIR  |= BIT0 | BIT6 | BIT4;
  P1OUT  &= ~(BIT0 | BIT6 | BIT4);
  // UART init
  uart_init();
  // SPI init (use by nrf.h)
  spi_init();
  // start interrupt
  __enable_interrupt();

  // hello msg
  printf("\n\r\n\rChannel from 2400 MHz to 2525 MHz (1 channel = 1 MHz):\n\r");

  // print out header, high then low digit
  i = 0;
  while(i < NRF_CHANNELS)
    printf("%x",i++>>4);
  printf("\n\r");
  i = 0;
  while(i < NRF_CHANNELS)
    printf("%x",i++&0xf);
  printf("\n\r\n\r");

  // main program loop
  while(1) {
    // do scan
    _nrf_scan();
  }
}

void _nrf_scan(void) {
  // CE low
  P1OUT  &= ~(BIT4);
  // Power Up nRF24L01+
  nrf_reg_write(NRF_CONFIG, nrf_reg_read(NRF_CONFIG) | NRF_PWR_UP);
  // SchockBurst off
  nrf_reg_write(NRF_EN_AA, 0x00);
  // RF power = 0 dBm, data rate at 2 MBPS
  nrf_reg_write(NRF_RF_SETUP, NRF_TX_PWR_0DB | NRF_DR_2MBPS);
  // reset channel statuts array
  for(i = 0; i < NRF_CHANNELS; i++)
    channel[i] = 0;
  // scan loop
  unsigned char _sample;
  unsigned char _channel;
  for(_sample = 0; _sample < 75; _sample++) {
    for(_channel = 0; _channel < NRF_CHANNELS ; _channel++) {
      // set channel
      nrf_reg_write(NRF_RF_CH, _channel);
      // nRF24 in PRX mode
      nrf_reg_write(NRF_CONFIG, nrf_reg_read(NRF_CONFIG) | NRF_PRIM_RX);
      // CE high
      P1OUT  |= BIT4;
      // delay 300 us
      __delay_cycles(300);
      // CE low
      P1OUT  &= ~(BIT4);
      // RPD bit0 is set, if signal > -64 dBm during CE high state
      if((nrf_reg_read(NRF_RPD) & 0x01) & (channel[_channel] < 9))
        channel[_channel]++;
    }
  }
  // print result
  for(i = 0; i < NRF_CHANNELS; i++)
    printf("%d", channel[i]);
  printf("\n\r");
}
