#ifndef HPCC_PHYSICALSTATES_H
#define HPCC_PHYSICALSTATES_H

struct PhysicalStates {
    bool pump_flora_micro;
    bool pump_flora_gro;
    bool pump_flora_bloom;
    bool pump_ph_up_reservoir;
    bool pump_ph_up_basin;
    bool pump_ph_down_basin;
    bool submersible_pump;
    bool plant_lights;
    bool air_mover;
    bool r3;
    bool r4;
    bool r5;
    bool r6;
    bool r7;
    bool fan0;
    bool fan1;
};

#endif //HPCC_PHYSICALSTATES_H
