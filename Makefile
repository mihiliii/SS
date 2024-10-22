BUILD_DIR = build
ASSEMBLER_PROGRAM_NAME = assembler
LINKER_PROGRAM_NAME = linker

DEBUG_MODE = 0

BISON_FILE = misc/bison.y
FLEX_FILE = misc/flex.l

C_FILES = \
$(BUILD_DIR)/bison.tab.c \
$(BUILD_DIR)/lex.yy.c

ASSEMBLER_FILES =\
$(wildcard src/Assembler/*.cpp) \

LINKER_FILES =\
$(wildcard src/Linker/*.cpp) \

COMMON_FILES =\
$(wildcard src/*.cpp) \

CXXFLAGS = -Wall -Iinc -g -std=c++2a
FLEXFLAGS =
BISONFLAGS = -d

ifeq ($(DEBUG_MODE), 1)
	FLEXFLAGS += -d
endif

all: assembler linker

linker: $(LINKER_FILES) $(COMMON_FILES)
	g++ $(CXXFLAGS) -o $(LINKER_PROGRAM_NAME) $(^)

assembler: $(ASSEMBLER_FILES) $(COMMON_FILES) $(C_FILES) 
	g++ $(CXXFLAGS) -o $(ASSEMBLER_PROGRAM_NAME) $(^) -lfl

$(BUILD_DIR)/bison.tab.c: $(BISON_FILE) Makefile | $(BUILD_DIR)
	bison $(BISONFLAGS) -o $(@) $(<)

$(BUILD_DIR)/lex.yy.c: $(FLEX_FILE) Makefile | $(BUILD_DIR)
	flex -o $(@) $(FLEXFLAGS) $(<)

$(BUILD_DIR):
	mkdir $(@)

clean:
	rm -rf $(BUILD_DIR) $(ASSEMBLER_PROGRAM_NAME) $(LINKER_PROGRAM_NAME)

clean_all: clean
	find . -type f -name "*.o" -delete
	find . -type f -name "*.hex" -delete