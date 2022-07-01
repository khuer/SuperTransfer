#ifndef _LOG_H
#define _LOG_H

#include <iostream>
#include <forward_list>
#include <stdarg.h>
#include <stdio.h>

#define __DEBUG 1

#ifdef __DEBUG
    #define MYLOGI(format, ...) \
        printf("[INFO] [%s:%d:%s] " format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
    #define MYLOGE(format, ...) \
        printf("[ERROR][%s:%d:%s] " format "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
    #define MYLOGI(format, ...)
    #define MYLOGI(format, ...)
#endif

enum class LogLevel
{
    Info = 0,
    Error,
};

void LOG(LogLevel level, const char *msg);

class LogBase
{
public:
    LogBase(LogLevel level) : next(nullptr),mLevel(level) {}
    virtual void printLog(const char *msg) = 0;
    // void print(const char *format, ... )
    // {
    //     char buf[LOG_MAX_LENGTH] = {0};
    //     va_list args;                   //定义一个va_list类型的变量，用来储存单个参数  
    //     va_start(args, format);         //使args指向可变参数的第一个参数  
    //     sprintf(buf, format, args);     //必须用vprintf等带V的  
    //     va_end(args);                   //结束可变参数的获取

    //     cout << buf << endl;
    // }
    bool isMatch(LogLevel level)
    {
        return (level == mLevel);
    }

    LogBase *next;

private:
    LogLevel mLevel;
};

class LogInfo : public LogBase
{
public:
    LogInfo() : LogBase(LogLevel::Info) {}
    virtual void printLog(const char *msg) override
    {
        std::cout << "[INFO] " << msg << std::endl;
    }
};

class LogError : public LogBase
{
public:
    LogError() : LogBase(LogLevel::Error) {}
    virtual void printLog(const char *msg) override
    {
        std::cout << "[Error] " << msg << std::endl;
    }
};

class LogSystem
{
public:
    static LogSystem* getInstance()
    {
        static LogSystem logSystem;
        return &logSystem;
    }
    ~LogSystem();
    void printLog(LogLevel level, const char *msg);

private:
    LogSystem();
    LogBase *mLogList;
};

#endif
