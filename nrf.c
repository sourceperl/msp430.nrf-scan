/*
  MSP430 code for SPI interface with nRF24L01+
*/

#include <msp430.h>
#include <legacymsp430.h>
#include <stdio.h>
#include "spi.h"
#include "nRF24L01P.h"

#include "nrf.h"

// prototypes
unsigned char nrf_reg_read(unsigned char reg);
void nrf_reg_write(unsigned char reg, unsigned char val);

unsigned char nrf_reg_read(unsigned char reg)
{
  unsigned char val;
  spi_csl();
  // send command
  spi_xfer_byte(NRF_R_REGISTER | reg);
  // read response
  val = spi_xfer_byte(NRF_NOP);
  spi_csh();
  return val;
}

void nrf_reg_write(unsigned char reg, unsigned char val)
{
  spi_csl();
  // send command
  spi_xfer_byte(NRF_W_REGISTER | reg);
  // read response
  spi_xfer_byte(val);
  spi_csh();
}
