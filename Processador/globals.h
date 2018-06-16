//
//  globals.h
//  MIPSc++
//
//  Created by Ricardo Jacobi on 18/4/15.
//
//

#ifndef MIPSc___globals_h
#define MIPSc___globals_h
#include <cstdint>
#include <iostream>

int32_t breg[32];

int32_t pc,						// contador de programa
        ri,						// registrador de intrucao
        hi,						// 32 bits mais significativos da multiplicacao
        lo;						// 32 bits menos significativos da multiplicacao

int32_t	opcode,					// codigo da operacao
        rs,						// indice registrador rs
        rt,						// indice registrador rt
        rd,						// indice registrador rd
        shamt,					// deslocamento
        funct,					// campo auxiliar
        kte16,					// constante instrucao tipo I
        kte26;					// constante instrucao tipo J

#endif
