#ifndef COMPILER_MACROS_H
#define COMPILER_MACROS_H

// Check for MSVC (Microsoft Visual C++)
#if defined(_MSC_VER)
    #define DISABLE_OPTIMIZATION __pragma(optimize("", off))
    #define RESTORE_OPTIMIZATION __pragma(optimize("", on))

// Check for GCC or Clang
#elif defined(__GNUC__) || defined(__clang__)
    // GCC and Clang use a pragma with push/pop to save and restore settings
    #define DISABLE_OPTIMIZATION \
        _Pragma("GCC push_options") \
        _Pragma("GCC optimize (\"O0\")")
    #define RESTORE_OPTIMIZATION _Pragma("GCC pop_options")

// Fallback for other compilers
#else
    #define DISABLE_OPTIMIZATION
    #define RESTORE_OPTIMIZATION

#endif

#endif // COMPILER_MACROS_H