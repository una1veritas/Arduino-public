################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
ASM_SRCS += \
../z80asm/division.asm \
../z80asm/example.asm \
../z80asm/example1.asm \
../z80asm/example2.asm \
../z80asm/example3.asm \
../z80asm/hex2nibble.asm \
../z80asm/mon_v00.asm \
../z80asm/mon_v01.asm \
../z80asm/mon_v02.asm \
../z80asm/print_hl_dec_2_unfin.asm \
../z80asm/wozmon80.asm 

LINK_OBJ += \
./z80asm/division.asm.o \
./z80asm/example.asm.o \
./z80asm/example1.asm.o \
./z80asm/example2.asm.o \
./z80asm/example3.asm.o \
./z80asm/hex2nibble.asm.o \
./z80asm/mon_v00.asm.o \
./z80asm/mon_v01.asm.o \
./z80asm/mon_v02.asm.o \
./z80asm/print_hl_dec_2_unfin.asm.o \
./z80asm/wozmon80.asm.o 

ASM_DEPS += \
./z80asm/division.asm.d \
./z80asm/example.asm.d \
./z80asm/example1.asm.d \
./z80asm/example2.asm.d \
./z80asm/example3.asm.d \
./z80asm/hex2nibble.asm.d \
./z80asm/mon_v00.asm.d \
./z80asm/mon_v01.asm.d \
./z80asm/mon_v02.asm.d \
./z80asm/print_hl_dec_2_unfin.asm.d \
./z80asm/wozmon80.asm.d 


# Each subdirectory must supply rules for building sources it contributes
z80asm/division.asm.o: ../z80asm/division.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/example.asm.o: ../z80asm/example.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/example1.asm.o: ../z80asm/example1.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/example2.asm.o: ../z80asm/example2.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/example3.asm.o: ../z80asm/example3.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/hex2nibble.asm.o: ../z80asm/hex2nibble.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/mon_v00.asm.o: ../z80asm/mon_v00.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/mon_v01.asm.o: ../z80asm/mon_v01.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/mon_v02.asm.o: ../z80asm/mon_v02.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/print_hl_dec_2_unfin.asm.o: ../z80asm/print_hl_dec_2_unfin.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '

z80asm/wozmon80.asm.o: ../z80asm/wozmon80.asm
	@echo 'Building file: $<'
	@echo 'Starting S compile'
	"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -c -g -x assembler-with-cpp -flto -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10812 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR   -I"/Users/sin/eclipse/cpp-2025-03/Eclipse.app/Contents/Eclipse/arduinoPlugin/libraries/LiquidCrystal/1.0.7/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<" -o "$@"
	@echo 'Finished building: $<'
	@echo ' '


