#ifndef __FIFO_H
#define __FIFO_H

#define FIFO_BUFFER_SIZE 32

// FIFO struct
typedef struct 
{
  volatile unsigned char data[FIFO_BUFFER_SIZE];
  volatile unsigned char size;
  volatile unsigned char raddr;
  volatile unsigned char waddr;
} fifo;

// FIFO access routines
         void fifo_init(fifo *_fifo);
          int fifo_getc(fifo *_fifo); 
          int fifo_putc(fifo *_fifo, int c);
unsigned char fifo_size(fifo *_fifo);

#endif 
