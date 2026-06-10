################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../unlock-ben-eater-hardware/unlock-ben-eater-hardware.ino 

INO_DEPS += \
./unlock-ben-eater-hardware/unlock-ben-eater-hardware.d 

OBJS += \
./unlock-ben-eater-hardware/unlock-ben-eater-hardware.o 


# Each subdirectory must supply rules for building sources it contributes
unlock-ben-eater-hardware/%.o: ../unlock-ben-eater-hardware/%.ino unlock-ben-eater-hardware/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	avr-g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-unlock-2d-ben-2d-eater-2d-hardware

clean-unlock-2d-ben-2d-eater-2d-hardware:
	-$(RM) ./unlock-ben-eater-hardware/unlock-ben-eater-hardware.d ./unlock-ben-eater-hardware/unlock-ben-eater-hardware.o

.PHONY: clean-unlock-2d-ben-2d-eater-2d-hardware

