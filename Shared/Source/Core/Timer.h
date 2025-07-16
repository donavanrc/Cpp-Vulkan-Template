#pragma once
#include "pch.h"

class Timer
{
public:
    Timer();
    ~Timer() = default;
    void Reset();
    void Tick();
    float GetDeltaTime() const { return m_DeltaTime.count(); }
    float GetTotalTime() const { return m_TotalTime; }

private:
    std::chrono::steady_clock::time_point m_LastTime;
    std::chrono::duration<float, std::milli> m_DeltaTime;
    float m_TotalTime;
};
