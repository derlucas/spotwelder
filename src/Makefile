MCU=attiny2313
PROGRAMMER_MCU=t2313
PROJECTNAME=main
F_CPU=8000000
AVRDUDE_PROGRAMMERID=usbasp
AVRDUDE_PORT=/dev/tty

LFUSE=0xE4
HFUSE=0xD9
EFUSE=0xFF

#source files
#PRJSRC=$(shell echo *.c)
PRJSRC=main.c

# additional includes (e.g. -I/path/to/mydir)
INC=

# libraries to link in (e.g. -lmylib)
LIBS=

# Optimization level, 
# use s (size opt), 1, 2, 3 or 0 (off)
OPTLEVEL=s

# compiler
CFLAGS=-I. $(INC) -g -DF_CPU=$(F_CPU) -mmcu=$(MCU) -O$(OPTLEVEL) \
	-fpack-struct -fshort-enums             \
	-funsigned-bitfields -funsigned-char    \
	-Wall -Wstrict-prototypes               \
	-Wa,-ahlms=$(firstword                  \
	$(filter %.lst, $(<:.c=.lst)))

# linker
LDFLAGS=-Wl,-Map,$(TRG).map -mmcu=$(MCU) -lm $(LIBS)

##### executables ####
CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
SIZE=avr-size
AVRDUDE=avrdude
REMOVE=rm -f

##### automatic target names ####
TRG=$(PROJECTNAME).out
HEXROMTRG=$(PROJECTNAME).hex 
HEXTRG=$(HEXROMTRG)
# $(PROJECTNAME).eep.hex

#  C
CFILES=$(filter %.c, $(PRJSRC))

# List all object files we need to create
OBJDEPS=$(CFILES:.c=.o)

# Define all lst files.
LST=$(filter %.lst, $(OBJDEPS:.o=.lst))

# All the possible generated assembly 
# files (.s files)
GENASMFILES=$(filter %.s, $(OBJDEPS:.o=.s)) 


.SUFFIXES : .c .cc .cpp .C .o .out .s .S .hex .ee.hex .h .hh .hpp


.PHONY: writeflash clean stats gdbinit stats

# Make targets:
all: $(TRG) hex stats #writeflash

stats: $(TRG)
	$(OBJDUMP) -h $(TRG)
	$(SIZE) $(TRG) 

hex: $(HEXTRG)


program: hex
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) -p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) -e -U flash:w:$(HEXROMTRG)

fuse: hex
	$(AVRDUDE) -c $(AVRDUDE_PROGRAMMERID) -p $(PROGRAMMER_MCU) -P $(AVRDUDE_PORT) -e -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m -U efuse:w:$(EFUSE):m 


$(TRG): $(OBJDEPS) 
	$(CC) $(LDFLAGS) -o $(TRG) $(OBJDEPS)


#### Generating object files ####
# object from C
.c.o: 
	$(CC) $(CFLAGS) -c $< -o $@


#### Generating hex files ####
# hex files from elf
.out.hex:
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

.out.eep.hex:
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@


#### Cleanup ####
clean:
	$(REMOVE) $(TRG) $(TRG).map $(DUMPTRG)
	$(REMOVE) $(OBJDEPS)
	$(REMOVE) $(LST) $(GDBINITFILE)
	$(REMOVE) $(GENASMFILES)
	$(REMOVE) $(HEXTRG)
	
