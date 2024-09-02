BUILD_DIR = build
PROGRAM_NAME = assembler 

DEBUG_MODE = 0

BISON_FILE = misc/bison.y
FLEX_FILE = misc/flex.l

C_FILES = \
$(BUILD_DIR)/bison.tab.c \
$(BUILD_DIR)/lex.yy.c

CPP_FILES = \
src/Assembler.cpp \
src/CustomSection.cpp \
src/Directives.cpp \
src/ElfHeader.cpp \
src/ForwardReferenceTable.cpp \
src/Instructions.cpp \
src/LiteralTable.cpp \
src/main.cpp \
src/RelocationTable.cpp \
src/Section.cpp \
src/SectionHeaderTable.cpp \
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
	./$(PROGRAM_NAME) $(ARGS)

all: $(CPP_FILES) $(C_FILES) 
	g++ $(CXXFLAGS) -o $(PROGRAM_NAME) $(^) -lfl

$(BUILD_DIR)/bison.tab.c: $(BISON_FILE) Makefile | $(BUILD_DIR)
	bison $(BISONFLAGS) -o $(@) $(<)

$(BUILD_DIR)/lex.yy.c: $(FLEX_FILE) Makefile | $(BUILD_DIR)
	flex $(FLEXFLAGS) -o $(@) $(<)

$(BUILD_DIR):
	mkdir $(@)

clean:
	rm -rf $(BUILD_DIR) $(PROGRAM_NAME)