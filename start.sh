TESTS_DIR=tests/
ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator

ARGUMENT=$1

if [ "$ARGUMENT" = "asm" ]; then
  $ASSEMBLER -o ${TESTS_DIR}main.o ${TESTS_DIR}main.s
  $ASSEMBLER -o ${TESTS_DIR}math.o ${TESTS_DIR}math.s
  $ASSEMBLER -o ${TESTS_DIR}handler.o ${TESTS_DIR}handler.s
  $ASSEMBLER -o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_timer.s
  $ASSEMBLER -o ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_terminal.s
  $ASSEMBLER -o ${TESTS_DIR}isr_software.o ${TESTS_DIR}isr_software.s
elif [ "$ARGUMENT" = "link" ]; then
  $LINKER -hex \
    -place=my_code@0x40000000 -place=math@0xF0000000 \
    -o ${TESTS_DIR}program.hex \
    ${TESTS_DIR}handler.o ${TESTS_DIR}math.o ${TESTS_DIR}main.o \
    ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_software.o
elif [ "$ARGUMENT" = "emul" ]; then
  $EMULATOR ${TESTS_DIR}program.o
elif [ "$ARGUMENT" = "all" ]; then
  $ASSEMBLER -o ${TESTS_DIR}main.o ${TESTS_DIR}main.s
  $ASSEMBLER -o ${TESTS_DIR}math.o ${TESTS_DIR}math.s
  $ASSEMBLER -o ${TESTS_DIR}handler.o ${TESTS_DIR}handler.s
  $ASSEMBLER -o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_timer.s
  $ASSEMBLER -o ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_terminal.s
  $ASSEMBLER -o ${TESTS_DIR}isr_software.o ${TESTS_DIR}isr_software.s
  $LINKER -hex \
    -place=my_code@0x40000000 -place=math@0xF0000000 \
    -o ${TESTS_DIR}program.hex \
    ${TESTS_DIR}handler.o ${TESTS_DIR}math.o ${TESTS_DIR}main.o \
    ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_software.o
  $EMULATOR ${TESTS_DIR}program.o
else
  echo "Invalid argument"
fi
