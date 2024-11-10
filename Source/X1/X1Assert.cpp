#include "X1Assert.h"
#include "Engine/Engine.h"
#include "Logging/LogVerbosity.h"
#include <format>
#include <map>
#include <string>
DEFINE_LOG_CATEGORY(LogAssert);
thread_local std::map<FString, int> codes;
thread_local int nextId = 0;

void _X1AssertionHelper::X1AssertionFailed_WithCustomLog(
    const char *file, int line, const char *cond, const FString &userMessage,
    const std::function<void(const FString &)> &logFunc)
{
    FString src;
    src.Reserve(strlen(file) + 6);
    src += file;
    src += ":";
    src += FString::FromInt(line);

    int id{};
    const bool isNew = !codes.contains(src);
    if (isNew)
        codes[src] = nextId++;
    id = nextId - 1;

    FString message;
    message.Reserve(userMessage.Len() + strlen(file) + 30);
    message.Append("Assert! Msg=");
    message.Append(userMessage);
    message.Append(", Cond=");
    message.Append(cond);
    message.Append(", Src=");
    message.Append(src);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(id, 20.f, FColor::Red, message);
    if (isNew)
    {
        logFunc(message);
    }
}

void _X1AssertionHelper::X1AssertionFailed(const char *file, int line,
                                           const char *cond,
                                           const FString &message)
{

    _X1AssertionHelper::X1AssertionFailed_WithCustomLog(
        file, line, cond, message, [&](const FString &message) {
            UE_LOG(LogAssert, Error, TEXT("%s"), *message);
        });
}
