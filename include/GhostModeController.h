#include "GhostMode.h"

#include "GhostMode.h"

#ifndef GHOSTMODECONTROLLER_H
#define GHOSTMODECONTROLLER_H

class GhostModeController
{
public:
    static GhostModeController *GetInstance()
    {
        static GhostModeController instance;
        return &instance;
    }

    GhostMode GetMode()
    {
        return mode;
    }

    void Update(float deltaTime)
    {
        timer += deltaTime;
        if (timer >= timeLimit)
        {
            if (mode == GhostMode::SCATTER)
            {
                Chase();
            }
            else
            {
                Scatter();
            }
        }
    }

private:
    GhostMode mode;
    float timer;
    float timeLimit;

private:
    GhostModeController() : mode(GhostMode::SCATTER), timer(0), timeLimit(7) {}
    ~GhostModeController() {}

    GhostModeController(const GhostModeController &) = delete;            // Prevent copy-construction
    GhostModeController &operator=(const GhostModeController &) = delete; // Prevent assignment

    void Chase()
    {
        mode = GhostMode::CHASE;
        timeLimit = 20;
        timer = 0;
    }
    void Scatter()
    {
        mode = GhostMode::SCATTER;
        timeLimit = 7;
        timer = 0;
    }
};

#endif // GHOSTMODECONTROLLER_H
