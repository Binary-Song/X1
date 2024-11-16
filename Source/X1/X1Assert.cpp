#include "X1Assert.h"
#include "Engine/Engine.h"
#include "Logging/LogVerbosity.h"
#include <format>
#include <map>
#include <string>
DEFINE_LOG_CATEGORY(LogAssert);
thread_local int nextId = 0;

void _X1AssertionHelper::X1AssertionFailed_WithCustomLog(
    const char *file, int line, const char *cond, const FString &userMessage,
    const std::function<void(const FString &)> &logFunc)
{
}

void _X1AssertionHelper::X1AssertionFailed(const char *file, int line,
                                           const char *cond,
                                           const FString &userMessage)
{
    FString message;
    message.Reserve(userMessage.Len() + strlen(file) + 30);
    message.Append("Assert! ");
    if (userMessage.Len() > 0)
    {
        message.Append("Msg=");
        message.Append(userMessage);
        message.Append(", ");
    }
    message.Append("Cond=");
    message.Append(cond);
    message.Append(", Src=");
    message += file;
    message += ":";
    message += FString::FromInt(line);
    _X1AssertionHelper::LogToScreenAndConsole(message);
}

struct MessageDeduplicate
{
    int getId(const FString &msg, bool &bNew)
    {
        auto it = m_map.find(msg);
        bNew = it == m_map.end();
        if (bNew)
        {
            m_map[msg] = m_id;
            m_id++;
            return m_id - 1;
        }
        return it->second;
    }

private:
    int m_id = 1;
    std::map<FString, int> m_map;
};

void _X1AssertionHelper::LogToScreenAndConsole(const FString &message)
{
    thread_local MessageDeduplicate md;
    bool bNew = false;
    if (GEngine)
        GEngine->AddOnScreenDebugMessage(md.getId(message, bNew), 20.f,
                                         FColor::Red, message);
    if (bNew)
        UE_LOG(LogAssert, Error, TEXT("%s"), *message);
}