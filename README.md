# **Intel 8086 Decompiler/Simulator** 
## **What Is This?**
This is a decompiler and simulator of the Intel 8086 processor which ingests binary instruction sets and outputs the corresponding assembly code and a printout of register and flag states. 

Sample binary files are provided for use with the decompiler/simulator, and are from Casey Muratori's [performance aware programming course](https://github.com/cmuratori/computer_enhance/tree/main/perfaware/part1). 

Please see the constraints section for the list of instructions that can currently be processed. 

For detailed information on the 8086 processor, visit this [handbook](https://edge.edx.org/c4x/BITSPilani/EEE231/asset/8086_family_Users_Manual_1_.pdf). 

## **Constraints**
The decompiler and simulator can process *only* the following instructions: 
- mov
- add
- cmp
- conditional jump instructions: 
    - je
    - jl
    - jle
    - jb
    - jbe 
    - jp
    - jo
    - js
    - jne
    - jnl
    - jg 
    - jnb
    - ja
    - jnp
    - jno
    - jns
    - loop
    - loopz
    - loopnz
    - jcxz

## **System Requirements** 
- g++ must be installed

## **How To Run the Decompiler/Simulator**
The decompiler/simulator can be run from the command line. Append the name of the binary file you wish to decompile & simulate:
````bash
./run.sh {filename}
````