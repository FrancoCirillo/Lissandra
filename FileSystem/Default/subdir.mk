################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Estructuras.c \
../compactador.c \
../fileSystem.c \
../memtable.c 

OBJS += \
./Estructuras.o \
./compactador.o \
./fileSystem.o \
./memtable.o 

C_DEPS += \
./Estructuras.d \
./compactador.d \
./fileSystem.d \
./memtable.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O2 -g -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


