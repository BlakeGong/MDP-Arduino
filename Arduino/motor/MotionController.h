#ifndef MOTIONCONTROLLER_H
#define MOTIONCONTROLLER_H

#include "Arduino.h"
#include <PID_v1.h>
#include <DualVNH5019MotorShield.h>

class MotionController {
public:
    MotionController();
    void executeCommand(String command);
    void initPid();
    static void M1CountInc();
    static void M2CountInc();
    void moveForward(long);
    void moveForwardGrids(long);
    void turnLeft();
    void turnRight();

private:
    static const bool isDebug = false;
    static const double kp = 0.5, ki = 1, kd = 0;
    static volatile long M1Count, M2Count;
    static const int setSpeed = 300;
    DualVNH5019MotorShield motorShield;
    double SetpointLeft, InputLeft, OutputLeft;
    double SetpointRight, InputRight, OutputRight;
    PID* pidLeft;
    PID* pidRight;
    void updatePid();
    void printInOut();
    void printCounts();
};

#endif