#include "Driver.h"
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

void Driver::Initialize_mem() {
    string str, inst;
    stringstream ss;
    bool flag = false;
    int32_t ins;
    int ct = 0;
    string::size_type n;
//Populando memória de dados
    int i;
    for(i=0;i<50;i++){
        Data_MEM[i] = i;
    }
//Abrir arquivo do RARS e popular a memória de instrução
    ifstream file;
    file.open("lui_auipc_load_text.mif");
    while(true) {
        getline(file,str);
        //cout << str << endl;
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
            //cout << inst << endl;
            ss << std::hex << inst;
            ss >> ins;
            //cout << "ins = " << ins << endl;
            Inst_MEM[ct] = ins;
            ct++;
            inst.clear();
            ss.str("");
            ss.clear();
        } else { //BEGIN sempre na linha 6 nos casos de teste
            ct++;
            if(ct == 6) {
                //cout << str << endl;
                flag = true;
                ct = 0;
            }
        }
    }
}
void Driver::Receive_PC() {
    while(true) {
        pc_value = P_in_Inst.read();
        counter++;
        R_inst.notify(SC_ZERO_TIME);
        //wait(SC_ZERO_TIME);
    }
}

void Driver::Receive_addr() {
    while(true) {
        data_addr = P_in_Data.read();
        if(Write_Signal.read()) {
            Data_MEM[data_addr] = Data.read();
        } else {
            R_data.notify(SC_ZERO_TIME);
        }
        //wait(SC_ZERO_TIME);
    }
}

void Driver::Send_Data() {
    while(true) {
        wait(R_data);
        P_out_Data.write(Data_MEM[data_addr-start_addr]);
        wait(SC_ZERO_TIME);
    }
}

void Driver::Send_Inst() {
    while(true) {
        wait(R_inst);
        if(counter == 10) { //Limitado a 10 instruções dado o tamanho de Inst_MEM, podendo ser alterado
            sc_stop();
            break;
        }
        //cout << "Inst_MEM[" << pc_value << "]" << endl;
        P_out_Inst.write(Inst_MEM[pc_value/4]);
        wait(SC_ZERO_TIME);
    }
}

void Driver::dump_data() {
    int i;
    for(i=0;i<50;i++) {
        cout << "MEM[" << i << "] = " << Data_MEM[i] << endl;
    }
}
