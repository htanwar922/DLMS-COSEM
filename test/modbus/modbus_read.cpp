#include "iostream"
#include "modbus_read.hpp"
#include "iomanip"

using namespace std;
int main()
{
    while(1){
    float reg_value;
    reg_value = meter.read_register(3840);
    std::cout<<left<<left<<setw(40)<<setfill('.')<<left<<setw(40)<<setfill('.')<<"apparent power "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3842);
    std::cout<<left<<setw(40)<<setfill('.')<<"active power "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3846);
    std::cout<<left<<setw(40)<<setfill('.')<<"avg PF "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3848);
    std::cout<<left<<setw(40)<<setfill('.')<<"line to line voltage "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3850);
    std::cout<<left<<setw(40)<<setfill('.')<<"line to neutral voltage "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3852);
    std::cout<<left<<setw(40)<<setfill('.')<<"average current "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3854);
    std::cout<<left<<setw(40)<<setfill('.')<<"frequency "<<fixed<<setprecision(6)<<reg_value<<std::endl;

    reg_value = meter.read_register(3856);
    std::cout<<left<<setw(40)<<setfill('.')<<"R-phase apparent power "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3858);
    std::cout<<left<<setw(40)<<setfill('.')<<"R-phase active power "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3862);
    std::cout<<left<<setw(40)<<setfill('.')<<"R-phase PF "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3864);
    std::cout<<left<<setw(40)<<setfill('.')<<"R-Y phase voltage "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3866);
    std::cout<<left<<setw(40)<<setfill('.')<<"R-phase voltage "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3868);
    std::cout<<left<<setw(40)<<setfill('.')<<"R-phase current "<<fixed<<setprecision(6)<<reg_value<<std::endl;

    reg_value = meter.read_register(3870);
    std::cout<<left<<setw(40)<<setfill('.')<<"Y-phase apparent power "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3872);
    std::cout<<left<<setw(40)<<setfill('.')<<"Y-phase active power "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3876);
    std::cout<<left<<setw(40)<<setfill('.')<<"Y-phase PF "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3878);
    std::cout<<left<<setw(40)<<setfill('.')<<"Y-B phase voltage "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3880);
    std::cout<<left<<setw(40)<<setfill('.')<<"Y-phase voltage "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3882);
    std::cout<<left<<setw(40)<<setfill('.')<<"Y-phase current "<<fixed<<setprecision(6)<<reg_value<<std::endl;

    reg_value = meter.read_register(3884);
    std::cout<<left<<setw(40)<<setfill('.')<<"B-phase apparent power "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3886);
    std::cout<<left<<setw(40)<<setfill('.')<<"B-phase active power "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3890);
    std::cout<<left<<setw(40)<<setfill('.')<<"B-phase PF "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3892);
    std::cout<<left<<setw(40)<<setfill('.')<<"B-R phase voltage "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3894);
    std::cout<<left<<setw(40)<<setfill('.')<<"B-phase voltage "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3896);
    std::cout<<left<<setw(40)<<setfill('.')<<"B-phase current "<<fixed<<setprecision(6)<<reg_value<<std::endl;

    reg_value = meter.read_register(3898);
    std::cout<<left<<setw(40)<<setfill('.')<<"Forward apparent energy "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3900);
    std::cout<<left<<setw(40)<<setfill('.')<<"Forward active energy "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    reg_value = meter.read_register(3840 + 98);
    std::cout<<left<<setw(40)<<setfill('.')<<"Power Interruptions "<<fixed<<setprecision(6)<<reg_value<<std::endl;
    while(1);
    }
}