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
#include "driver.h"

int sc_main(int, char **)
{
  // channels
  sc_clock C1;
  // module instances
  simple_bus_master_blocking     *master_b;
  simple_bus_master_non_blocking *master_nb;
  simple_bus_master_direct       *master_d;
  simple_bus_slow_mem            *mem_slow;
  simple_bus                     *bus;
  simple_bus_fast_mem            *mem_fast;
  simple_bus_arbiter             *arbiter;
  master_b = new simple_bus_master_blocking("master_b", 4, 0x4c, false, 300);
  master_nb = new simple_bus_master_non_blocking("master_nb", 3, 0x38, false, 20);
  master_d = new simple_bus_master_direct("master_d", 0x00, 100);
  mem_fast = new simple_bus_fast_mem("mem_fast", 0x00, 0x7f);
  mem_slow = new simple_bus_slow_mem("mem_slow", 0x80, 0xff, 1);
  // bus = new simple_bus("bus",true); // verbose output
  bus = new simple_bus("bus");
  // arbiter = new simple_bus_arbiter("arbiter",true); // verbose output
  arbiter = new simple_bus_arbiter("arbiter");

  cache_driver driver("driver");
  Cache data_cache("data_cache", 0, 0, 0xff, false, 100);

  sc_fifo<int32_t> Processor_in(1), Processor_out(1);
  sc_signal<bool> Write_Signal;
  sc_signal<int32_t> Data_in;


  // connect instances
  master_d->clock(C1);
  bus->clock(C1);
  master_b->clock(C1);
  master_nb->clock(C1);
  mem_slow->clock(C1);
  master_d->bus_port(*bus);
  master_b->bus_port(*bus);
  master_nb->bus_port(*bus);
  bus->arbiter_port(*arbiter);
  bus->slave_port(*mem_slow);
  bus->slave_port(*mem_fast);

  driver.Data_in(Data_in);
  driver.Processor_in(Processor_in);
  driver.Processor_out(Processor_out);
  driver.Write_Signal(Write_Signal);

  data_cache.Bus_port(*bus);
  data_cache.Data_in(Data_in);
  data_cache.Processor_in(Processor_in);
  data_cache.Processor_out(Processor_out);
  data_cache.Write_Signal(Write_Signal);

  sc_start(10000, SC_NS);

  return 0;
}
