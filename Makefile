MISC_DIR = misc 
BUILD_DIR = build
INC_DIR = inc
SRC_DIR = src

PROGRAM = executable

BISON_FILE = misc/bison.y
FLEX_FILE = misc/flex.l

C_FILES = \
$(BUILD_DIR)/bison.tab.c \
$(BUILD_DIR)/lex.yy.c

CPP_FILES = \
src/Assembler.cpp \
src/main.cpp

run: all
	./executable

all: parse_only cpp_only

cpp_only: $(CPP_FILES)
	g++ -o executable $(^)

parse_only: $(C_FILES)
	g++ -o $(@) $(^) -lfl

$(BUILD_DIR)/bison.tab.c: $(BISON_FILE) Makefile | $(BUILD_DIR)
	bison -o $(@) -d $(<)

$(BUILD_DIR)/lex.yy.c: $(FLEX_FILE) Makefile | $(BUILD_DIR)
	flex -o $(@) $(<)

$(BUILD_DIR):
	mkdir $(@)

clean:
	rm -rf $(BUILD_DIR) parse_only $(PROGRAM)