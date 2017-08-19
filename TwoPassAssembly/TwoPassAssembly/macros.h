#pragma once
#include <iostream>

#define PRINT(x) std::cout<<x<<std::endl

#define REGDIR 1
#define REGIND 2
#define REGINDPOM 4
#define IMMED 8
#define MEMDIR 16

#define ADDR_IMMED 0b100
#define ADDR_REGDIR 0b000
#define ADDR_MEMDIR 0b110
#define ADDR_REGIND 0b010
#define ADDR_REGINDPOM 0b111

#define INSTRUCTION 1
#define OP1 2
#define OP2 4

#define JMP_INST 0
#define LOAD_STORE_INST 1
#define STACK_INST 2
#define ALU_INST 3

#define DD 0b000
#define UW 0b001
#define SW 0b101
#define UB 0b011
#define SB 0b111