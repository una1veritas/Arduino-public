MCU = atmega2560
F_CPU = 16000000UL
CC = /opt/homebrew/bin/avr-gcc
OBJCOPY = /opt/homebrew/bin/avr-objcopy
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra
TARGET = usart_demo
SRCS = atmega2560_usart.c example_main.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

clean:
	rm -f $(OBJS) $(TARGET).elf $(TARGET).hex

flash: $(TARGET).hex
	# adjust PROGRAMMER and PORT as needed for your setup
	#avrdude -p m2560 -c stk500v2 -P /dev/ttyACM0 -b 115200 -U flash:w:$(TARGET).hex:i
	avrdude -v -p m2560 -c stk500v2 -P /dev/tty.usbmodem312201 -b 115200 -D -U flash:w:usart_demo.hex:i

.PHONY: all clean flash
