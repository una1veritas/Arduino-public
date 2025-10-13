################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../cpmdisk/8in2s4d.asm 

OBJS += \
./cpmdisk/8in2s4d.o 


# Each subdirectory must supply rules for building sources it contributes
cpmdisk/%.o: ../cpmdisk/%.asm cpmdisk/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Assembler'
	avr-gcc -g -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-cpmdisk

clean-cpmdisk:
	-$(RM) ./cpmdisk/8in2s4d.o

.PHONY: clean-cpmdisk

