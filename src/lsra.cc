#include <cstdio>
#include <vector>
#include <map>
#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <algorithm>


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

/**
 * map virtual register to physical register, and spill the register at the same time.
 * for each entry of register, if the value is positive, it represents a physical regiser number,
 * if the value is negative, it means the virtual register needs to be spilled.
 */
std::vector<Inst> 
ConvertInstruction(const std::vector<Inst>& instructions, 
                   const std::map<int, int>& register_map) {
    std::vector<Inst> res;
    std::regex r("%(\\d+)");
    
    for (auto i : instructions) {
        std::string dest_str;
        std::for_each(std::sregex_token_iterator(i.begin(), i.end(), r, {-1, 0}),
                        std::sregex_token_iterator(),
                        [&](auto x) {
                            
                            if (x.str()[0] == '%') {
                                int key = std::stoi(x.str().substr(1));
                                int value = register_map.at(key);

                                if (value > 0) {
                                /* it could map to a physical register */
                                    dest_str += "%" + std::to_string(value);
                                } else {
                                    dest_str += "%" + std::to_string(value);
                                }
                            } else {
                                dest_str += x;
                            }
                        });
        if (dest_str.empty()) dest_str = i;
        res.push_back(dest_str);
    }
    return res;
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
    std::vector<Interval> active;
    std::vector<unsigned> register_pool(num_register);
    std::iota(register_pool.begin(), register_pool.end(), 1);
    /* if map to a physical register, the num is positive
     * else the num is negative
     */
    std::map<int, int> virtual_map;
    int spill_cnt = 0;
    for (auto& i : intervals) {
        /* expire */
        for (auto j = active.begin(); j != active.end(); ) {
            if (j->end >= i.start) break;
            register_pool.push_back(virtual_map[j->id]);
            j = active.erase(j);
        }
        if (register_pool.empty()) {
            /* choose the interval with last endpoint to spill */
            Interval spill = active.back();
            if (spill.end > i.end) {
                active.pop_back();
                
                AddToActive(active, i);
                virtual_map.emplace(i.id, virtual_map[spill.id]);
                virtual_map[spill.id] = -(++spill_cnt);
            } else {
                virtual_map[i.id] = -(++spill_cnt);
            }
        } else {
            unsigned regi = AllocaRegister(active, register_pool, i);
            virtual_map.emplace(i.id, regi);
        }
    }
    
    return ConvertInstruction(instructions, virtual_map);
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

    for (auto& i : res) {
        std::cout << i << std::endl;
    }
    return 0;
}
