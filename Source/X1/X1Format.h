#pragma once
#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"
#include <format>
#include <functional>

template <typename T> FString DefaultFmt(const T &Data)
{
    return FString::Format(TEXT("{0}"), {FStringFormatArg(Data)});
}

template <typename T> FString DbgFmt(const T &Data)
{
    return DefaultFmt(Data);
}

inline FString DbgFmt(const FName &Data)
{
    return DefaultFmt(Data.ToString());
}

template <typename T> FString DbgFmt(const TSet<T> &Set)
{
    FString String = "[";
    bool bFirst = true;
    for (const T &Element : Set)
    {
        if (bFirst)
            String += DbgFmt(Element);
        else
        {
            String += ", ";
            String += DbgFmt(Element);
        }

        bFirst = false;
    }
    String += "]";
    return String;
}