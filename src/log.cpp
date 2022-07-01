#include "log.h"

void LOG(LogLevel level, const char *msg)
{
    LogSystem::getInstance()->printLog(level, msg);
}

LogSystem::LogSystem()
{
    mLogList = new LogError;
    mLogList->next = new LogInfo;
}

LogSystem::~LogSystem()
{
    LogBase *temp;
    while(mLogList != nullptr) {
        temp = mLogList;
        mLogList = mLogList->next;
        delete temp;
    }
}

void LogSystem::printLog(LogLevel level, const char *msg)
{
    for (LogBase *temp = mLogList; temp != nullptr; temp=temp->next){
        if(temp->isMatch(level)) {
            temp->printLog(msg);
            break;
        }
    }
}
