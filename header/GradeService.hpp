#pragma once
#include "grade.hpp"
#include "exceptions.hpp"

namespace isd{

class GradeService {
public:
    static void add(Grade<int>& g,int v) {
        if (v < 0 || v > 10) throw ISDException("Оценка должна быть 0...10");
        g.vals_.push_back(v);
    }

    static void clear(Grade<int>& g) {
        g.vals_.clear();
    }
};

}
