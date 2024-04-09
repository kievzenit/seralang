#include "log_error.h"

void utils::log_error(std::string &error) {
    std::cout << error << std::endl;
    std::exit(-1);
}