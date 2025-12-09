#pragma once
#include <stdexcept>
#include <string>

namespace isd{

struct ISDException: std::runtime_error {
    explicit ISDException(const std::string& m): std::runtime_error(m) {}
};

} 
