#include "X1Assert.h"
#include "Engine/Engine.h"
#include "Logging/LogVerbosity.h"
#include <format>
#include <map>
#include <string>
DEFINE_LOG_CATEGORY(LogAssert);
thread_local std::map<FString, int> codes;
thread_local int nextId = 0;

void X1AssertionFailed(const char *file, int line, const char *condition, FString userMessage)
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
    message.Append(condition);
    message.Append(", Src=");
    message.Append(src);

    if (GEngine)
        GEngine->AddOnScreenDebugMessage(id, 5.f, FColor::Red, message);
    if (isNew)
        UE_LOG(LogAssert, Error, TEXT("%s"), *message);
}