################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include z80asm/subdir.mk
-include libraries/LiquidCrystal/src/subdir.mk
-include libraries/LiquidCrystal/examples/setCursor/subdir.mk
-include libraries/LiquidCrystal/examples/TextDirection/subdir.mk
-include libraries/LiquidCrystal/examples/SerialDisplay/subdir.mk
-include libraries/LiquidCrystal/examples/Scroll/subdir.mk
-include libraries/LiquidCrystal/examples/HelloWorld/subdir.mk
-include libraries/LiquidCrystal/examples/Display/subdir.mk
-include libraries/LiquidCrystal/examples/CustomCharacter/subdir.mk
-include libraries/LiquidCrystal/examples/Cursor/subdir.mk
-include libraries/LiquidCrystal/examples/Blink/subdir.mk
-include libraries/LiquidCrystal/examples/Autoscroll/subdir.mk
-include cpmdisk/subdir.mk
-include core/core/subdir.mk
-include subdir.mk
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Z80_System
BUILD_ARTIFACT_EXTENSION :=
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: main-build

# Main-build Target
main-build: Z80_System

# Tool invocations
Z80_System: $(OBJS) $(USER_OBJS) makefile $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross G++ Linker'
	avr-g++  -o "Z80_System" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Z80_System
	-@echo ' '

.PHONY: all clean dependents main-build

-include ../makefile.targets
