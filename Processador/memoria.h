/*
 *  memoria.h
 *  MIPSc++
 *
 *  Created by Ricardo Jacobi on 18/4/15.
 *  Copyright 2015 Universidade de Brasilia. All rights reserved.
 *
 */

#ifndef MEM_H 
#define MEM_H

#include <iostream>

using namespace std;

#define MEM_SIZE 20000
#define DATA_SEGMENT_START 0x2000

extern int32_t mem[MEM_SIZE];
extern int32_t int_mem[MEM_SIZE];

int32_t lb(const uint32_t &address, uint16_t kte);

int32_t lh(const uint32_t &address, uint16_t kte);

int32_t lw(const uint32_t &address, uint16_t kte);

void sb(const uint32_t &address, uint16_t kte, int8_t dado);

void sh(const uint32_t &address, uint16_t kte, int16_t dado);

void sw(const uint32_t &address, uint16_t kte, int32_t dado);

#endif