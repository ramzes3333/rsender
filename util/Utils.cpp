#include "Utils.h"
#include <regex>
#include <random>
#include <sstream>
#include <iomanip>
#include <ctime>

std::string Utils::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");

    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

std::string Utils::generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);

    uint32_t data[4] = { dis(gen), dis(gen), dis(gen), dis(gen) };

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    oss << std::setw(8) << data[0] << "-";
    oss << std::setw(4) << ((data[1] >> 16) & 0xFFFF) << "-";
    oss << std::setw(4) << (((data[1] >> 0) & 0x0FFF) | 0x4000) << "-";            // version 4
    oss << std::setw(4) << (((data[2] >> 16) & 0x3FFF) | 0x8000) << "-";           // variant 1
    oss << std::setw(4) << ((data[2] >> 0) & 0xFFFF);
    oss << std::setw(8) << data[3];

    return oss.str();
}

std::string Utils::getCurrentDateTime() {
    std::time_t t = std::time(nullptr);
    std::tm localTm;
#ifdef _WIN32
    localtime_s(&localTm, &t);
#else
    localtime_r(&t, &localTm);
#endif

    char buf[25]; // "YYYY-MM-DDTHH:MM:SS.000"
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S.000", &localTm);
    return std::string(buf);
}

std::string Utils::getInputLineText(TInputLine* input) {
    char buffer[256] = {};
    input->getData(buffer);
    return std::string(buffer);
}
