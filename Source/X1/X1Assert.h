#pragma once
#include "CoreMinimal.h"
#include "Logging/StructuredLog.h"
#include <format>
#include <functional>
DECLARE_LOG_CATEGORY_EXTERN(LogX1Assert, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogX1, Log, All);

enum class _X1LogType
{
    Assert,
    Log,
};

struct _X1LogExtras
{
    _X1LogType type = {};
    bool bRepeat = false;
};

// Internal class, DO NOT USE!
// Use assertion macros instead.
class _X1AssertionHelper
{
public:
    static void Log(_X1LogExtras type,
                    const char *file,
                    int line,
                    const char *cond,
                    const FString &userMessage = {});

    template <typename... T>
    static FString BuildFormattedString(const FString::ElementType *fmt,
                                        T &&...args)
    {
        TArray<FStringFormatArg> a = {FStringFormatArg(args)...};
        return FString::Format(fmt, a);
    }

    static void LogToScreenAndConsole(const _X1LogExtras &extras,
                                      const FString &message,
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
            _X1AssertionHelper::Log(                                           \
                type, __FILE__, __LINE__, #cond, TEXT(msg));                   \
            ret;                                                               \
        }                                                                      \
    }
#define _X1_ASSERT_FMT2(type, ret, cond, fmt, ...)                             \
    {                                                                          \
        if (!(cond))                                                           \
        {                                                                      \
            _X1AssertionHelper::Log(type,                                      \
                                    __FILE__,                                  \
                                    __LINE__,                                  \
                                    #cond,                                     \
                                    _X1AssertionHelper::BuildFormattedString(  \
                                        TEXT(fmt), __VA_ARGS__));              \
            ret;                                                               \
        }                                                                      \
    }

#define _X1_ASSERT_GET_OVLD(_1, _2, _3, _4, _5, NAME, ...) NAME
#define _X1_ASSERT(ex, ret, ...)                                               \
    _X1_ASSERT_GET_OVLD(__VA_ARGS__,                                           \
                        _X1_ASSERT_FMT2,                                       \
                        _X1_ASSERT_FMT2,                                       \
                        _X1_ASSERT_FMT2,                                       \
                        _X1_ASSERT_FMT1,                                       \
                        _X1_ASSERT_FMT0)                                       \
    (ex, ret, __VA_ARGS__)

// clang-format off
#define _X1_LOGEXTRA_AS _X1LogExtras(_X1LogType::Assert)
#define _X1_LOGEXTRA_LG _X1LogExtras(_X1LogType::Log)
#define _X1_LOGEXTRA_LR _X1LogExtras(_X1LogType::Log, true)
#define _X1_RETURN_RV return
#define _X1_RETURN_RE return {}
#define _X1_RETURN_CO continue
#define _X1_RETURN_PA ;


// 当条件不满足时，在屏幕和Log输出消息，并从当前函数返回 void（不会导致程序崩溃）。
// 示例语法：
// (1) (Foo>0)
// (2) (Foo>0, "Foo must be greater than 0!")
// (3) (Foo>0, "Foo={0}. Foo must be greater than 0!", Foo)
#define X1_ASSERT_RET_VOID(...)     _X1_ASSERT(_X1_LOGEXTRA_AS, _X1_RETURN_RV, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并从当前函数返回 {}（不会导致程序崩溃）。
// 示例语法：
// (1) (Foo>0)
// (2) (Foo>0, "Foo must be greater than 0!")
// (3) (Foo>0, "Foo={0}. Foo must be greater than 0!", Foo)
#define X1_ASSERT_RET_EMPTY(...)    _X1_ASSERT(_X1_LOGEXTRA_AS, _X1_RETURN_RE, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并 continue （不会导致程序崩溃）。
// 示例语法：
// (1) (Foo>0)
// (2) (Foo>0, "Foo must be greater than 0!")
// (3) (Foo>0, "Foo={0}. Foo must be greater than 0!", Foo)
#define X1_ASSERT_CONTINUE(...)     _X1_ASSERT(_X1_LOGEXTRA_AS, _X1_RETURN_CO, __VA_ARGS__)
// 当条件不满足时，在屏幕和Log输出消息，并继续执行（不会导致程序崩溃）。
// 示例语法：
// (1) (Foo>0)
// (2) (Foo>0, "Foo must be greater than 0!")
// (3) (Foo>0, "Foo={0}. Foo must be greater than 0!", Foo)
#define X1_ASSERT_PASS(...)         _X1_ASSERT(_X1_LOGEXTRA_AS, _X1_RETURN_PA, __VA_ARGS__)
// 在屏幕和Log输出消息。重复消息不会显示。
// 示例语法：
// (1) ("Hello world!")
// (2) ("Hello world! Foo={0}!", Foo)
#define X1_LOG(...)            _X1_ASSERT(_X1_LOGEXTRA_LG, _X1_RETURN_PA, 0, __VA_ARGS__)
// 在屏幕和Log输出消息。重复消息会显示。
// 示例语法：
// (1) ("Hello world!")
// (2) ("Hello world! Foo={0}!", Foo)
#define X1_LOG_REPEAT(...)     _X1_ASSERT(_X1_LOGEXTRA_LR, _X1_RETURN_PA, 0, __VA_ARGS__)
// clang-format on
