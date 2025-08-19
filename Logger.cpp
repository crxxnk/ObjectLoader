#include "Logger.h"

Logger::Logger(const std::string &file)
{
    logfile.open(file, std::ios::out | std::ios::app);
    if(!logfile.is_open())
        throw std::runtime_error("Could not open log file.");
}

std::string Logger::getTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Logger::levelToString(Level level)
{
    switch (level)
    {
        case INFO: return "INFO";
        case WARNING: return "WARNING";
        case ERROR: return "ERROR";
        case DEBUG: return "DEBUG";
        case NONE: return "";
        default: return "UNKNOWN";
    }
}

Logger &Logger::getInstance(const std::string &file)
{
    static Logger instance(file);
    return instance;
}

void Logger::log(const std::string &message, Level level)
{
    std::lock_guard<std::mutex> guard(logMutex);
    std::string logEntry;

    if(level != NONE)
        logEntry = "[" + getTimestamp() + "] [" + levelToString(level) + "] " + message;
    else
        logEntry = message;
    
    if(level == ERROR)
        errors++;
    else if(level == WARNING)
        warnings++;
    else if(level == DEBUG)
        debugMessages++;
    else if(level == INFO)
        infoMessages++;

    std::cout << logEntry << std::endl;
    
    logfile << logEntry << std::endl;
}

void Logger::logFinish()
{
    log("Compilation finished with " + std::to_string(getErrors()) + " errors, " + std::to_string(getWarnings()) + " warnings, "
    + std::to_string(getDebugMessages()) + " debug messages, " + std::to_string(getInfoMessages()) + " info messages.", NONE);
}

Logger::~Logger()
{
    if (logfile.is_open())
        logfile.close();
}