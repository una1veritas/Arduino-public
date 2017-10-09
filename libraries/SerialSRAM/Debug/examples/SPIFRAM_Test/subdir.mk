################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../examples/SPIFRAM_Test/SPIFRAM_Test.ino 

OBJS += \
./examples/SPIFRAM_Test/SPIFRAM_Test.o 

INO_DEPS += \
./examples/SPIFRAM_Test/SPIFRAM_Test.d 


# Each subdirectory must supply rules for building sources it contributes
examples/SPIFRAM_Test/%.o: ../examples/SPIFRAM_Test/%.ino
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


