################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../SPISRAM_memblock_test.ino 

INO_DEPS += \
./SPISRAM_memblock_test.d 

OBJS += \
./SPISRAM_memblock_test.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.ino subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean--2e-

clean--2e-:
	-$(RM) ./SPISRAM_memblock_test.d ./SPISRAM_memblock_test.o

.PHONY: clean--2e-

