#include "RiscV.h"

void RiscV::init() {
	PC = ri = 0x0000; //Endereço inicial da memória de instruçoes
	memset(breg, 0, sizeof breg);
	while (ready_signal.read()) {
		wait(SC_ZERO_TIME);
	}
	cout << "-----------------------START RISCV-----------------------" << endl;
	init_event.notify(SC_ZERO_TIME);
}
/******************************************************************************
 fetch()
 - Busca instrucao na memoria e escreve no registrador de instrucoes ri
 - Incrementa pc
 ******************************************************************************/
void RiscV::fetch() {
	wait(init_event);
	while (true) {
		cout << "\nFETCH" << endl;
		P_out_Inst.write(PC);
		ri = P_in_Inst.read();
		cout << "Ri: " << ri << endl;
		if(ri == 0){
			cout << "-----------------------END RISCV-----------------------" << endl;
			while(true){
				wait();
			}
		}
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
	while (true) {
		wait(fetch_event);
		opcode = ri & 0x0000007F;
		if (opcode == OPC_R) {
			rd = (ri >> 7) & 0x1F;
			funct3 = (ri >> 12) & 0x07;
			rs1 = (ri >> 15) & 0x1F;
			rs2 = (ri >> 20) & 0x1F;
			funct7 = (ri >> 25) & 0x7F;
		} else if (opcode == TIPOI || opcode == LW || opcode == JALR) {
			rd = (ri >> 7) & 0x1F;
			funct3 = (ri >> 12) & 0x07;
			rs1 = (ri >> 15) & 0x1F;
			kte12 = (ri >> 20) & 0xFFF;
			//***************Cálculo do imediato********************
			Imm = kte12;
			//******************************************************
		} else if (opcode == BRANCH) {
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
		} else if (opcode == AUIPC || opcode == LUI) {
			rd = (ri >> 7) & 0x1F;
			kte20 = (ri >> 12) & 0xFFFFF;
			Imm = kte20;
		} else if (opcode == SW) {
			kte5 = (ri >> 7) & 0x1F;
			funct3 = (ri >> 12) & 0x07;
			rs1 = (ri >> 15) & 0x1F;
			rs2 = (ri >> 20) & 0x1F;
			kte7 = (ri >> 25) & 0x3F;
			//***************Cálculo do imediato********************
			Imm = (kte7 << 5) + kte5;
			//******************************************************
		} else if (opcode == JAL) {
			rd = (ri >> 7) & 0x1F;
			kte20 = (ri >> 12) & 0xFFFFF;
			//***************Cálculo do imediato********************
			Imm = (kte20 >> 9) & 0x3FF;
			Imm += ((kte20 >> 8) & 0x01) << 10;
			Imm += (kte20 & 0xFF) << 11;
			Imm += ((kte20 >> 19) & 0x01) << 19;
			//******************************************************
		}
		cout << "\nDECODE" << endl;
		decode_event.notify(SC_ZERO_TIME);
		wait(SC_ZERO_TIME);
	}
}

void RiscV::debug_decode() {
	cout << "PC = " << PC << endl;
	cout << "opcode = " << opcode << " rs1 = " << rs1 << " rs2 = " << rs2
			<< " rd = " << rd << " funct7 = " << funct7 << " funct3 = "
			<< funct3 << endl;
}

void RiscV::dump_breg() {
	cout << "-----------------------DUMP BREG-----------------------" << endl;
	for (int i = 0; i < 32; i++) {
		printf("BREG[%2d] = \t%8d \t\t\t%8x\n", i, breg[i], breg[i]);
	}
	cout << "-------------------------------------------------------" << endl;

}

void RiscV::execute() {
	/**********************************************************************************************
	 As operações de shift logico estão com typecast para unsigned visando evitar a extensão de sinal
	 uma vez que esta só acontece no shift aritmético
	 ***********************************************************************************************/
	while (true) {
		wait(decode_event);
		cout << "\nEXECUTE" << endl;
		breg[0] = 0;
		cout << "opcode:" << opcode << endl;
		switch (opcode) {
		case OPC_R:
			switch (funct7) {
			case ZERO7:
				switch (funct3) {
				case ADD_MUL_ADDI:
					breg[rd] = breg[rs1] + breg[rs2];
					cout << "breg[" << rd << "]:" << breg[rd] << endl;
					break;
				case SLL:
					breg[rd] = ((uint32_t) breg[rs1]) << breg[rs2];
					break;
				case SLT:
					breg[rd] = (breg[rs1] < breg[rs2]) ? 1 : 0;
					break;
				case SLTU:
					breg[rd] =
							((uint32_t) breg[rs1] < (uint32_t) breg[rs2]) ?
									1 : 0;
					break;
				case XOR_DIV:
					breg[rd] = breg[rs1] ^ breg[rs2];
					break;
				case SRL:
					breg[rd] = ((uint32_t) breg[rs1]) >> breg[rs2];
					break;
				case OR_REM:
					breg[rd] = breg[rs1] | breg[rs2];
					break;
				case AND:
					breg[rd] = breg[rs1] & breg[rs2];
					break;
				}
				break;
			case SUB7:
				switch (funct3) {
				case ADD_MUL_ADDI: //SUB
					breg[rd] = breg[rs1] - breg[rs2];
					break;
				case SRL: //SRA
					breg[rd] = breg[rs1] >> breg[rs2];
					break;
				}
				break;
			}
			break;
		case TIPOI:
			switch (funct3) {
			case ADD_MUL_ADDI: //ADDI
				breg[rd] = breg[rs1] + Imm;

				break;
			case SLT: //SLTI
				breg[rd] = (breg[rs1] < Imm) ? 1 : 0;
				break;
			case XOR_DIV: //XORI
				breg[rd] = breg[rs1] ^ Imm;
				break;
			case SRL: //SRLI
				breg[rd] = ((uint32_t) breg[rs1]) >> Imm;
				break;
			case OR_REM: //ORI
				breg[rd] = breg[rs1] | Imm;
				break;
			case AND: //ANDI
				breg[rd] = breg[rs1] & Imm;
				break;
			}
			break;
		case BRANCH:
			switch (funct3) {
			case ADD_MUL_ADDI: //BEQ
				if (breg[rs1] == breg[rs2]) {
					PC = (PC - 4) + (Imm << 1);
				}
				break;
			case SLL: //BNE
				if (breg[rs1] != breg[rs2]) {
					PC = (PC - 4) + (Imm << 1);
				}
				break;
			case XOR_DIV: //BLT
				if (breg[rs1] < breg[rs2]) {
					PC = (PC - 4) + (Imm << 1);
				}
				break;
			case SRL: //BGE
				if (breg[rs1] >= breg[rs2]) {
					PC = (PC - 4) + (Imm << 1);
				}
				break;
			}
			break;
		case LUI:
			breg[rd] = (Imm << 12);
			break;
		case AUIPC:
			breg[rd] = (PC - 4) + (Imm << 12);
			break;
		case JALR:
			breg[rd] = PC;
			PC = (breg[rs1] + Imm);
			break;
		case JAL:
			breg[rd] = PC;
			PC = (PC-4) + (Imm << 1);
			break;
		case LW:
			if (data_mem <= ((int32_t) breg[rs1] + Imm)) {
				wait(10, SC_NS);
				P_out_Data.write((int32_t) (breg[rs1] + Imm));
				Write_Signal.write(false);
				wait(SC_ZERO_TIME);
				breg[rd] = P_in_Data.read();
			}
			break;
		case SW:
			if ((int32_t) (breg[rs1] + Imm) >= data_mem) {
				wait(10, SC_NS);
				P_out_Data.write((int32_t) (breg[rs1] + Imm));
				Data.write(breg[rs2]);
				Write_Signal.write(true);
				wait(SC_ZERO_TIME);

			}
			break;
		}
		cout << "breg[" << rd << "]:" << breg[rd] << endl;
		execute_event.notify();
		wait(SC_ZERO_TIME);
	}
}
