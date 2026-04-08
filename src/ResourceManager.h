#pragma once
#include "raylib.h"


class ResourceManager {
public:

    ResourceManager(const ResourceManager&) = delete;
    void operator=(const ResourceManager&) = delete;

    static ResourceManager& Get() {
        static ResourceManager instance;
        return instance;
    }


    Model DIRT_MODEL;
    void init();

private:
    ResourceManager() {};

};
