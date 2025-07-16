#pragma once
#include "pch.h"

class IApplication
{
public:
    virtual void Initialize() = 0;
    virtual void Destroy() = 0;
};