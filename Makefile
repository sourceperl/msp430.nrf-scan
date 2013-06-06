TARGET := main.elf
CC     := msp430-gcc
CFLAGS := -Os -Wall -mmcu=msp430g2553 -mdisable-watchdog
OBJS   += main.o uart.o fifo.o spi.o nrf.o

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(CFLAGS) $(OBJS)

clean:
	rm -rf $(TARGET) $(OBJS)

prog: $(TARGET)
	mspdebug -q rf2500 --force-reset "prog $(TARGET)"

size: $(TARGET)
	msp430-size -A $(TARGET)
