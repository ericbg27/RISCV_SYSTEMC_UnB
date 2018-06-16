/*
 *  mips.h
 *  MIPS
 *
 *  Created by Ricardo Jacobi on 18/04/11.
 *  Copyright 2011 Universidade de Brasilia. All rights reserved.
 *
 */

#ifndef MIPS_H
#define MIPS_H

#include <iostream>
using namespace std;

#define MEM_SIZE 20000
#define get_byte_0(w) (w & 0xFF)
#define get_byte_1(w) ((w>>8) & 0xFF)
#define get_byte_2(w) ((w>>16) & 0xFF)
#define get_byte_3(w) ((w>>24) & 0xFF)

//
// definicao dos valores do campo de extensao
//
enum FUNCT3 {
	ADD_MUL_ADDI=0x00, SLL=0x01,
	SLT=0x02, XOR_DIV=0x04,
	SRL=0x05, OR_REM=0x06,
	AND=0x07,
	ECALL=0x0c,
};

enum FUNCT7 {
    ZERO7=0x00,
    SUB7=0x20,
    MUL_DIV=0x01,
}
//
// definicao dos valores do campo de codigo da operacao do MIPS
//
enum OPCODES {
	OPC_R=0x33,	    LW=0x03,
	LUI=0x37,
	SW=0x23,	    BRANCH=0x63,
	TIPOI=0x13,     AUIPC=0x17,
	JALR=67,		JAL=0x6F

};

//
// identificacao dos registradores do banco do MIPS
//
enum REGISTERS {
	ZERO=0, RA=1,	SP=2,	GP=3,   // essa linha ja foi
	A0=4,	A1=5,	A2=6,	A3=7,
	T0=8,	T1=9,	T2=10,	T3=11,
	T4=12,	T5=13,	T6=14,	T7=15,
	T8=24,	T9=25,	S0=16,	S1=17,
	S2=18,	S3=19,	S4=20,	S5=21,
	S6=22,	S7=23,	K0=26,	K1=27,
	GP=28,	SP=29,	FP=30,	RA=31
};

//
// Identificacao de tipo de dado
//
enum DATA_TYPE {
	BYTE=4, INTEGER=1
};

//
// banco de registradores do MIPS
//
extern int32_t breg[32];

//
// registradores especiais
//
extern
int32_t pc,						// contador de programa
		ri;						// registrador de intrucao

//
// memoria ligada ao processador
//
extern int32_t mem[MEM_SIZE];
extern int32_t int_mem[MEM_SIZE];

//
// campos das instrucoes RISC-V32
//  Formato R:  |    Funct7   | rs2 | rs1 | Funct3 |     rd   | OPCODE |
//  Formato I:  |      IMM[11:0]    | rs1 | Funct3 |     rd   | OPCODE |
//  Formato S:  | IMM[12|10:5]| rs2 | rs1 | Funct3 | imm[4:0] | OPCODE |
//  Formato J:

extern
int32_t	opcode,					// codigo da operacao
		rs1,						// indice registrador rs
		rs2,						// indice registrador rt
		rd,						// indice registrador rd
		funct3,					// campo auxiliar
		funct7,
		kte12,					// constante instrucao tipo I
		kte20;					// constante instrucao tipo UJ
		kte5;                   // constante instrução tipos S e SB
        kte7;                   // constante instrução tipos S e SB

//
// Funcoes definidas em mips.cpp
//

void init();
void fetch ();
void debug_decode();
void dump_breg();
void dump_mem(int start, int end, char format);
int load_mem(const char *fn, int start);
void execute ();
void step();
void run();

#endif
