################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../main.c \
../NVM_driver.c \
../protocol_avr911.c \
../RC_Calib.c \
../TPI_driver.c \
../usart_driver.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
main.o \
NVM_driver.o \
protocol_avr911.o \
RC_Calib.o \
TPI_driver.o \
usart_driver.o


OBJS_AS_ARGS +=  \
main.o \
NVM_driver.o \
protocol_avr911.o \
RC_Calib.o \
TPI_driver.o \
usart_driver.o


C_DEPS +=  \
main.d \
NVM_driver.d \
protocol_avr911.d \
RC_Calib.d \
TPI_driver.d \
usart_driver.d


C_DEPS_AS_ARGS +=  \
main.d \
NVM_driver.d \
protocol_avr911.d \
RC_Calib.d \
TPI_driver.d \
usart_driver.d


OUTPUT_FILE_PATH +=AVR918.elf

OUTPUT_FILE_PATH_AS_ARGS +=AVR918.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

# AVR32/GNU C Compiler













./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : (AVR_8_bit_GNU_Toolchain_3.4.0_653) 4.6.2
	$(QUOTE)C:\avr8-gnu-toolchain-win32_x86\bin\avr-gcc.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DF_CPU=11059200UL -DATTINY10  -O1 -fpack-struct -fshort-enums -g2 -Wall -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"  -mmcu=atmega324a   -o"$@" "$<" 
	@echo Finished building: $<



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : (AVR_8_bit_GNU_Toolchain_3.4.0_653) 4.6.2
	$(QUOTE)C:\avr8-gnu-toolchain-win32_x86\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="AVR918.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group  -mmcu=atmega324a  
	@echo Finished building target: $@
	"C:\avr8-gnu-toolchain-win32_x86\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature  "AVR918.elf" "AVR918.hex"
	"C:\avr8-gnu-toolchain-win32_x86\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "AVR918.elf" "AVR918.eep" || exit 0
	"C:\avr8-gnu-toolchain-win32_x86\bin\avr-objdump.exe" -h -S "AVR918.elf" > "AVR918.lss"
	"C:\avr8-gnu-toolchain-win32_x86\bin\avr-size.exe" "AVR918.elf"
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS)$(C_DEPS_AS_ARGS) $(EXECUTABLES) 
	rm -rf "AVR918.elf" "AVR918.a" "AVR918.hex" "AVR918.lss" "AVR918.eep" "AVR918.map"
	