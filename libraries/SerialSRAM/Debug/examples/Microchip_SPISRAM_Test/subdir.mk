################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../examples/Microchip_SPISRAM_Test/Microchip_SPISRAM_Test.ino 

OBJS += \
./examples/Microchip_SPISRAM_Test/Microchip_SPISRAM_Test.o 

INO_DEPS += \
./examples/Microchip_SPISRAM_Test/Microchip_SPISRAM_Test.d 


# Each subdirectory must supply rules for building sources it contributes
examples/Microchip_SPISRAM_Test/%.o: ../examples/Microchip_SPISRAM_Test/%.ino
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


