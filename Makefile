CC=avr-gcc
CFLAGS=-funsigned-char -funsigned-bitfields -Os -fpack-struct -fshort-enums -g2 -Wall -Werror -c -Wno-pointer-sign  -mmcu=attiny2313 -DF_CPU=1000000L

.PHONY: all

all: main.hex

OBJS=main.o

main: $(OBJS)
	$(CC) -mmcu=attiny2313 -o $@ $(OBJS)

main.hex: main
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature $< $@

clean:
	rm -f main $(OBJS) main.hex

flash: main.hex
	avrdude -c usbasp -p t2313 -F -B 64 -U flash:w:main.hex

