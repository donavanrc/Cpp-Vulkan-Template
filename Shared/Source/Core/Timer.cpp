#include "Timer.h"

Timer::Timer()
{
    Reset();
}

void Timer::Reset()
{
    m_LastTime = std::chrono::steady_clock::now();
    m_TotalTime = 0.0f;
}

void Timer::Tick()
{
    auto CurrentTime = std::chrono::steady_clock::now();
    m_DeltaTime = CurrentTime - m_LastTime;
    m_LastTime = CurrentTime;
    m_TotalTime += m_DeltaTime.count();
}
