################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../common.cpp \
../ihex_processor.cpp \
../srec_processor.cpp 

INO_SRCS += \
../UniProgrammer.ino 

CPP_DEPS += \
./common.d \
./ihex_processor.d \
./srec_processor.d 

INO_DEPS += \
./UniProgrammer.d 

OBJS += \
./UniProgrammer.o \
./common.o \
./ihex_processor.o \
./srec_processor.o 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.ino subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.cpp subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean--2e-

clean--2e-:
	-$(RM) ./UniProgrammer.d ./UniProgrammer.o ./common.d ./common.o ./ihex_processor.d ./ihex_processor.o ./srec_processor.d ./srec_processor.o

.PHONY: clean--2e-

