/**
 * linear scan register allocation
 */

#ifndef _LINEARSCAN_H__
#define _LINEARSCAN_H__
#include <list>
#include <map>
#include <set>
#include <vector>

class MachineUnit;
class MachineOperand;
class MachineFunction;

class LinearScan
{
private:
    struct Interval
    {
        int start;
        int end;
        bool spill;
        int disp;
        int rreg;
        std::set<MachineOperand *> defs;
        std::set<MachineOperand *> uses;
    };
    MachineUnit *unit;
    MachineFunction *func;
    std::vector<int> regs;
    std::map<MachineOperand *, std::set<MachineOperand *>> du_chains;
    std::vector<Interval *> intervals;
    std::vector<Interval *> active;
    static bool compareStart(Interval *a, Interval *b);
    static bool compareEnd(Interval *a, Interval *b);
    void expireOldIntervals(Interval *interval);
    void spillAtInterval(Interval *interval);
    void makeDuChains();
    void computeLiveIntervals();
    bool linearScanRegisterAllocation();
    void modifyCode();
    void genSpillCode();

public:
    LinearScan(MachineUnit *unit);
    void allocateRegisters();
};

#endif