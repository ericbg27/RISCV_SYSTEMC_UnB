/*
 *  main.cpp
 *  MIPS
 *
 *  Created by Ricardo Jacobi on 18/04/11.
 *  Copyright 2011 Universidade de Brasilia. All rights reserved.
 *
 */

#include <iostream>
#include "globals.h"
#include "memoria.h"
#include "mips.h"

using namespace std;

int main (int argc, const char * argv[]) {
	/*
    pc = 0;
	mem[0] = 0x04432146;
	
	fetch();
	decode();
	debug_decode();
	dump_breg();
	*/
	/*
	load_mem("buble.bin", 0);
	load_mem("buble.data", 8192);
	cout << "Dados originais:" << endl << endl;
	dump_mem(8192, 8224, 'd');
	 */
	/*
	int size = load_int_mem("fib.bin", 0);
	cout << "Bloco lido = " << size << endl;
	cout << "Dados originais:" << endl << endl;
	dump_int_mem(0, size, 'h');
	
	cout << "lw(0) = " << hex << load_word(0) << endl;
	cout << "lw(4) = " << hex << load_word(4) << endl;
	
	cout << "lb(0) = " << hex << (int)load_byte(0) << endl;
	cout << "lb(3) = " << hex << (int)load_byte(3) << endl;
	 */
	
	/*
	run();
	cout << "Dados processados:" << endl;
	dump_mem(8192, 8224, 'd');
	 */
	
	load_mem("buble.bin", 0);
	load_mem("buble.data", 8192);
	cout << "Dados memória: " << endl;
	dump_mem(8192, 8224, 'd');
	run();
	
	dump_breg();
	
	cout << "Dados ordenados: " << endl;
	dump_mem(8192, 8224, 'd');
	
	
    /* teste de acesso a memoria
	int32_t b0, b1, b2, b3;
	int32_t h0, h1;
		
	//mem[0] = 0x01020304;
		
	int32_t s0 = 0;
		
	sb((uint32_t)s0, 0, 0x04);
	sb((uint32_t)s0, 1, 0x03);
	sb((uint32_t)s0, 2, 0x02);
	sb((uint32_t)s0, 3, 0x01);
		
	b0 = lb((uint32_t)s0, 0);
	b1 = lb((uint32_t)s0, 1);
	b2 = lb((uint32_t)s0, 2);
	b3 = lb((uint32_t)s0, 3);
				
	s0 = 400;
	
	//sw(s0, 0, 0x00100020);
	sh(s0, 0, 0x0020);
	sh(s0, 2, 0x0010);
		
	h0 = lh(s0, 0);
	h1 = lh(s0, 2);
		
	cout << "\nByte 0 = " << (int)b0;
	cout << "\nByte 1 = " << (int)b1;
	cout << "\nByte 2 = " << (int)b2;
	cout << "\nByte 3 = " << (int)b3;
		
	cout << "\nShort 0 = " << (int)h0;
	cout << "\nShort 1 = " << (int)h1;
	
	h0 = lh(0, 0);
	h1 = lh(0, 2);
	
	cout << "\nShort 0 = " << hex << (int)h0;
	cout << "\nShort 1 = " << hex << (int)h1;
		
	cout << endl;
    */
		
	return 0;
}
