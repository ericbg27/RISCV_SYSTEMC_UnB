/*
 *  mips.cpp
 *  MIPSc++
 *
 *  Created by Ricardo Jacobi on 18/4/15.
 *  Copyright 2015 Universidade de Brasilia. All rights reserved.
 *
 */


#include "mips.h"
#include "memoria.h"

void init() {
	pc = ri = hi = lo = 0;
}
// busca instrucao na memoria e escreve no registrador de instrucoes ri
void fetch () {
	ri = lw(pc, 0);
	pc = pc + 4;
}

void decode () {
	opcode	= (ri >> 26) & 0x3F;
	rs		= (ri >> 21) & 0x1F;
	rt		= (ri >> 16) & 0x1F;
	rd		= (ri >> 11) & 0x1F;
	shamt	= (ri >> 6) & 0x1F;
	funct	= ri & 0x3F;
	kte16	= ri & 0xFFFF;
	kte26	= ri &0x03FFFFFF;
}


void debug_decode() {
	cout << "PC = " << pc << endl;
	cout << "opcode = " << opcode << " rs = " << rs << " rt = " << rt << " rd = " << rd << " shamt = " << shamt << " funct = " << funct << endl;
}

void dump_breg() {
	for (int i=0; i<32; i++) {
		printf("BREG[%2d] = \t%8d \t\t\t%8x\n", i, breg[i], breg[i]);
	}
}


void dump_mem(int start, int end, char format) {
	switch (format) {
		case 'h':
		case 'H':
			for (uint32_t i = start; i <= end; i+=4)
				printf("%x \t%x\n", i, lw(i, 0));
			break;
		case 'd':
		case 'D':
			for (int i = start; i <= end; i+=4)
				printf("%x \t%d\n", i, lw(i, 0));
			break;
		default:
			break;
	}
}

int load_mem(const char *fn, int start) {
	FILE *fptr;
	int *m_ptr = mem + (start>>2);  
	int size = 0;
	
	fptr = fopen(fn, "rb");
	if (!fptr) {
		printf("Arquivo nao encontrado!");
		return -1;
	}
	else {
		while (!feof(fptr)) {
			fread(m_ptr, 4, 1, fptr);
			m_ptr++;
			size++;
		}
		fclose(fptr);
	}
	return size;
}

void execute () {
	int64_t u;
	char *c;
	
	breg[0] = 0;
	switch (opcode) {
		case FUN:
			switch (funct) {
				case SYSCALL: // emula as chamadas do MARS
					switch (V0) {
						case 1:	cout << breg[A0]; break;
						case 2:                   break;
						case 3:				      break;
						case 4:	c = (char *)&mem[breg[A0]>>2];
                            c += breg[A0]%4;
							while ((*c++) != 0)
								cout << c;
							cout << endl; 
							break;
						case 5:	cin >> breg[V0]; break;
						case 6:                  break;
                        case 10: exit(0);
						default:break;
					}
				case SLL:  breg[rd] = breg[rt] << shamt;			break;
				case SRL:  breg[rd] = (uint32_t)breg[rt] >> shamt;	break;
				case SRA:  breg[rd] = breg[rt] >> shamt;			break;
				case JR:   pc = breg[rs];							break;
				case MUL:	
					u = breg[rs]*breg[rt];
					hi = (u >> 32); 
					lo = (u & 0xFFFFFFFF);
					break;
				case DIV:  
					lo = breg[rs]/breg[rt]; 
					hi = breg[rs]%breg[rt]; 
					break;
				case ADD:  breg[rd] = breg[rs] + breg[rt];        break;
				case ADDU: breg[rd] = breg[rs] + breg[rt];        break;
				case SUB:  breg[rd] = breg[rs] - breg[rt];        break;
				case AND:  breg[rd] = breg[rs] & breg[rt];        break;
				case OR:   breg[rd] = breg[rs] | breg[rt];        break;
				case XOR:  breg[rd] = breg[rs] ^ breg[rt];        break;
				case NOR:  breg[rd] = ~(breg[rs] | breg[rt]);     break;
				case SLT:  breg[rd] = breg[rs] < breg[rt];        break;
				default: printf("Instrucao Invalida (*0x%x~0x%x)\n", pc, ri);
			}
			break;
		case JAL:   breg[RA] = pc;								
			pc = (pc & 0xf0000000) | (kte26<<2);
			break;
			
		case J:     pc = (pc & 0xf0000000) | (kte26<<2);		break;
			
		case BEQ:   if (breg[rs] == breg[rt])
			pc += (kte16 << 2);     
			break;
		case BNE:   if (breg[rs] != breg[rt])
			pc += (kte16 << 2);     
			break;
		case BLEZ:  if (breg[rs] <= 0)
			pc += (kte16 << 2);     
			break;
		case BGTZ:  if (breg[rs] > 0)
			pc += (kte16 << 2);     
			break;
		case ADDI:	breg[rt] = breg[rs] + (short)kte16;						break;
		case ADDIU:	breg[rt] = (unsigned)breg[rs]+(short)kte16;				break;
		case SLTI:	breg[rt] = breg[rs] < (short)kte16;						break;
		case SLTIU:	breg[rt] = (unsigned)breg[rs] < (unsigned)kte16;		break;
		case ANDI:	breg[rt] = breg[rs] & kte16;							break;
		case ORI:	breg[rt] = breg[rs] | kte16;							break;
		case XORI:	breg[rt] = breg[rs] ^ kte16;							break;
		case LUI:	breg[rt] = (kte16 << 16);								break;
		case LB:	breg[rt] = (char)(mem[breg[rs] + kte16]);				break;
		case LH:	breg[rt] = lh(breg[rs],kte16);                          break;
		case LW:	breg[rt] = lw(breg[rs], kte16);                         break;
		case LHU:	breg[rt] = (uint32_t)mem[breg[rs] + kte16];             break;
		case SB:	sb(breg[rs], kte16, get_byte_0(breg[rt]));              break;
		case SH:	sh(breg[rs], kte16, (uint16_t)(breg[rt]&0xFFFF));		break;
		case SW:	sw(breg[rs], kte16, breg[rt]);                          break;
		default:
			break;
	}
}

void step() {
	fetch();
	decode();
    execute();
}

void run() {
	init();
	while (pc < DATA_SEGMENT_START)
        step();
}

