TESTS_DIR=./tests
ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator

CURRENT_TEST=testA

# check if CURRENT_TEST is testA
if [ ${CURRENT_TEST}="testA" ]; then
    TEST_DIR=${TESTS_DIR}/testA/
    ${ASSEMBLER} -o ${TEST_DIR}main.o ${TEST_DIR}main.s
    ${ASSEMBLER} -o ${TEST_DIR}math.o ${TEST_DIR}math.s
    ${ASSEMBLER} -o ${TEST_DIR}handler.o ${TEST_DIR}handler.s
    ${ASSEMBLER} -o ${TEST_DIR}isr_timer.o ${TEST_DIR}isr_timer.s
    ${ASSEMBLER} -o ${TEST_DIR}isr_terminal.o ${TEST_DIR}isr_terminal.s
    ${ASSEMBLER} -o ${TEST_DIR}isr_software.o ${TEST_DIR}isr_software.s
    ${LINKER} -hex \
    -place=my_code@0x40000000 -place=math@0xF0000000 \
    -o program.hex \
    ${TEST_DIR}handler.o ${TEST_DIR}math.o ${TEST_DIR}main.o \
    ${TEST_DIR}isr_terminal.o ${TEST_DIR}isr_timer.o ${TEST_DIR}isr_software.o
    # ${EMULATOR} program.hex

# else 
#     TESTS_DIR=${TESTS_DIR}/myTest/
#     for file in ${TESTS_DIR}*.s; do
#         ${ASSEMBLER} -o ${file}.o ${file}
#     done
#     ${LINKER} -hex \
fi
