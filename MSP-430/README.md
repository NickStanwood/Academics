# MSP-430
Emulation of the TI MSP-430 microprocessor

This emulator showcases the inner workings of the MSP-430. it works very similiar to the actual
machine, as it takes a S-record file created from the cross_assembler.exe and loads it into the 
memory of the machine. After this it will begin the fetch, decode and execute loop in which it fetches 
data from memory, decodes the instruction embedded in it, and executes the given instruction. The 
Program can be run with a self made debugger to see what is happening as the code progresses.

the main files are as follows:

    mainline.c
    
      - loads the .s19 file into the emulated memory.
      - initiates the fetch , decode and execute loop.
      
    CPU.c
    
      - utilizes a bus to retrieve data from emulated memory. 
      - uses the Memory Address Bus to set a value in memory to fetch from or write to.
      - uses the Memory Data Bus to set values in the register file.
      - instruction errors are handled here
      
    Decoder.c
    
      - decodes the data retrieved from memory to find the embedded instruction
      
    ALU.c
    
      - uses a function pointer in order to execute the proper intruction
      - updates the Status Register depending on the result of the instruction
      
    memory.c
    
      - 64Kib of emulated memory to store the data, code and stack as well as interrupt vectors
        and device status registers




