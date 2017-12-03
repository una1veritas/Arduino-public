################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../Sort_Algorithms.ino 

CPP_SRCS += \
../sortalgorithms.cpp 

LINK_OBJ += \
./sortalgorithms.cpp.o 

INO_DEPS += \
./Sort_Algorithms.ino.d 

CPP_DEPS += \
./sortalgorithms.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
Sort_Algorithms.o: ../Sort_Algorithms.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  "$@"
	@echo 'Finished building: $<'
	@echo ' '

sortalgorithms.cpp.o: ../sortalgorithms.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  "$@"
	@echo 'Finished building: $<'
	@echo ' '


