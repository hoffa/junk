SOURCES = cfg.c control.c engine.c main.c menu.c SDL_plus.c
TARGET = dd2.tns
OBJECTS = $(SOURCES:.c=.o)

CC = nspire-gcc
LD = nspire-ld
OBJCOPY = arm-none-eabi-objcopy
CFLAGS = -Wall -Wextra -Ofast -DDD2_DATA=\"\" -DVERSION=\"0.2.4\"
LDFLAGS =
LIBS = -lSDL -lfdm

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $(@:.tns=.elf)
	$(OBJCOPY) -O binary $(@:.tns=.elf) $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET:.tns=.elf) $(TARGET)