BUILD_DIR = build
ASSEMBLER_PROGRAM_NAME = assembler 

DEBUG_MODE = 0

BISON_FILE = misc/bison.y
FLEX_FILE = misc/flex.l

C_FILES = \
$(BUILD_DIR)/bison.tab.c \
$(BUILD_DIR)/lex.yy.c

CPP_FILES = \
src/Assembler/Assembler.cpp \
src/Assembler/CustomSection.cpp \
src/Assembler/Directives.cpp \
src/Assembler/ElfHeader.cpp \
src/Assembler/ForwardReferenceTable.cpp \
src/Assembler/Instructions.cpp \
src/Assembler/LiteralTable.cpp \
src/Assembler/main.cpp \
src/Assembler/RelocationTable.cpp \
src/Assembler/Section.cpp \
src/Assembler/SectionHeaderTable.cpp \
src/Assembler/StringTable.cpp \
src/Assembler/SymbolTable.cpp \


CXXFLAGS = -Wall -Iinc -g -std=c++2a
FLEXFLAGS =
BISONFLAGS = -d

ARGS = -o test.o test.s

ifeq ($(DEBUG_MODE), 1)
	FLEXFLAGS += -d
endif

run: all
	./$(ASSEMBLER_PROGRAM_NAME) $(ARGS)

all: $(CPP_FILES) $(C_FILES) 
	g++ $(CXXFLAGS) -o $(ASSEMBLER_PROGRAM_NAME) $(^) -lfl

$(BUILD_DIR)/bison.tab.c: $(BISON_FILE) Makefile | $(BUILD_DIR)
	bison $(BISONFLAGS) -o $(@) $(<)

$(BUILD_DIR)/lex.yy.c: $(FLEX_FILE) Makefile | $(BUILD_DIR)
	flex $(FLEXFLAGS) -o $(@) $(<)

$(BUILD_DIR):
	mkdir $(@)

clean:
	rm -rf $(BUILD_DIR) $(ASSEMBLER_PROGRAM_NAME)