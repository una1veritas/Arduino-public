################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
O_SRCS += \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/FAT.o \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/SCCB.o \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/delay.o \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/int.o \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/lcd.o \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/main.o \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/mmc_sd.o \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/ov7660config.o \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/ov7670.o 

C_SRCS += \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/FAT.c \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/SCCB.c \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/delay.c \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/int.c \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/lcd.c \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/main.c \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/mmc_sd.c \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/ov7670.c \
../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/ov7670config.c 

OBJS += \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/FAT.o \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/SCCB.o \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/delay.o \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/int.o \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/lcd.o \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/main.o \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/mmc_sd.o \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/ov7670.o \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/ov7670config.o 

C_DEPS += \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/FAT.d \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/SCCB.d \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/delay.d \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/int.d \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/lcd.d \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/main.d \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/mmc_sd.d \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/ov7670.d \
./ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/ov7670config.d 


# Each subdirectory must supply rules for building sources it contributes
ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/%.o: ../ATmega32_tft_ov7670_REV4.2/ATmega32_TFT_ov7670/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	avrgcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


