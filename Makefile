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
src/Assembler/Assembler.cpp \
src/Assembler/Directives.cpp \
src/Assembler/ForwardReferenceTable.cpp \
src/Assembler/Instructions.cpp \
src/Assembler/LiteralTable.cpp \
src/Assembler/main.cpp \
src/CustomSection.cpp \
src/Elf32File.cpp \
src/RelocationTable.cpp \
src/Section.cpp \
src/StringTable.cpp \
src/SymbolTable.cpp \

LINKER_FILES =\
src/Linker/Linker.cpp \
src/Linker/main.cpp \
src/Assembler/LiteralTable.cpp \
src/CustomSection.cpp \
src/Elf32File.cpp \
src/RelocationTable.cpp \
src/Section.cpp \
src/StringTable.cpp \
src/SymbolTable.cpp \


CXXFLAGS = -Wall -Iinc -g -std=c++2a
FLEXFLAGS =
BISONFLAGS = -d

ARGS = -o test.o test.s

ifeq ($(DEBUG_MODE), 1)
	FLEXFLAGS += -d
endif

run: all
	./$(ASSEMBLER_PROGRAM_NAME) $(ARGS)

all: assembler linker

linker: $(LINKER_FILES)
	g++ $(CXXFLAGS) -o $(LINKER_PROGRAM_NAME) $(^)

assembler: $(ASSEMBLER_FILES) $(C_FILES) 
	g++ $(CXXFLAGS) -o $(ASSEMBLER_PROGRAM_NAME) $(^) -lfl

$(BUILD_DIR)/bison.tab.c: $(BISON_FILE) Makefile | $(BUILD_DIR)
	bison $(BISONFLAGS) -o $(@) $(<)

$(BUILD_DIR)/lex.yy.c: $(FLEX_FILE) Makefile | $(BUILD_DIR)
	flex -o $(@) $(FLEXFLAGS) $(<)

$(BUILD_DIR):
	mkdir $(@)

clean:
	rm -rf $(BUILD_DIR) $(ASSEMBLER_PROGRAM_NAME) $(LINKER_PROGRAM_NAME)