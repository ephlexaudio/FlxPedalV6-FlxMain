################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../BaseUiInt.cpp \
../ComboDataInt.cpp \
../DataFuncts.cpp \
../Effects2.cpp \
../GPIOClass.cpp \
../HostUiInt.cpp \
../PedalUiInt.cpp \
../Processing.cpp \
../ProcessingControl.cpp \
../SharedMemoryInt.cpp \
../UsbInt.cpp \
../jackaudioio.cpp \
../main.cpp \
../mainFunctions.cpp \
../utilityFunctions.cpp 

OBJS += \
./BaseUiInt.o \
./ComboDataInt.o \
./DataFuncts.o \
./Effects2.o \
./GPIOClass.o \
./HostUiInt.o \
./PedalUiInt.o \
./Processing.o \
./ProcessingControl.o \
./SharedMemoryInt.o \
./UsbInt.o \
./jackaudioio.o \
./main.o \
./mainFunctions.o \
./utilityFunctions.o 

CPP_DEPS += \
./BaseUiInt.d \
./ComboDataInt.d \
./DataFuncts.d \
./Effects2.d \
./GPIOClass.d \
./HostUiInt.d \
./PedalUiInt.d \
./Processing.d \
./ProcessingControl.d \
./SharedMemoryInt.d \
./UsbInt.d \
./jackaudioio.d \
./main.d \
./mainFunctions.d \
./utilityFunctions.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Buildroot ARM C++ Compiler (/home/buildrooteclipse/buildroot-2016.08/output)'
	/home/buildrooteclipse/buildroot-2016.08/output/host/usr/bin/arm-buildroot-linux-uclibcgnueabihf-g++ -std=c++0x -O1 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


