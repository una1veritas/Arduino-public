################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../z80asm/7seg_test.asm \
../z80asm/boot.asm \
../z80asm/division.asm \
../z80asm/example.asm \
../z80asm/example1.asm \
../z80asm/example2.asm \
../z80asm/example3.asm \
../z80asm/mon_0000.asm \
../z80asm/mon_v00.asm \
../z80asm/mon_v01.asm \
../z80asm/nascom8krombasic.asm \
../z80asm/rom_f000.asm \
../z80asm/rom_mon_F000.asm \
../z80asm/test.asm \
../z80asm/wozmon80.asm 

OBJS += \
./z80asm/7seg_test.o \
./z80asm/boot.o \
./z80asm/division.o \
./z80asm/example.o \
./z80asm/example1.o \
./z80asm/example2.o \
./z80asm/example3.o \
./z80asm/mon_0000.o \
./z80asm/mon_v00.o \
./z80asm/mon_v01.o \
./z80asm/nascom8krombasic.o \
./z80asm/rom_f000.o \
./z80asm/rom_mon_F000.o \
./z80asm/test.o \
./z80asm/wozmon80.o 


# Each subdirectory must supply rules for building sources it contributes
z80asm/%.o: ../z80asm/%.asm z80asm/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Assembler'
	avr-gcc -g -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-z80asm

clean-z80asm:
	-$(RM) ./z80asm/7seg_test.o ./z80asm/boot.o ./z80asm/division.o ./z80asm/example.o ./z80asm/example1.o ./z80asm/example2.o ./z80asm/example3.o ./z80asm/mon_0000.o ./z80asm/mon_v00.o ./z80asm/mon_v01.o ./z80asm/nascom8krombasic.o ./z80asm/rom_f000.o ./z80asm/rom_mon_F000.o ./z80asm/test.o ./z80asm/wozmon80.o

.PHONY: clean-z80asm

