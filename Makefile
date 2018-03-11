# cstruct Makefile

INC = src
CFLAGS += $(addprefix -I,$(INC)) -g -std=c99 -Wall -Wpedantic
LIBNAME = src/cstruct.o

TEXT_RED = \033[31;1m
TEXT_GREEN = \033[32;1m
TEXT_BLUE = \033[34;1m
TEXT_END = \033[0m

RED_TEXT = "$(TEXT_RED)$(1)$(TEXT_END)"
GREEN_TEXT = "$(TEXT_GREEN)$(1)$(TEXT_END)"
BLUE_TEXT = "$(TEXT_BLUE)$(1)$(TEXT_END)"

all: lib test

%.o: %.c
	gcc -c $< $(CFLAGS) -o $@ -MMD

lib: $(LIBNAME)
	@echo $(call BLUE_TEXT,Built lib.)

test: test/test.o lib
	gcc -o test/test $< $(LIBNAME)
	@echo $(call BLUE_TEXT,Built test, running it.)
	@./test/test && echo $(call GREEN_TEXT,Test passed.) || echo $(call RED_TEXT,Test failed)

clean:
	- rm -rf **/*.o **/*.d test/test
