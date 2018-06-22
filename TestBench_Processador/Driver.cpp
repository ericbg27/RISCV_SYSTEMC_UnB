#include "Driver.h"

using namespace std;

void Driver::Initialize_mem() {
//Populando memória de dados
    int i;
    for(i=0;i<50;i++){
        Data_MEM[i] = i;
    }

//Abrir arquivo do RARS e popular a memória de instrução
}

void Driver::Receive_PC() {
    while(true) {
        pc_value = P_in_Inst.read();
        counter++;
        R_inst.notify();
        wait(SC_ZERO_TIME);
    }
}

void Driver::Receive_addr() {
    while(true) {
        data_addr = P_in_Data.read();
        if(Write_Signal.read()) {
            MEM[data_addr-start_addr] = Data.read();
        } else {
            R_data.notify();
        }
        wait(SC_ZERO_TIME);
    }
}

void Driver::Send_Data() {
    while(true) {
        P_out_Data.write(Data_MEM[data_addr-start_addr]);
    }
}

void Driver::Send_Inst() {
    while(true) {
        P_out_Inst.write(Inst_MEM[pc_value]);
        if(counter == MAX_INST) { //Limitado a 10 instruções dado o tamanho de Inst_MEM, podendo ser alterado
            sc_stop();
            break;
        }
    }
}

void Driver::dump_data() {
    int i;
    for(i=0;i<50;i++) {
        cout << "MEM[" << i << "] = " << Data_MEM[i] << endl;
    }
}
