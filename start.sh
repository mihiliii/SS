TESTS_DIR=./tests/testA/
ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator

# check if CURRENT_TEST is testA
if [ ${TESTS_DIR}="./tests/testA/" ]; then
    ${ASSEMBLER} -o ${TESTS_DIR}main.o ${TESTS_DIR}main.s
    ${ASSEMBLER} -o ${TESTS_DIR}math.o ${TESTS_DIR}math.s
    ${ASSEMBLER} -o ${TESTS_DIR}handler.o ${TESTS_DIR}handler.s
    ${ASSEMBLER} -o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_timer.s
    ${ASSEMBLER} -o ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_terminal.s
    ${ASSEMBLER} -o ${TESTS_DIR}isr_software.o ${TESTS_DIR}isr_software.s
    ${LINKER} -hex \
    -place=my_code@0x40000000 -place=math@0xF0000000 \
    -o ${TESTS_DIR}program.hex \
    ${TESTS_DIR}handler.o ${TESTS_DIR}math.o ${TESTS_DIR}main.o \
    ${TESTS_DIR}isr_terminal.o ${TESTS_DIR}isr_timer.o ${TESTS_DIR}isr_software.o
    ${EMULATOR} ${TESTS_DIR}program.o
fi
