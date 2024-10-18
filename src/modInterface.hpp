#pragma once

#include <dlfcn.h>
#define MOD_API __attribute__((visibility("default")))

#define clink extern "C" MOD_API
#define ABSTRACT 0

extern "C" {
    class Mod {
    public:
        virtual ~Mod() {}

        // Called when the mod is loaded
        virtual void Initialize() = ABSTRACT;

        // Called when the mod is unloaded
        virtual void Shutdown() = ABSTRACT;

        // Example of a mod function
        virtual void DoSomething() = ABSTRACT;
    };
}
// Factory function to create instances of IMod
clink Mod* CreateMod();
clink void DestroyMod(Mod* mod);
