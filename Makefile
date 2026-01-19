MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os
OBJ = main.o
TARGET = main

all: $(TARGET).hex

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex -R .eeprom $< $@

flash:
	avrdude -c arduino -p $(MCU) -P /dev/cu.usbmodem11401 -b 115200 -U flash:w:$(TARGET).hex

clean:
	rm -f *.o *.elf *.hex

