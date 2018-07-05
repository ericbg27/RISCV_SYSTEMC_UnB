#ifndef INIT_PROCESS_H_
#define INIT_PROCESS_H_

#include "systemc.h"
#include "simple_bus_direct_if.h"
#include "simple_bus_types.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
using namespace std;


SC_MODULE(init_process){
    bool status;
    sc_out<bool> ready_signal;
    sc_port<simple_bus_direct_if> bus_port;


    void proc();
    SC_CTOR(init_process){
    	SC_THREAD(proc);
    }

};


inline void init_process::proc(){
    	string str, inst;
    	stringstream ss;
		ifstream file;
	    bool flag = false;
	    string::size_type n;
	    int32_t ins;
	    int ct = 0;
	    int32_t address = 0x00;

	    file.open("teste1_text.mif");
	    while(true) {
	        getline(file,str);
	        if(flag == true) {
	            n = str.find(":");
	            if(n == string::npos) {
	                break;
	            }
	            for(int i=0;i<str.length();i++) {
	                if(str.at(i) == ':') {
	                    for(int j=i+2;j<=i+9;j++) {
	                        inst += str.at(j);
	                    }
	                    break;
	                }
	            }
	            ss << std::hex << inst;
	            ss >> ins;
	            //PEGANDO INS ERRADO PARA NUMEROS NEGATIVOS
	            cout << "ins" << ins << endl;
                status = bus_port->direct_write(&ins,address);
                if (status == 0)
                	sb_fprintf(stdout, "%s %s : blocking-write failed at address %x\n",
                    sc_time_stamp().to_string().c_str(), name(), address);


	            address = address + 4;

				cout << std::hex << "instr:" << inst << endl;
	            ct++;
	            inst.clear();
	            ss.str("");
	            ss.clear();
	        } else { //BEGIN sempre na linha 6 nos casos de teste
	            ct++;
	            if(ct == 6) {
	                flag = true;
	                ct = 0;
	            }
	        }

	    }
	    ready_signal.write(true);
	    wait(SC_ZERO_TIME);
}

#endif
