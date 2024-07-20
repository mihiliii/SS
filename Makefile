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
src/SectionContent.cpp \
src/Instructions.cpp

run: all
	./$(PROGRAM_NAME)

all: $(CPP_FILES) $(C_FILES) 
	g++ -Iinc -o $(PROGRAM_NAME) $(^) -lfl

$(BUILD_DIR)/bison.tab.c: $(BISON_FILE) Makefile | $(BUILD_DIR)
	bison -o $(@) -d $(<)

$(BUILD_DIR)/lex.yy.c: $(FLEX_FILE) Makefile | $(BUILD_DIR)
	flex -o $(@) $(<)

$(BUILD_DIR):
	mkdir $(@)

clean:
	rm -rf $(BUILD_DIR) $(PROGRAM_NAME)