PORT=/dev/cu.usbmodem1411

ifndef VIRTUAL_SERIAL_PATH
$(warning VIRTUAL_SERIAL_PATH must be set to use usb virtual serial)
else
CFLAGS+= -I$(VIRTUAL_SERIAL_PATH) -DVIRTUAL_SERIAL
LDFLAGS+=  -L$(VIRTUAL_SERIAL_PATH) -lVirtualSerial
endif

ifneq ($(filter program,$(MAKECMDGOALS)),$())
ifndef PORT
$(error PORT must be set to program device)
endif
endif

MCU=atmega32u4
CFLAGS+= -g -w -mcall-prologues -mmcu=$(MCU) -Os -std=c99
LDFLAGS+= -Wl,-gc-sections -Wl,-relax
CC=avr-gcc
TARGET=main
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET).hex

clean:
	rm -f *.o *.hex *.obj *.hex

%.hex: %.obj
	avr-objcopy -R .eeprom -O ihex $< $@

%.obj: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

program: $(TARGET).hex
	avrdude -p $(MCU) -c avr109 -P $(PORT) -U flash:w:$(TARGET).hex
