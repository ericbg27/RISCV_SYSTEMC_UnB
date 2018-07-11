/*****************************************************************************

 Licensed to Accellera Systems Initiative Inc. (Accellera) under one or
 more contributor license agreements.  See the NOTICE file distributed
 with this work for additional information regarding copyright ownership.
 Accellera licenses this file to you under the Apache License, Version 2.0
 (the "License"); you may not use this file except in compliance with the
 License.  You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 implied.  See the License for the specific language governing
 permissions and limitations under the License.

 *****************************************************************************/

/*****************************************************************************
 
 simple_bus_main.cpp : sc_main
 
 Original Author: Ric Hilderink, Synopsys, Inc., 2001-10-11
 
 *****************************************************************************/

/*****************************************************************************
 
 MODIFICATION LOG - modifiers, enter your name, affiliation, date and
 changes you are making here.
 
 Name, Affiliation, Date:
 Description of Modification:
 
 *****************************************************************************/

#include "systemc.h"
#include "simple_bus_master_blocking.h"
#include "simple_bus_master_non_blocking.h"
#include "simple_bus_master_direct.h"
#include "simple_bus_slow_mem.h"
#include "simple_bus.h"
#include "simple_bus_fast_mem.h"
#include "simple_bus_arbiter.h"
#include "Cache.h"
#include "Cache_inst.h"
#include "RiscV.h"
#include "init_process.h"

int sc_main(int, char **) {
	// channels
	sc_clock C1;
	// module instances
	simple_bus_master_direct *master_d;
	simple_bus_slow_mem *mem_slow;
	simple_bus *bus;
	simple_bus_fast_mem *mem_fast;
	simple_bus_arbiter *arbiter;
	master_d = new simple_bus_master_direct("master_d", 0x40, 0xff);
	mem_fast = new simple_bus_fast_mem("mem_fast", 0x00, 0x7f);
	mem_slow = new simple_bus_slow_mem("mem_slow", 0x80, 0xff, 1);
	bus = new simple_bus("bus");
	arbiter = new simple_bus_arbiter("arbiter");

	Cache data_cache("data_cache", 0, 0, 0xff, false, 100);
	Cache_inst inst_cache("inst_cache", 0, 0, 0xff, false, 100);
	RiscV RV("RV");
	init_process init("init");

	sc_fifo<int32_t> Processor_in_data(1), Processor_out_data(1),
			Processor_in_inst(1), Processor_out_inst(1);
	sc_signal<bool> Write_Signal, ready_signal;
	sc_signal<int32_t> Data_in;

	// connect instances
	master_d->clock(C1);
	bus->clock(C1);

	mem_slow->clock(C1);
	master_d->bus_port(*bus);

	bus->arbiter_port(*arbiter);
	bus->slave_port(*mem_slow);
	bus->slave_port(*mem_fast);

	RV.P_in_Data(Processor_out_data);
	RV.P_in_Inst(Processor_out_inst);
	RV.P_out_Data(Processor_in_data);
	RV.P_out_Inst(Processor_in_inst);
	RV.Data(Data_in);
	RV.Write_Signal(Write_Signal);
	RV.ready_signal(ready_signal);

	data_cache.Bus_port(*bus);
	data_cache.Data_in(Data_in);
	data_cache.Processor_in(Processor_in_data);
	data_cache.Processor_out(Processor_out_data);
	data_cache.Write_Signal(Write_Signal);

	inst_cache.Bus_port(*bus);
	inst_cache.Processor_in(Processor_in_inst);
	inst_cache.Processor_out(Processor_out_inst);

	init.bus_port(*bus);
	init.ready_signal(ready_signal);

	sc_start(1000, SC_NS);

	RV.dump_breg();
	inst_cache.dump_cache();
	data_cache.dump_cache();
	return 0;
}
