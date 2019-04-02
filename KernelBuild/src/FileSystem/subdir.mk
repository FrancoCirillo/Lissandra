################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/FileSystem/fileSystem.c 

OBJS += \
./src/FileSystem/fileSystem.o 

C_DEPS += \
./src/FileSystem/fileSystem.d 


# Each subdirectory must supply rules for building sources it contributes
src/FileSystem/%.o: ../src/FileSystem/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/include/commons -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


