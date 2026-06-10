################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../TommyPROM/CmdStatus.cpp \
../TommyPROM/PromAddressDriver.cpp \
../TommyPROM/PromDevice.cpp \
../TommyPROM/PromDevice23.cpp \
../TommyPROM/PromDevice27.cpp \
../TommyPROM/PromDevice28C.cpp \
../TommyPROM/PromDevice8755A.cpp \
../TommyPROM/PromDeviceSST28SF.cpp \
../TommyPROM/PromDeviceSST39SF.cpp \
../TommyPROM/XModem.cpp 

INO_SRCS += \
../TommyPROM/TommyPROM.ino 

CPP_DEPS += \
./TommyPROM/CmdStatus.d \
./TommyPROM/PromAddressDriver.d \
./TommyPROM/PromDevice.d \
./TommyPROM/PromDevice23.d \
./TommyPROM/PromDevice27.d \
./TommyPROM/PromDevice28C.d \
./TommyPROM/PromDevice8755A.d \
./TommyPROM/PromDeviceSST28SF.d \
./TommyPROM/PromDeviceSST39SF.d \
./TommyPROM/XModem.d 

INO_DEPS += \
./TommyPROM/TommyPROM.d 

OBJS += \
./TommyPROM/CmdStatus.o \
./TommyPROM/PromAddressDriver.o \
./TommyPROM/PromDevice.o \
./TommyPROM/PromDevice23.o \
./TommyPROM/PromDevice27.o \
./TommyPROM/PromDevice28C.o \
./TommyPROM/PromDevice8755A.o \
./TommyPROM/PromDeviceSST28SF.o \
./TommyPROM/PromDeviceSST39SF.o \
./TommyPROM/TommyPROM.o \
./TommyPROM/XModem.o 


# Each subdirectory must supply rules for building sources it contributes
TommyPROM/%.o: ../TommyPROM/%.cpp TommyPROM/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

TommyPROM/%.o: ../TommyPROM/%.ino TommyPROM/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-TommyPROM

clean-TommyPROM:
	-$(RM) ./TommyPROM/CmdStatus.d ./TommyPROM/CmdStatus.o ./TommyPROM/PromAddressDriver.d ./TommyPROM/PromAddressDriver.o ./TommyPROM/PromDevice.d ./TommyPROM/PromDevice.o ./TommyPROM/PromDevice23.d ./TommyPROM/PromDevice23.o ./TommyPROM/PromDevice27.d ./TommyPROM/PromDevice27.o ./TommyPROM/PromDevice28C.d ./TommyPROM/PromDevice28C.o ./TommyPROM/PromDevice8755A.d ./TommyPROM/PromDevice8755A.o ./TommyPROM/PromDeviceSST28SF.d ./TommyPROM/PromDeviceSST28SF.o ./TommyPROM/PromDeviceSST39SF.d ./TommyPROM/PromDeviceSST39SF.o ./TommyPROM/TommyPROM.d ./TommyPROM/TommyPROM.o ./TommyPROM/XModem.d ./TommyPROM/XModem.o

.PHONY: clean-TommyPROM

