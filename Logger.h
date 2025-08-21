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
    enum Level { INFO, WARNING, ERROR, DEBUG, NONE };
private:
    std::ofstream logfile;
    static Logger* instance;
    std::mutex logMutex;

    Logger(const std::string &file);
    std::string getTimestamp();
    std::string levelToString(Level level);

    int warnings, errors, debugMessages, infoMessages;
public:
    static Logger &getInstance(const std::string &file = "log.txt");
    void log(const std::string &message, Level level = INFO);
    void logFinish();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    int getWarnings() { return warnings; }
    int getErrors() { return errors; }
    int getDebugMessages() { return debugMessages; }
    int getInfoMessages() { return infoMessages; }
};

extern Logger &logger;