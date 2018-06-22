#include <iostream>
#include "RiscV.h"
#include "Driver.h"

using namespace std;

int sc_main(int argc, char * argv[]) {
    Driver D("D");
    RiscV RV("RV");

    sc_fifo<int32_t> conn1(1), conn2(1), conn3(1), conn4(1);
    sc_signal<int32_t> signal1;
    sc_signal<bool> signal2;

    RV.P_in_Data(conn1);
    RV.P_in_Inst(conn2);
    RV.P_out_Data(conn3);
    RV.P_out_Inst(conn4);
    RV.Data(signal1);
    RV.Write_Signal(signal2);

    D.P_out_Data(conn1);
    D.P_out_Inst(conn2);
    D.P_in_Data(conn3);
    D.P_in_Inst(conn4);
    D.Data(signal1);
    D.Write_Signal(signal2);

    //D.Initialize_mem();

    sc_start();

    D.dump_data();

    RV.dump_breg();
    return 0;
}
