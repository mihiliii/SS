BUILD_DIR = build
PROGRAM_NAME = executable

BISON_FILE = misc/bison.y
FLEX_FILE = misc/flex.l

C_FILES = \
$(BUILD_DIR)/bison.tab.c \
$(BUILD_DIR)/lex.yy.c

CPP_FILES = \
src/Assembler.cpp \
src/main.cpp \
src/Section.cpp \
src/Instructions.cpp \
src/SectionHeaderTable.cpp \
src/SymbolTable.cpp \
src/StringTable.cpp \
src/Directives.cpp \
src/InputSection.cpp \


CXXFLAGS = -Wall -Iinc -g -std=c++2a

run: all
	./$(PROGRAM_NAME)

all: $(CPP_FILES) $(C_FILES) 
	g++ $(CXXFLAGS) -o $(PROGRAM_NAME) $(^) -lfl

$(BUILD_DIR)/bison.tab.c: $(BISON_FILE) Makefile | $(BUILD_DIR)
	bison -o $(@) -d $(<)

$(BUILD_DIR)/lex.yy.c: $(FLEX_FILE) Makefile | $(BUILD_DIR)
	flex -o $(@) $(<)

$(BUILD_DIR):
	mkdir $(@)

clean:
	rm -rf $(BUILD_DIR) $(PROGRAM_NAME)