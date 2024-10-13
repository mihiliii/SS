DIR=./tests/
ASSEMBLER=./assembler
LINKER=./linker
EMULATOR=./emulator

${ASSEMBLER} -o ${DIR}main.o ${DIR}main.s
${ASSEMBLER} -o ${DIR}math.o ${DIR}math.s
${ASSEMBLER} -o ${DIR}handler.o ${DIR}handler.s
${ASSEMBLER} -o ${DIR}isr_timer.o ${DIR}isr_timer.s
${ASSEMBLER} -o ${DIR}isr_terminal.o ${DIR}isr_terminal.s
${ASSEMBLER} -o ${DIR}isr_software.o ${DIR}isr_software.s
# ${LINKER} -hex \
#   -place=my_code@0x40000000 -place=math@0xF0000000 \
#   -o program.hex \
#   handler.o math.o main.o isr_terminal.o isr_timer.o isr_software.o
# ${EMULATOR} program.hex