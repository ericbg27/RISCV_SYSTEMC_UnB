#ifndef RISCV_H
#define RISCV_H

#include <iostream>
#include "systemc.h"

using namespace std;

//Verificar utilidade desses defines
#define get_byte_0(w) (w & 0xFF)
#define get_byte_1(w) ((w>>8) & 0xFF)
#define get_byte_2(w) ((w>>16) & 0xFF)
#define get_byte_3(w) ((w>>24) & 0xFF)
//
// definicao dos valores do campo de extensao
//

SC_MODULE(RiscV) {
	//****************************************************************
	// Definicao dos valores do campo de funct3 do RISCV
	//****************************************************************
	enum FUNCT3 {
		ADD_MUL_ADDI = 0x00,
		SLL = 0x01,
		SLT = 0x02,
		XOR_DIV = 0x04,
		SRL = 0x05,
		OR_REM = 0x06,
		AND = 0x07,
		SLTU = 0x03
	};
	//****************************************************************
	// Definicao dos valores do campo de funct7 do RISCV
	//****************************************************************
	enum FUNCT7 {
		ZERO7 = 0x00, SUB7 = 0x20, MUL_DIV = 0x01,
	};
	//****************************************************************
	// Definicao dos valores do campo de codigo da operacao do RISCV
	//****************************************************************
	enum OPCODES {
		OPC_R = 0x33,
		LW = 0x03,
		LUI = 0x37,
		SW = 0x23,
		BRANCH = 0x63,
		TIPOI = 0x13,
		AUIPC = 0x17,
		JALR = 0x67,
		JAL = 0x6F
	//ECALL=0x73,
	};
	//******************************************************
	// Identificacao dos registradores do banco do RISCV
	//******************************************************
	enum REGISTERS {
		ZERO = 0,
		RA = 1,
		SP = 2,
		GP = 3,
		TP = 4,
		T0 = 5,
		T1 = 6,
		T2 = 7,
		S0 = 8,
		S1 = 9,
		A0 = 10,
		A1 = 11,
		A2 = 12,
		A3 = 13,
		A4 = 14,
		A5 = 15,
		S8 = 24,
		S9 = 25,
		A6 = 16,
		A7 = 17,
		S2 = 18,
		S3 = 19,
		S4 = 20,
		S5 = 21,
		S6 = 22,
		S7 = 23,
		S10 = 26,
		S11 = 27,
		T3 = 28,
		T4 = 29,
		T5 = 30,
		T6 = 31
	};
	//**************************************************************
	// Identificacao de tipo de dado (não sei se será necessário)
	// - Verificar utilidade
	//**************************************************************
	enum DATA_TYPE {
		BYTE = 4, INTEGER = 1
	};
	//**************************************************************
	// Banco de registradores do MIPS
	//**************************************************************
	int32_t breg[32] = { 0 };
	//**************************************************************
	// registradores especiais
	//**************************************************************
	int32_t PC,						// contador de programa
			ri;						// registrador de intrucao
	//*****************************************************************************
	// campos das instrucoes RISC-V32
	//  Formato R:  |    Funct7   | rs2 | rs1 | Funct3 |     rd   | OPCODE |
	//  Formato I:  |  IMM[11:0]  | rs1 | Funct3 | rd  | OPCODE |
	//  Formato S:  |  IMM[11:5]  | rs2 | rs1 | Funct3 | IMM[4:0] | OPCODE |
	//  Formato SB: | IMM[12|10:5]| rs2 | rs1 | Funct3 | IMM[4:1|11] | OPCODE |
	//  Formato U:  | IMM[31:12]  | rd  | OPCODE |
	//  Formato UJ: | IMM[20|10:1|11|19:12]   |   rd   | OPCODE |
	//*****************************************************************************
	int32_t opcode,					// codigo da operacao
			rs1,						// indice registrador rs
			rs2,						// indice registrador rt
			rd,						// indice registrador rd
			funct3,					// campo auxiliar
			funct7, kte12,					// constante instrucao tipo I
			kte20,					// constante instrucao tipo UJ
			kte5,                  // constante instrução tipos S e SB
			kte7, Imm;                   // constante instrução tipos S e SB

	//****************************************************************************
	// Entradas, saídas e outras variáveis
	//****************************************************************************
	sc_fifo_in<int32_t> P_in_Data; //Entrada da cache de dados
	sc_fifo_in<int32_t> P_in_Inst; //Entrada da cache de instruções
	sc_fifo_out<int32_t> P_out_Data; //Saída de endereços para cache de dados
	sc_fifo_out<int32_t> P_out_Inst; //Saída de endereços para cache de instruções
	sc_out<int32_t> Data; //Dados
	sc_out<bool> Write_Signal; //Sinal de ativação da escrita
	sc_event execute_event;
	sc_event fetch_event;
	sc_event decode_event;
	sc_event init_event;
	sc_in<bool> ready_signal;

	//****************************************************************************
	// Funcoes definidas em RiscV.cpp
	//****************************************************************************
	void init();
	void fetch();
	void decode();
	void execute();
	void debug_decode();
	void dump_breg();

	SC_CTOR(RiscV) {
		SC_THREAD(init);
		SC_THREAD(fetch);
		sensitive << execute_event << init_event;
		SC_THREAD(decode);
		sensitive << fetch_event;
		SC_THREAD(execute);
		sensitive << decode_event;
	}
	;
};

#endif
