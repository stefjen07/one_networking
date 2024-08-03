#include <iostream>

#define INFO(message) std::cerr << "[Info] " << message << std::endl;
#define ERROR(code, message) std::cerr << "[Error] " << message << std::endl; return code;
#define DEBUG(message) std::cerr << "[Debug] " << message << std::endl;