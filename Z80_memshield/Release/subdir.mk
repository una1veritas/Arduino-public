################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
INO_SRCS += \
../Z80_memshield.ino 

CPP_SRCS += \
../OC1AClock.cpp \
../Z80.cpp \
../mem.cpp 

LINK_OBJ += \
./OC1AClock.cpp.o \
./Z80.cpp.o \
./mem.cpp.o 

INO_DEPS += \
./Z80_memshield.ino.d 

CPP_DEPS += \
./OC1AClock.cpp.d \
./Z80.cpp.d \
./mem.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
OC1AClock.cpp.o: ../OC1AClock.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  "$@"
	@echo 'Finished building: $<'
	@echo ' '

Z80.cpp.o: ../Z80.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  "$@"
	@echo 'Finished building: $<'
	@echo ' '

Z80_memshield.o: ../Z80_memshield.ino
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  "$@"
	@echo 'Finished building: $<'
	@echo ' '

mem.cpp.o: ../mem.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  "$@"
	@echo 'Finished building: $<'
	@echo ' '


