<<<<<<< HEAD
#ifndef RISCVc___globals_h
#define RISCVc___globals_h
=======
//
//  globals.h
//  MIPSc++
//
//  Created by Ricardo Jacobi on 18/4/15.
//
//

#ifndef MIPSc___globals_h
#define MIPSc___globals_h
>>>>>>> 697879eb57d476622f76be735749f0a2b6adc775
#include <cstdint>
#include <iostream>

int32_t breg[32];

int32_t pc,						// contador de programa
        ri,						// registrador de intrucao
<<<<<<< HEAD

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
=======
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

>>>>>>> 697879eb57d476622f76be735749f0a2b6adc775
#endif
