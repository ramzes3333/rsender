#include "Logger.h"
#include <fstream>
#include <ctime>
#include <iomanip>

std::ofstream logFile("app.log", std::ios::app);

void Logger::log(const std::string& message) {
    if (!logFile.is_open()) return;

    std::time_t now = std::time(nullptr);
    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    logFile << "[" << timeStr << "] " << message << std::endl;
}
