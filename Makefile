BUILD_DIR = build
ASSEMBLER_PROGRAM_NAME = assembler
LINKER_PROGRAM_NAME = linker
EMULATOR_PROGRAM_NAME = emulator
READELF32_PROGRAM_NAME = readelf32

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

EMULATOR_FILES =\
$(wildcard src/Emulator/*.cpp) \

ELF32_FILES =\
$(wildcard src/Elf32/*.cpp) \

READELF32_FILES =\
$(wildcard src/ReadElf32/*.cpp) \

CXXFLAGS = -Wall -Iinc -g -std=c++2a
FLEXFLAGS =
BISONFLAGS = -d

ifeq ($(DEBUG_MODE), 1)
	FLEXFLAGS += -d
endif

all: assembler linker emulator readelf32

assembler: $(ASSEMBLER_FILES) $(ELF32_FILES) $(C_FILES)
	g++ $(CXXFLAGS) -o $(ASSEMBLER_PROGRAM_NAME) $(^) -lfl

linker: $(LINKER_FILES) $(ELF32_FILES)
	g++ $(CXXFLAGS) -o $(LINKER_PROGRAM_NAME) $(^)

emulator: $(EMULATOR_FILES) $(ELF32_FILES)
	g++ $(CXXFLAGS) -o $(EMULATOR_PROGRAM_NAME) $(^)

readelf32: $(READELF32_FILES) $(ELF32_FILES)
	g++ $(CXXFLAGS) -o $(READELF32_PROGRAM_NAME) $(^)

$(BUILD_DIR)/bison.tab.c: $(BISON_FILE) Makefile | $(BUILD_DIR)
	bison $(BISONFLAGS) -o $(@) $(<)

$(BUILD_DIR)/lex.yy.c: $(FLEX_FILE) Makefile | $(BUILD_DIR)
	flex -o $(@) $(FLEXFLAGS) $(<)

$(BUILD_DIR):
	mkdir $(@)

clean:
	rm -rf $(BUILD_DIR) $(ASSEMBLER_PROGRAM_NAME) $(LINKER_PROGRAM_NAME) $(EMULATOR_PROGRAM_NAME) $(READELF32_PROGRAM_NAME)

clean_assembler:
	rm -rf $(ASSEMBLER_PROGRAM_NAME)

clean_linker:
	rm -rf $(LINKER_PROGRAM_NAME)

clean_emulator:
	rm -rf $(EMULATOR_PROGRAM_NAME)

clean_all: clean
	find . -type f -name "*.o" -delete
	find . -type f -name "*.hex" -delete
