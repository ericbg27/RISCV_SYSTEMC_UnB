#ifndef DRIVER_H_
#define DRIVER_H_

#include "systemc.h"
#include "simple_bus_test.h"
#include <iostream>

SC_MODULE(cache_driver){
    sc_out<int32_t> Data_in;
    sc_fifo_out<int32_t> Processor_in;
    sc_fifo_in<int32_t> Processor_out;
    sc_out<bool> Write_Signal;

    void source();
    void sink();

    SC_CTOR(cache_driver){
    	SC_METHOD(source);
    	SC_THREAD(sink);
    }
};


inline void cache_driver::source(){
	Data_in.write(2);
	Processor_in.write(0x00);
	Write_Signal.write(1);
}

inline void cache_driver::sink(){
	while(true){
		std::cout<< "Processor out: " << Processor_out.read() << std::endl;
	}
}

#endif /* DRIVER_H_ */
