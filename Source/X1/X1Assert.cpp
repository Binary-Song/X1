#include "X1Assert.h"
#include "Engine/Engine.h"
#include "Logging/LogVerbosity.h"
#include <format>
#include <map>
#include <string>
DEFINE_LOG_CATEGORY(LogAssert);
thread_local int nextId = 0;

void _X1AssertionHelper::Log(_X1LogExtras type, const char *file, int line,
                             const char *cond, const FString &userMessage)
{
    const FString message = [&]() {
        FString message; 
        message.Reserve(userMessage.Len() + strlen(file) + 30);
        if (type.type == _X1LogType::Assert)
            message.Append("Assert! ");
        else
            message.Append("Log ");
        if (userMessage.Len() > 0)
        {
            message.Append("Msg=");
            message.Append(userMessage);
            message.Append(", ");
        }
        if (type.type == _X1LogType::Assert)
        {
            message.Append("Cond=");
            message.Append(cond);
            message.Append(", ");
        }
        message.Append("Src=");
        message.Append(file);
        message.Append(":");
        message.Append(FString::FromInt(line));
        return message;
    }();
    _X1AssertionHelper::LogToScreenAndConsole(message, type.bRepeat);
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

void _X1AssertionHelper::LogToScreenAndConsole(const FString &message,
                                               bool bRepeatable)
{
    thread_local MessageDeduplicate md;
    bool bNew = false;
    int id = md.getId(message, bNew);
    if (bRepeatable)
    {
        id = -1;
        bNew = true;
    }

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(id, 20.f, FColor::Red, message);
    if (bNew)
        UE_LOG(LogAssert, Error, TEXT("%s"), *message);
}