TESTS_DIR=tests/
ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator
READELF32=./readelf32

DO_READ_ELF=false
PROGRAM_ARG=""

for arg in "$@"; do
  if [[ "$arg" == "-d" ]]; then
    DO_READ_ELF=true
  elif [[ -z "$ARGUMENT" ]]; then
    PROGRAM_ARG="$arg"
  fi
done

if [ "$PROGRAM_ARG" = "asm" ]; then
  $ASSEMBLER -o ${TESTS_DIR}main.o ${TESTS_DIR}main.s
  $ASSEMBLER -o ${TESTS_DIR}math.o ${TESTS_DIR}math.s
  $ASSEMBLER -o ${TESTS_DIR}handler.o ${TESTS_DIR}handler.s
  $ASSEMBLER -o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_timer.s
  $ASSEMBLER -o ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_terminal.s
  $ASSEMBLER -o ${TESTS_DIR}isr_software.o ${TESTS_DIR}isr_software.s
  if $DO_READ_ELF; then
    $READELF32 ${TESTS_DIR}main.o
    $READELF32 ${TESTS_DIR}math.o
    $READELF32 ${TESTS_DIR}handler.o
    $READELF32 ${TESTS_DIR}isr_timer.o
    $READELF32 ${TESTS_DIR}isr_terminal.o
    $READELF32 ${TESTS_DIR}isr_software.o
  fi

elif [ "$PROGRAM_ARG" = "link" ]; then
  $LINKER -hex \
    -place=my_code@0x40000000 -place=math@0xF0000000 \
    -o ${TESTS_DIR}program.hex \
    ${TESTS_DIR}handler.o \
    ${TESTS_DIR}math.o \
    ${TESTS_DIR}main.o \
    ${TESTS_DIR}isr_terminal.o \
    ${TESTS_DIR}isr_timer.o \
    ${TESTS_DIR}isr_software.o
  if $DO_READ_ELF; then
    $READELF32 ${TESTS_DIR}program.o
  fi

elif [ "$PROGRAM_ARG" = "emul" ]; then
  $EMULATOR ${TESTS_DIR}program.o

elif [ "$PROGRAM_ARG" = "all" ]; then
  $ASSEMBLER -o ${TESTS_DIR}main.o ${TESTS_DIR}main.s
  $ASSEMBLER -o ${TESTS_DIR}math.o ${TESTS_DIR}math.s
  $ASSEMBLER -o ${TESTS_DIR}handler.o ${TESTS_DIR}handler.s
  $ASSEMBLER -o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_timer.s
  $ASSEMBLER -o ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_terminal.s
  $ASSEMBLER -o ${TESTS_DIR}isr_software.o ${TESTS_DIR}isr_software.s
  if $DO_READ_ELF; then
    $READELF32 ${TESTS_DIR}main.o
    $READELF32 ${TESTS_DIR}math.o
    $READELF32 ${TESTS_DIR}handler.o
    $READELF32 ${TESTS_DIR}isr_timer.o
    $READELF32 ${TESTS_DIR}isr_terminal.o
    $READELF32 ${TESTS_DIR}isr_software.o
  fi
  $LINKER -hex \
    -place=my_code@0x40000000 -place=math@0xF0000000 \
    -o ${TESTS_DIR}program.hex \
    ${TESTS_DIR}handler.o \
    ${TESTS_DIR}math.o \
    ${TESTS_DIR}main.o \
    ${TESTS_DIR}isr_terminal.o \
    ${TESTS_DIR}isr_timer.o \
    ${TESTS_DIR}isr_software.o
  if $DO_READ_ELF; then
    $READELF32 ${TESTS_DIR}program.o
  fi
  $EMULATOR ${TESTS_DIR}program.o

else
  echo "Invalid argument"
fi
