################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../HardwareVerify/PromAddressDriver.cpp \
../HardwareVerify/PromDevice.cpp \
../HardwareVerify/PromDevice28C.cpp 

INO_SRCS += \
../HardwareVerify/HardwareVerify.ino 

CPP_DEPS += \
./HardwareVerify/PromAddressDriver.d \
./HardwareVerify/PromDevice.d \
./HardwareVerify/PromDevice28C.d 

INO_DEPS += \
./HardwareVerify/HardwareVerify.d 

OBJS += \
./HardwareVerify/HardwareVerify.o \
./HardwareVerify/PromAddressDriver.o \
./HardwareVerify/PromDevice.o \
./HardwareVerify/PromDevice28C.o 


# Each subdirectory must supply rules for building sources it contributes
HardwareVerify/%.o: ../HardwareVerify/%.ino HardwareVerify/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

HardwareVerify/%.o: ../HardwareVerify/%.cpp HardwareVerify/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-HardwareVerify

clean-HardwareVerify:
	-$(RM) ./HardwareVerify/HardwareVerify.d ./HardwareVerify/HardwareVerify.o ./HardwareVerify/PromAddressDriver.d ./HardwareVerify/PromAddressDriver.o ./HardwareVerify/PromDevice.d ./HardwareVerify/PromDevice.o ./HardwareVerify/PromDevice28C.d ./HardwareVerify/PromDevice28C.o

.PHONY: clean-HardwareVerify

