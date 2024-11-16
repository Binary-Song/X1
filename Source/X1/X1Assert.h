#pragma once
#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"
#include <format>
#include <functional>
DECLARE_LOG_CATEGORY_EXTERN(LogAssert, Log, All);

enum class X1LogType
{
    Assert,
    Log,
};

struct X1LogExtras
{
    X1LogType type = {};
    bool bRepeat = false;
};

// use assertion macros instead
class _X1AssertionHelper
{
public:
    static void Log(X1LogExtras type, const char *file, int line,
                    const char *cond, const FString &userMessage = {});

    template <typename... T>
    static FString BuildFormattedString(const FString::ElementType *fmt,
                                        T &&...args)
    {
        TArray<FStringFormatArg> a = {FStringFormatArg(args)...};
        return FString::Format(fmt, a);
    }

    static void LogToScreenAndConsole(const FString &message,
                                      bool repeatable = false);
};

#define _X1_ASSERT_FMT0(type, ret, cond)                                       \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            _X1AssertionHelper::Log(type, __FILE__, __LINE__, #cond);          \
            ret;                                                               \
        }                                                                      \
    }
#define _X1_ASSERT_FMT1(type, ret, cond, msg)                                  \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            _X1AssertionHelper::Log(type, __FILE__, __LINE__, #cond,           \
                                    TEXT(msg));                                \
            ret;                                                               \
        }                                                                      \
    }
#define _X1_ASSERT_FMT2(type, ret, cond, fmt, ...)                             \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            _X1AssertionHelper::Log(type, __FILE__, __LINE__, #cond,           \
                                    _X1AssertionHelper::BuildFormattedString(  \
                                        TEXT(fmt), __VA_ARGS__));              \
            ret;                                                               \
        }                                                                      \
    }

#define _X1_ASSERT_GET_OVLD(_1, _2, _3, NAME, ...) \
    NAME
#define X1_ASSERT(type, ret, ...)                                        \
    _X1_ASSERT_GET_OVLD(__VA_ARGS__, _X1_ASSERT_FMT2, _X1_ASSERT_FMT1,         \
                        _X1_ASSERT_FMT0)                                       \
    (type, ret, __VA_ARGS__)

// clang-format off
#define X1_LOGTYPE_AS X1LogExtras(X1LogType::Assert)
#define X1_LOGTYPE_LG X1LogExtras(X1LogType::Log)
#define X1_LOGTYPE_LR X1LogExtras(X1LogType::Log, true)
#define X1_FAILACT_RV return
#define X1_FAILACT_RE return {}
#define X1_FAILACT_CO continue
#define X1_FAILACT_PA ;


// 当条件不满足时，在屏幕和Log输出消息，并从当前函数返回 void（不会导致程序崩溃）。
// 示例语法：
// (1) (Foo>0)
// (2) (Foo>0, "Foo must be greater than 0!")
// (3) (Foo>0, "Foo={0}. Foo must be greater than 0!", Foo)
#define X1_ASSERT_RET_VOID(...)     X1_ASSERT(X1_LOGTYPE_AS, X1_FAILACT_RV, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并从当前函数返回 {}（不会导致程序崩溃）。
// 示例语法：
// (1) (Foo>0)
// (2) (Foo>0, "Foo must be greater than 0!")
// (3) (Foo>0, "Foo={0}. Foo must be greater than 0!", Foo)
#define X1_ASSERT_RET_EMPTY(...)    X1_ASSERT(X1_LOGTYPE_AS, X1_FAILACT_RE, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并 continue （不会导致程序崩溃）。
// 示例语法：
// (1) (Foo>0)
// (2) (Foo>0, "Foo must be greater than 0!")
// (3) (Foo>0, "Foo={0}. Foo must be greater than 0!", Foo)
#define X1_ASSERT_CONTINUE(...)     X1_ASSERT(X1_LOGTYPE_AS, X1_FAILACT_CO, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并继续执行（不会导致程序崩溃）。
// 示例语法：
// (1) (Foo>0)
// (2) (Foo>0, "Foo must be greater than 0!")
// (3) (Foo>0, "Foo={0}. Foo must be greater than 0!", Foo)
#define X1_ASSERT_PASS(...)         X1_ASSERT(X1_LOGTYPE_AS, X1_FAILACT_PA, __VA_ARGS__)
// 在屏幕和Log输出消息。重复消息不会显示。
// 示例语法：
// (1) ("Hello world!")
// (2) ("Hello world! Foo={0}!", Foo)
#define X1_LOG(...)            X1_ASSERT(X1_LOGTYPE_LG, X1_FAILACT_PA, 0, __VA_ARGS__)
// 在屏幕和Log输出消息。重复消息会显示。
// 示例语法：
// (1) ("Hello world!")
// (2) ("Hello world! Foo={0}!", Foo)
#define X1_LOG_REPEAT(...)     X1_ASSERT(X1_LOGTYPE_LR, X1_FAILACT_PA, 0, __VA_ARGS__)
// clang-format on
