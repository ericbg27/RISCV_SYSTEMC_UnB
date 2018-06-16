#ifndef RISCVc___globals_h
#define RISCVc___globals_h
#include <cstdint>
#include <iostream>

int32_t breg[32];

int32_t pc,						// contador de programa
        ri;						// registrador de intrucao

int32_t	opcode,					// codigo da operacao
		rs1,						// indice registrador rs
		rs2,						// indice registrador rt
		rd,						// indice registrador rd
		funct3,					// campo auxiliar
		funct7,
		kte12,					// constante instrucao tipo I
		kte20,					// constante instrucao tipo UJ
		kte5,                  // constante instrução tipos S e SB
        kte7,
        Imm;
#endif
