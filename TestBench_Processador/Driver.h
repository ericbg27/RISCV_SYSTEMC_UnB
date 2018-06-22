#include "systemc.h"
#include <iostream>

SC_MODULE(Driver) {
    const int MAX_INST = 10;

    sc_fifo_in<int32_t> P_in_Data; //Entrada da cache de dados
    sc_fifo_in<int32_t> P_in_Inst; //Entrada da cache de instruções
    sc_fifo_out<int32_t> P_out_Data; //Saída de endereços para cache de dados
    sc_fifo_out<int32_t> P_out_Inst; //Saída de endereços para cache de instruções
    sc_in<int32_t> Data; //Dados
    sc_in<bool> Write_Signal; //Sinal de ativação da escrita, conectado em ambas as caches

    void start_addr = 0x2000;
    int counter;

    int32_t pc_value;
    int32_t data_addr;
    int32_t Data_MEM[50];
    int32_t Inst_MEM[MAX_INST];

    void Initialize_mem();
    void Receive_PC();
    void Receive_addr();
    void Send_Inst();
    void Send_Data();
    void dump_data();
    sc_event R_inst;
    sc_event R_data;

    SC_CTOR(Driver) {
        SC_METHOD(Initialize_mem);

        SC_METHOD(dump_data);

        SC_THREAD(Receive_PC);
        sensitive << P_in_Inst;

        SC_THREAD(Receive_addr);
        sensitive << P_in_Data;

        SC_THREAD(Send_Inst);
        sensitive << R_inst;

        SC_THREAD(Send_Data);
        sensitive << R_data;

        counter = 0;
    }
};
