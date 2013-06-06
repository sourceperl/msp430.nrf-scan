#ifndef __UART_H
#define __UART_H

/*
 * initialize soft UART
 */
void uart_init(void);

/*
 * read one character from UART blocking.
 *
 * @return	character received 
 */
unsigned char uart_getc(void);

/*
 * write one chracter to the UART blocking.
 *
 * @param[in]	*c	the character to write
 */
void uart_putc(unsigned char c);

/*
 * wait tx buffer is clean (= tx fifo empty)
 */
void uart_wait_tx(void);

#endif
