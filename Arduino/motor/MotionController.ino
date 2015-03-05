#include "Arduino.h"
#include <Pin.h>
#include <PinChangeInt.h>
#include <PID_v1.h>
#include <DualVNH5019MotorShield.h>
#include <MotionController.h>

const long brakeDist = 60;
volatile long MotionController::M1Count = 0;
volatile long MotionController::M2Count = 0;

MotionController::MotionController() {
	M1Count = M2Count = 0;
	this->motorShield.init();
	initPid();
	MotionController::initInterrupts();
}

void MotionController::initInterrupts()
{
	PCintPort::attachInterrupt(Pin::M1_ENCODER_A, MotionController::M1CountInc, CHANGE);
	PCintPort::attachInterrupt(Pin::M2_ENCODER_A, MotionController::M2CountInc, CHANGE);
}

void MotionController::initPid() {
	this->pidLeft = new PID(&(this->InputLeft), &(this->OutputLeft), &(this->SetpointLeft), MotionController::kp, MotionController::ki, MotionController::kd, DIRECT);
	this->pidRight = new PID(&(this->InputRight), &(this->OutputRight), &(this->SetpointRight), MotionController::kp, MotionController::ki, MotionController::kd, DIRECT);

	this->SetpointLeft = 0;
	this->SetpointRight = 0;

	this->pidLeft->SetOutputLimits(0, 100);
	this->pidRight->SetOutputLimits(0, 100);

	this->pidLeft->SetMode(AUTOMATIC);
	this->pidRight->SetMode(AUTOMATIC);
}

void MotionController::M1CountInc() {
	MotionController::M1Count++;
}

void MotionController::M2CountInc() {
	M2Count++;
}

void MotionController::moveForward(long dist) {
	long prevCount = M1Count = M2Count = 0;
	while (MotionController::M1Count + MotionController::M2Count - 2 * prevCount < 2 * (dist - brakeDist)) {
		updatePid();
		motorShield.setSpeeds(150 + this->OutputLeft - this->OutputRight, 150 - this->OutputLeft + this->OutputRight);

		printInOut();
		printCounts();
		Serial.println();
	}
	while (MotionController::M1Count + MotionController::M2Count - 2 * prevCount < 2 * dist) {
		updatePid();
		motorShield.setBrakes(100 - this->OutputLeft + this->OutputRight, 100 + this->OutputLeft - this->OutputRight);

		printInOut();
		printCounts();
		Serial.println();
	}

	delay(300);
	Serial.print("Final readings: ");
	printCounts();
	Serial.println();
}

void MotionController::moveForwardGrids(long grids) {
	this->moveForward(grids * 576);
}

void MotionController::readCounts() {
	printCounts();
	Serial.println();
}

void MotionController::spinMotor(long speed) {
	motorShield.setSpeeds(speed, speed);
}

void MotionController::turnLeft() {
	long dist = 783;
	// long dist = 792;
	long prevCount = M1Count = M2Count = 0;
	while (MotionController::M1Count + MotionController::M2Count - 2 * prevCount < 2 * (dist - brakeDist)) {
		updatePid();
		motorShield.setSpeeds(-150 - this->OutputLeft + this->OutputRight, 150 - this->OutputLeft + this->OutputRight);

		printInOut();
		printCounts();
		Serial.println();
	}
	while (MotionController::M1Count + MotionController::M2Count - 2 * prevCount < 2 * dist) {
		updatePid();
		motorShield.setBrakes(-100 + this->OutputLeft - this->OutputRight, 100 + this->OutputLeft - this->OutputRight);

		printInOut();
		printCounts();
		Serial.println();
	}
}

void MotionController::turnRight() {
	long dist = 792;
	long prevCount = M1Count = M2Count = 0;
	while (MotionController::M1Count + MotionController::M2Count - 2 * prevCount < 2 * (dist - brakeDist)) {
		updatePid();
		motorShield.setSpeeds(150 + this->OutputLeft - this->OutputRight, -150 + this->OutputLeft - this->OutputRight);

		printInOut();
		printCounts();
		Serial.println();
	}
	while (MotionController::M1Count + MotionController::M2Count - 2 * prevCount < 2 * dist) {
		updatePid();
		motorShield.setBrakes(100 - this->OutputLeft + this->OutputRight, -100 - this->OutputLeft + this->OutputRight);

		printInOut();
		printCounts();
		Serial.println();
	}
}

void MotionController::printInOut() {
	// Serial.print("\tInputLeft: ");
	// Serial.print(this->InputLeft);
	// Serial.print("\tOutputLeft: ");
	// Serial.print(this->OutputLeft);
	// Serial.print("\tInputRight: ");
	// Serial.print(this->InputRight);
	// Serial.print("\tOutputRight: ");
	// Serial.print(this->OutputRight);
}

void MotionController::printCounts() {
	Serial.print("\tM1: ");
	Serial.print(M1Count);
	Serial.print("\tM2: ");
	Serial.print(M2Count);
}

void MotionController::updatePid() {
	this->InputLeft = MotionController::M1Count - MotionController::M2Count;
	this->InputRight = MotionController::M2Count - MotionController::M1Count;
	this->pidLeft->Compute();
	this->pidRight->Compute();
}