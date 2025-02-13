#ifndef LYDIASYFT_STRING_UTILITIES_H
#define LYDIASYFT_STRING_UTILITIES_H

#include <string>
#include <vector>


namespace Syft {

    std::vector<std::string> split(const std::string& str, const std::string& delimiter = " ");
    std::string trim(const std::string& str);
    std::string to_lower_copy(const std::string& str);
    std::string to_upper_copy(const std::string& str);
}

#endif //LYDIASYFT_STRING_UTILITIES_H