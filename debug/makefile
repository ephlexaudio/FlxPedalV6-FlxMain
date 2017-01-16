################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
ARCHIVES += \
libUSER_OBJS.a \


# All Target
all: OfxMain

# Tool invocations
OfxMain: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Buildroot ARM CC Linker (/home/buildrooteclipse/buildroot-2016.08/output)'
	/home/buildrooteclipse/buildroot-2016.08/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-g++ -L/home/buildrooteclipse/buildroot-2016.08/output/host/usr/arm-buildroot-linux-uclibcgnueabihf/sysroot/usr/lib -o "OfxMain" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

libUSER_OBJS.a: $(OBJS) $(USER_OBJS)
	@echo 'Invoking: Buildroot ARM Archiver (/home/buildrooteclipse/buildroot-2016.08/output)'
	/home/buildrooteclipse/buildroot-2016.08/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-ar -r  "libUSER_OBJS.a" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C++_DEPS)$(OBJS)$(C_DEPS)$(CC_DEPS)$(ARCHIVES)$(CPP_DEPS)$(EXECUTABLES)$(CXX_DEPS)$(C_UPPER_DEPS) OfxMain
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
