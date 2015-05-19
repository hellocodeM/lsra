#include <cstdio>
#include <vector>
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

/**
 * @param instructions: stream of instructions
 * @param num_register: number of register to use, except for special registers 
 * @return : instructions after register allocation
 */
std::vector<std::string>
LSRA(std::vector<Inst>& instructions, int num_register) {
    std::vector<Interval> intervals = LiveAnalysis(instructions);
    /* the interval has already sorted by start-point */
    
    return instructions;
}

}

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

    for (auto& i : res) {
        std::cout << cnt++ << i << std::endl;
    }
    return 0;
}
