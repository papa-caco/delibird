RM=rm -rf
CC=gcc

C_SRCS=$(shell find . -iname "*.c" | tr '\n' ' ')
H_SRCS=$(shell find . -iname "*.h" | tr '\n' ' ')
OBJS=$(C_SRCS:./%.c=debug/%.o)

# Clean and compile .so
all: build/libdelibird.so

create-dirs:
	mkdir -p debug/delibird
	mkdir -p build/

build/libdelibird.so: create-dirs $(OBJS)
	$(CC) -shared -o "build/libdelibird.so" $(OBJS)

debug/delibird/%.o: delibird/%.c
	$(CC) -c -fmessage-length=0 -fPIC -Wno-switch-enum -Werror -g  -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

# Add debug parameters and compile
debug: CC += -DDEBUG -g
debug: all

# Clean build files
clean:
	$(RM) build
	$(RM) debug

.PHONY: all create-dirs clean
