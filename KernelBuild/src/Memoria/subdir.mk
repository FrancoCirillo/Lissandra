################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Memoria/memoria.c 

OBJS += \
./src/Memoria/memoria.o 

C_DEPS += \
./src/Memoria/memoria.d 


# Each subdirectory must supply rules for building sources it contributes
src/Memoria/%.o: ../src/Memoria/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -I/usr/include/commons -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


