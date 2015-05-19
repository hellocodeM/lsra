#include <cstdio>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <regex>


namespace lsra {
using Inst = std::string;

struct Interval {
    int start, end, id;
    Interval(int ss, int ee, int ii): start(ss), end(ee), id(ii) {}

};

std::ostream& operator<< (std::ostream& os, Interval& interval) {
    os << interval.id    << ' ' << 
          interval.start << ' ' <<
          interval.end;
    return os;
}


/**
 * live analysis
 */
std::vector<Interval> LiveAnalysis(std::vector<Inst>& instructions) {
    std::vector<Interval> intervals;
    for (int i = 0; i < instructions.size(); i++) {
        std::regex r("%(\\d+)");
        Inst& inst = instructions[i];
        auto iter = std::sregex_iterator(inst.begin(), inst.end(), r);
        auto iter_end = std::sregex_iterator();

        while (iter != iter_end){
            int register_id = std::stoi((*iter)[1]);
            auto interval_pos = std::find_if(intervals.begin(), intervals.end(), [&](Interval& inter) {
                return inter.id == register_id;
            });
            if (interval_pos == intervals.end()) {
                intervals.emplace_back(i, i, register_id);
            } else {
                interval_pos->end = i;
            }
            iter++;
        }
    }
    return intervals;
}


void AddToActive(std::vector<Interval>& active, const Interval& interval) {
    auto pos = std::upper_bound(active.begin(), active.end(), interval, [](auto x, auto y) {
        return x.end < y.end;
    });
    active.insert(pos, interval);
}

/**
 * alloca a register for the interval, add it into the active-list and keep increasing order,
 * @return the register allocated
 */
unsigned AllocaRegister(std::vector<Interval>& active, std::vector<unsigned>& regi_pool, const Interval& interval) {
    unsigned regi = regi_pool.back();
    regi_pool.pop_back();
    AddToActive(active, interval);
    return regi;
}

void Expire(std::vector<Interval>& active, std::vector<unsigned>& regi_pool, const Interval& cur) {
    for (auto i = active.begin(); i != active.end(); ) {
        if (i->end >= cur.start) return;
        regi_pool.push_back(i->id);
        i = active.erase(i);
    }
}

/**
 * @param instructions: stream of instructions
 * @param num_register: number of register to use, except for special registers 
 * @return : instructions after register allocation
 */
std::vector<std::string>
LSRA(std::vector<Inst>& instructions, int num_register) {
    /* After liveAnalysis, the interval has already sorted by start-point */
    std::vector<Interval> intervals = LiveAnalysis(instructions);
    std::vector<Inst> res(instructions);
    std::vector<Interval> active;
    std::vector<unsigned> register_pool(num_register);
    std::iota(register_pool.begin(), register_pool.end(), 1);
    /* if map to a physical register, the num is positive
     * else the num is negative
     */
    std::map<unsigned, unsigned> virtual_map;
    int spill_cnt = 0;
    for (auto& i : intervals) {
        Expire(active, register_pool, i);
        if (register_pool.empty()) {
            /* choose the interval with last endpoint to spill */
            Interval spill = active.back();
            if (spill.end > i.end) {
                active.pop_back();
                
                AddToActive(active, i);
                virtual_map[spill.id] = -(++spill_cnt);
            } else {
                virtual_map[i.id] = -(++spill_cnt);
            }
        } else {
            unsigned regi = AllocaRegister(active, register_pool, i);
            virtual_map.emplace(i.id, regi);
        }
    }
    
    for (auto i : virtual_map) 
        std::cout << i.first << " " << i.second << std::endl;
    
    return res;
}
} //end namespace ldra

int main()
{
    using namespace lsra;
    std::vector<Inst> instructions;
    std::ifstream is("input.ll");
    std::string line;

    while (std::getline(is, line)) {
        instructions.push_back(line);
    }

    auto res = LSRA(instructions, 4);
    int cnt = 0;

    //for (auto& i : res) {
    //    std::cout << cnt++ << i << std::endl;
    //}
    return 0;
}
