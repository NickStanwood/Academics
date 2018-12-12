################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Semaphore.obj: ../Semaphore.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="Semaphore.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

Systick.obj: ../Systick.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="Systick.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

UART.obj: ../UART.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="UART.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

command_line.obj: ../command_line.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="command_line.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

debugger.obj: ../debugger.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="debugger.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gpioe.obj: ../gpioe.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="gpioe.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

gpiof.obj: ../gpiof.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="gpiof.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

intr_handlers.obj: ../intr_handlers.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="intr_handlers.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernel.obj: ../kernel.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="kernel.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

kernelcalls.obj: ../kernelcalls.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="kernelcalls.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

lm3s9d92_startup_ccs.obj: ../lm3s9d92_startup_ccs.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="lm3s9d92_startup_ccs.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

main.obj: ../main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

mem_manager.obj: ../mem_manager.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="mem_manager.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

msg_queue.obj: ../msg_queue.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="msg_queue.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

processcalls.obj: ../processcalls.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="processcalls.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

spi.obj: ../spi.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="spi.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sysctl.obj: ../sysctl.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="sysctl.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

train_control.obj: ../train_control.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="train_control.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

train_display.obj: ../train_display.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="train_display.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

train_layout.obj: ../train_layout.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"E:/TI/ccsv5/tools/compiler/arm_5.1.1/bin/armcl" -mv7M3 --code_state=16 --abi=eabi -me -O2 --include_path="E:/TI/ccsv5/tools/compiler/arm_5.1.1/include" --diag_warning=225 --display_error_number --diag_wrap=off --preproc_with_compile --preproc_dependency="train_layout.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


