#pragma once

#include <string>
#ifdef _WIN32
    #include <windows.h>
    #define MOD_API __declspec(dllexport)
#else
    #include <dlfcn.h>
    #define MOD_API __attribute__((visibility("default")))
#endif

#define clink extern "C" MOD_API

#define ABSTRACT 0;

extern "C" {
    class Mod {
    public:
        virtual ~Mod() {}

        virtual void initialize() = ABSTRACT;
        virtual void shutdown() = ABSTRACT;
    };

    struct ModInfo {
        std::string name;
    };
}
// Factory function to create instances of Mod
clink Mod* createMod();
clink ModInfo* getModInfo();

