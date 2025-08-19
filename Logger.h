#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <mutex>

class Logger
{
public:
    enum Level { INFO, WARNING, ERROR, DEBUG };
private:
    std::ofstream logfile;
    static Logger* instance;
    std::mutex logMutex;

    Logger(const std::string &file);
    std::string getTimestamp();
    std::string levelToString(Level level);
public:
    static Logger &getInstance(const std::string &file = "log.txt");
    void log(const std::string &message, Level level = INFO);
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};