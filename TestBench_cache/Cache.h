//MODIFICADO
#ifndef __Cache_h
#define __Cache_h

#include "systemc.h"
#include "simple_bus_blocking_if.h"
#include "simple_bus_types.h"
#include <math.h>
#include <bitset>
#include <string>
#include <stdint.h>
#include <sstream>

using namespace std;

SC_MODULE(Cache) {
    static const int Bits_per_address = 32;
    static const int n_ways = 2; //Número de blocos por set
    static const int Cache_size = 1024; //Tamanho em words
    static const int block_size = 4; //Words por bloco
    static const int set_size = Cache_size/(n_ways*block_size);
    static const int OFFSET = log2(block_size);
    static const int SET = log2(set_size);
    static const int TAG = Bits_per_address - SET - OFFSET;
    static const int burst_size = block_size;

    simple_bus_status status;

    bool data_found;

    typedef struct {
        bool valid;
        int Tg;
        int32_t Dt[block_size];
    } blocks;

    blocks Cache_Data[set_size][n_ways];

    bool LRU[set_size] = {0};

    int32_t address;
    int32_t Store_data[burst_size];
    int32_t received_data[burst_size];
    int32_t retrieved_data;

    sc_in<int32_t> Data_in;
    //processor_in fifo
    sc_fifo_in<int32_t> Processor_in;
    sc_fifo_out<int32_t> Processor_out;
    sc_in<bool> Write_Signal;
    sc_port<simple_bus_blocking_if> Bus_port;

    sc_event processor_receive_event;
    sc_event search_event;

    int tag_field;
    int set_field;
    int offset_field;

    void receive_address(); //Recebe endereço do processador
    void search_data(); //Procura se o endereço está na cache, caso não esteja envia requisição para a memória
    void send_data(); //Envia o conteúdo do endereço para o processador

    SC_HAS_PROCESS(Cache);

    Cache(sc_module_name name_
        , unsigned int unique_priority
        , int32_t cache_start_address
        , int32_t cache_end_address
        , bool lock
        , int timeout   )
    : sc_module(name_)
    , cache_unique_priority(unique_priority)
    , cache_start_address(cache_start_address)
    , cache_end_address(cache_end_address)
    , cache_lock(lock)
    , cache_timeout(timeout)
     {
        SC_THREAD(receive_address);

        SC_THREAD(search_data);
        sensitive << processor_receive_event;

        SC_THREAD(send_data);
        sensitive << search_event;

        data_found = false;
    }

    private:
        unsigned int cache_unique_priority;
        int32_t cache_start_address;
        int32_t cache_end_address;
        bool cache_lock;
        int cache_timeout;
};
/*******************************************************************************************
receive_address()
- Recebe endereço da porta Processor_In
- Coloca esse endereço em formato binário
- Separa os campos TAG, SET e OFFSET
* Vale lembrar que os endereços requisitados pelo processador serão dados de 4 em 4, porém
aqui consideramos word addressing
- Transforma eles em inteiros e armazena nas variaveis da classe tag_field, set_field e
offset_field
*******************************************************************************************/
inline void Cache::receive_address() {
    int i;
    string bin_address, t, s, o;
    stringstream ss;

    while(true) {
        address = Processor_in.read();
        address /= 4;
        t.resize(TAG);
        s.resize(SET);
        o.resize(OFFSET);
        bin_address = bitset<Bits_per_address>(address).to_string();
        for(i=0;i<Bits_per_address;i++) {
            if(i < TAG) {
                t.at(i) = bin_address.at(i);
            } else if (i >= TAG && i < SET+TAG) {
                s.at(i-TAG) = bin_address.at(i);
            } else {
                o.at(i-(TAG+SET)) = bin_address.at(i);
            }
        }
        ss << t;
        ss >> tag_field;
        ss.str("");
        ss.clear();
        ss << s;
        ss >> set_field;
        ss.str("");
        ss.clear();
        ss << o;
        ss >> offset_field;
        ss.str("");
        ss.clear();
        processor_receive_event.notify();
        wait(SC_ZERO_TIME);
    }
}
/**********************************************************************************************
search_data()
- Procura se o endereço está na cache a partir dos campos decodificados em receive_data()
- Se não está na cache, faz um burst read para o simple_bus, buscando os dados
* Importante ressaltar que deve haver a verificação se a leitura em burst cabe na cache e,
caso nao caiba deve-se sobrescrever os dados já existentes nos lugares correspondentes
**********************************************************************************************/
inline void Cache::search_data() {
    int i, j, k, cnt;
    int32_t new_address;
    while(true) {
        wait(processor_receive_event);
        for(i=0;i<n_ways;i++) {
            if(Cache_Data[set_field][i].valid == true) {
                if(Cache_Data[set_field][i].Tg == tag_field) {
                    retrieved_data = Cache_Data[set_field][i].Dt[offset_field];
                    data_found = true;
                    LRU[set_field] = ~LRU[set_field]; //????
                    search_event.notify();
                }
            }
        }
        new_address = address - offset_field;
        if(data_found == false) { //Acho que falta o notify para o data_found == false && write_signal.read() == false
            status = Bus_port->burst_read(cache_unique_priority, received_data,
                     new_address, burst_size, cache_lock);
            if (status == SIMPLE_BUS_ERROR)
                sb_fprintf(stdout, "%s %s : cache-read failed at address %x\n",
                sc_time_stamp().to_string().c_str(), name(), address);

            cnt = 0;
            i = 0;
            j = set_field;
            if(LRU[j] == 0) {
                k = 0;
            } else {
                k = 1;
            }

            while(i < block_size && cnt < burst_size) {
                    Cache_Data[j][k].Dt[i] = received_data[cnt];
                    if(Cache_Data[j][k].valid == false)
                        Cache_Data[j][k].valid = true;
                    LRU[j] = ~LRU[j]; //Alteração do LRU
                    Cache_Data[j][k].Tg = tag_field;
                    i++;
                    cnt++;
            }

            if(Write_Signal.read()) {
                for(i=0;i<n_ways;i++) {
                    if(Cache_Data[set_field][i].Tg == tag_field) {
                        Cache_Data[set_field][i].Dt[offset_field] = Data_in.read();
                        break;
                    }
                }
                for(cnt=0;cnt<block_size;cnt++) {
                    Store_data[cnt] = Cache_Data[set_field][i].Dt[cnt];
                }
                //realizar burst write para a memória
                status = Bus_port->burst_write(cache_unique_priority, Store_data,
                         address, burst_size, cache_lock);
                      if (status == SIMPLE_BUS_ERROR)
                sb_fprintf(stdout, "%s %s : blocking-write failed at address %x\n",
                    sc_time_stamp().to_string().c_str(), name(), address);
            }else
            	search_event.notify(); //COLOQUEI AK POR VIA DAS DUVIDAS
        }
    }
}

/********************************************************************************
send_data
- Apenas escreve os dados em Processor_out
********************************************************************************/
inline void Cache::send_data() {
    while(true) {
        wait(search_event);
        Processor_out.write(retrieved_data);
    }
}

#endif
