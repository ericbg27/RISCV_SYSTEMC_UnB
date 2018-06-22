#include "RiscV.h"

void RiscV::init() {
	PC = ri = 0x0000; //Endereço inicial da memória de instruçoes
	init_event.notify(SC_ZERO_TIME);
}
/******************************************************************************
fetch()
- Busca instrucao na memoria e escreve no registrador de instrucoes ri
- Incrementa pc
******************************************************************************/
void RiscV::fetch() {
    wait(init_event);
    while(true) {
        //if(PC != 8) {
        P_out_Inst.write(PC);
        wait(SC_ZERO_TIME);
        ri = P_in_Inst.read();
        //}
        PC = PC + 4;
        fetch_event.notify(SC_ZERO_TIME);
        wait(execute_event);
	}
}

/*******************************************************************************
decode()
- Decodifica a instrução, atribuindo os valores aos campos
- Além disso, calcula o imediato das instruções que o possuem de acordo com o
formato que ele está na instrução
*******************************************************************************/
void RiscV::decode() {
    while(true) {
        wait(fetch_event);
        opcode	= ri & 0x0000007F;
        if(opcode == OPC_R) {
            rd = (ri >> 7) & 0x1F;
            funct3 = (ri >> 12) & 0x07;
            rs1 = (ri >> 15) & 0x1F;
            rs2 = (ri >> 20) & 0x1F;
            funct7 = (ri >> 25) & 0x7F;
        } else if(opcode == TIPOI || opcode == LW) {
            rd = (ri >> 7) & 0x1F;
            funct3 = (ri >> 12) & 0x07;
            rs1 = (ri >> 15) & 0x1F;
            kte12 = (ri >> 20) & 0xFFF;
            //***************Cálculo do imediato********************
            Imm = kte12;
            //******************************************************
        } else if(opcode == BRANCH) {
            kte5 = (ri >> 7) & 0x1F;
            funct3 = (ri >> 12) & 0x07;
            rs1 = (ri >> 15) & 0x1F;
            rs2 = (ri >> 20) & 0x1F;
            kte7 = (ri >> 25) & 0x3F;
            //***************Cálculo do imediato********************
            Imm = kte5 & 0x01;
            Imm = Imm << 10;
            Imm += (kte5 >> 1) & 0x0F;
            Imm += (kte7 & 0x3F) << 4;
            Imm += ((kte7 >> 6) & 0x01) << 11;
            //******************************************************
        } else if(opcode == AUIPC || opcode == LUI) {
            rd = (ri >> 7) & 0x1F;
            kte20 = (ri >> 12) & 0xFFFFF;
            Imm = kte20;
        } else if(opcode == SW) {
            kte5 = (ri >> 7) & 0x1F;
            funct3 = (ri >> 12) & 0x07;
            rs1 = (ri >> 15) & 0x1F;
            rs2 = (ri >> 20) & 0x1F;
            kte7 = (ri >> 25) & 0x3F;
            //***************Cálculo do imediato********************
            Imm = (kte7 << 5) + kte5;
            //******************************************************
        } else if(opcode == JAL) {
            rd = (ri >> 7) & 0x1F;
            kte20 = (ri >> 12) & 0xFFFFF;
            //***************Cálculo do imediato********************
            Imm = (kte20 >> 9) & 0x3FF;
            Imm += ((kte20 >> 8) & 0x01) << 10;
            Imm += (kte20 & 0xFF) << 11;
            Imm += ((kte20 >> 19) & 0x01) << 19;
            //******************************************************
        }
        decode_event.notify(SC_ZERO_TIME);
        wait(SC_ZERO_TIME);
	}
}


void RiscV::debug_decode() {
	cout << "PC = " << PC << endl;
	cout << "opcode = " << opcode << " rs1 = " << rs1 << " rs2 = " << rs2 << " rd = " << rd << " funct7 = " << funct7  << " funct3 = " << funct3 << endl;
}

void RiscV::dump_breg() {
	for (int i=0; i<32; i++) {
		printf("BREG[%2d] = \t%8d \t\t\t%8x\n", i, breg[i], breg[i]);
	}
}

void RiscV::execute() {
/**********************************************************************************************
As operações de shift logico estão com typecast para unsigned visando evitar a extensão de sinal
uma vez que esta só acontece no shift aritmético
***********************************************************************************************/
	while(true) {
        wait(decode_event);
        breg[0] = 0;
        switch (opcode) {
            case OPC_R:
                switch(funct7) {
                    case ZERO7:
                        switch(funct3) {
                            case ADD_MUL_ADDI:
                                breg[rd] = breg[rs1] + breg[rs2];
                                break;
                            case SLL:
                                breg[rd] = ((uint32_t)breg[rs1]) << breg[rs2];
                                break;
                            case SLT:
                                breg[rd] = (breg[rs1]<breg[rs2])?1:0;
                                break;
                            case SLTU:
                                breg[rd] = ((uint32_t)breg[rs1]<(uint32_t)breg[rs2])?1:0;
                                break;
                            case XOR_DIV:
                                breg[rd] = breg[rs1] ^ breg[rs2];
                                break;
                            case SRL:
                                breg[rd] = ((uint32_t)breg[rs1]) >> breg[rs2];
                                break;
                            case OR_REM:
                                breg[rd] = breg[rs1] | breg[rs2];
                                break;
                            case AND:
                                breg[rd] = breg[rs1] & breg[rs2];
                                break;
                        }
                    case SUB7:
                        switch(funct3) {
                            case ADD_MUL_ADDI: //SUB
                                breg[rd] = breg[rs1] - breg[rs2];
                                break;
                            case SRL: //SRA
                                breg[rd] = breg[rs1] >> breg[rs2];
                                break;
                        }
                }
            case TIPOI:
                switch(funct3) {
                    case ADD_MUL_ADDI: //ADDI
                        breg[rd] = breg[rs1] + Imm;
                        break;
                    case SLT: //SLTI
                        breg[rd] = (breg[rs1]<Imm)?1:0;
                        break;
                    case XOR_DIV: //XORI
                        breg[rd] = breg[rs1] ^ Imm;
                        break;
                    case SRL: //SRLI
                        breg[rd] = ((uint32_t)breg[rs1]) >> Imm;
                        break;
                    case OR_REM: //ORI
                        breg[rd] = breg[rs1] | Imm;
                        break;
                    case AND: //ANDI
                        breg[rd] = breg[rs1] & Imm;
                        break;
                }
            case BRANCH:
                switch(funct3) {
                    case ADD_MUL_ADDI: //BEQ
                        if(breg[rs1]==breg[rs2]) {
                            PC = PC + (Imm << 1);
                        }
                        break;
                    case SLL: //BNE
                        if(breg[rs1] != breg[rs2]) {
                            PC = PC + (Imm << 1);
                        }
                        break;
                    case XOR_DIV: //BLT
                        if(breg[rs1] < breg[rs2]) {
                            PC = PC + (Imm << 1);
                        }
                        break;
                    case SRL: //BGE
                        if(breg[rs1] >= breg[rs2]) {
                            PC = PC + (Imm << 1);
                        }
                        break;
                }
            case LUI:
                breg[rd] = (Imm << 12);
                break;
            case AUIPC:
                breg[rd] = PC + (Imm << 12);
                break;
            case JALR:
                breg[rd] = PC + 4;
                PC = (breg[rs1] + Imm)&(!1);
                break;
            case JAL:
                breg[rd] = PC + 4;
                PC = PC + (Imm << 1);
                break;
            case LW:
                if(0xFFC < ((int32_t)breg[rs1]+Imm)) {
                    P_out_Data.write((int32_t)(breg[rs1]+Imm));
                    wait(SC_ZERO_TIME);
                    breg[rd] = P_in_Data.read();
                }
                break;
            case SW:
                if((int32_t)(breg[rs1]+Imm) > 0xFFF) {
                    P_out_Data.write((int32_t)(breg[rs1]+Imm));
                    Data.write(breg[rs2]);
                    Write_Signal.write(true);
                    wait(SC_ZERO_TIME);
                }
                break;
            //case ECALL: //Implementar funções de chamada do sistema
                //break;
        }
        execute_event.notify();
        wait(SC_ZERO_TIME);
	}
}
