SOURCES = link.c
TARGET = Link.tns
OBJECTS = $(SOURCES:.c=.o)

CC = nspire-gcc
LD = nspire-ld
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -Wall -Wextra -O3
LDFLAGS =

ifneq ($(findstring cx8,$(MAKECMDGOALS)),)
	CFLAGS += -D NSP_BPP_SW8_HW16 -D NSP_CX
	LIBS = -lSDL-CX8
else
	ifneq ($(findstring cx,$(MAKECMDGOALS)),)
		CFLAGS += -D NSP_BPP_SW16_HW16 -D NSP_CX
		LIBS = -lSDL-CX
	endif
endif

ifneq ($(findstring tc,$(MAKECMDGOALS)),)
	CFLAGS += -D NSP_BPP_SW8_HW4 -D NSP_TC
	LIBS = -lSDL-TC
endif

cx: all
cx8: all
tc: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET:.tns=.elf) $(TARGET)