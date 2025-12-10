#pragma once
#include <stdexcept>
#include <string>

namespace isd{

struct ISDException: std::runtime_error {
    using std::runtime_error::runtime_error;
};

}
