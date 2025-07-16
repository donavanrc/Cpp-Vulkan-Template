#pragma once
#include "pch.h"

class IApplication
{
public:
    virtual void Init() = 0;
    virtual void Destroy() = 0;
};