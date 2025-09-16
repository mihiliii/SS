TESTS_DIR=tests/
ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator
READELF32=./readelf32

ARGUMENT=$1

if [ "$ARGUMENT" = "asm" ]; then
  $ASSEMBLER -o ${TESTS_DIR}main.o ${TESTS_DIR}main.s
  $ASSEMBLER -o ${TESTS_DIR}math.o ${TESTS_DIR}math.s
  $ASSEMBLER -o ${TESTS_DIR}handler.o ${TESTS_DIR}handler.s
  $ASSEMBLER -o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_timer.s
  $ASSEMBLER -o ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_terminal.s
  $ASSEMBLER -o ${TESTS_DIR}isr_software.o ${TESTS_DIR}isr_software.s
  $READELF32 ${TESTS_DIR}main.o
  $READELF32 ${TESTS_DIR}math.o
  $READELF32 ${TESTS_DIR}handler.o
  $READELF32 ${TESTS_DIR}isr_timer.o
  $READELF32 ${TESTS_DIR}isr_terminal.o
  $READELF32 ${TESTS_DIR}isr_software.o

elif [ "$ARGUMENT" = "link" ]; then
  $LINKER -hex \
    -place=my_code@0x40000000 -place=math@0xF0000000 \
    -o ${TESTS_DIR}program.hex \
    ${TESTS_DIR}handler.o \
    ${TESTS_DIR}math.o \
    ${TESTS_DIR}main.o \
    ${TESTS_DIR}isr_terminal.o \
    ${TESTS_DIR}isr_timer.o \
    ${TESTS_DIR}isr_software.o
  $READELF32 ${TESTS_DIR}program.o

elif [ "$ARGUMENT" = "emul" ]; then
  $EMULATOR ${TESTS_DIR}program.o

elif [ "$ARGUMENT" = "all" ]; then
  $ASSEMBLER -o ${TESTS_DIR}main.o ${TESTS_DIR}main.s
  $ASSEMBLER -o ${TESTS_DIR}math.o ${TESTS_DIR}math.s
  $ASSEMBLER -o ${TESTS_DIR}handler.o ${TESTS_DIR}handler.s
  $ASSEMBLER -o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_timer.s
  $ASSEMBLER -o ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_terminal.s
  $ASSEMBLER -o ${TESTS_DIR}isr_software.o ${TESTS_DIR}isr_software.s
  $READELF32 ${TESTS_DIR}main.o
  $READELF32 ${TESTS_DIR}math.o
  $READELF32 ${TESTS_DIR}handler.o
  $READELF32 ${TESTS_DIR}isr_timer.o
  $READELF32 ${TESTS_DIR}isr_terminal.o
  $READELF32 ${TESTS_DIR}isr_software.o
  $LINKER -hex \
    -place=my_code@0x40000000 -place=math@0xF0000000 \
    -o ${TESTS_DIR}program.hex \
    ${TESTS_DIR}handler.o \
    ${TESTS_DIR}math.o \
    ${TESTS_DIR}main.o \
    ${TESTS_DIR}isr_terminal.o \
    ${TESTS_DIR}isr_timer.o \
    ${TESTS_DIR}isr_software.o
  $READELF32 ${TESTS_DIR}program.o
  $EMULATOR ${TESTS_DIR}program.o

else
  echo "Invalid argument"
fi
