################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Z80Bus.cpp \
../progmem_rom.cpp \
../sloeber.ino.cpp 

INO_SRCS += \
../Z80_System.ino 

CPP_DEPS += \
./Z80Bus.d \
./progmem_rom.d \
./sloeber.ino.d 

INO_DEPS += \
./Z80_System.d 

OBJS += \
./Z80Bus.o \
./Z80_System.o \
./progmem_rom.o \
./sloeber.ino.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.ino subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean--2e-

clean--2e-:
	-$(RM) ./Z80Bus.d ./Z80Bus.o ./Z80_System.d ./Z80_System.o ./progmem_rom.d ./progmem_rom.o ./sloeber.ino.d ./sloeber.ino.o

.PHONY: clean--2e-

