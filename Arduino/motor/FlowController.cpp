#include <FlowController.h>
#include <Constants.h>

FlowController::FlowController(MotionController* motionController, SensorController* sensorController) {
	this->motionController = motionController;
	this->sensorController = sensorController;
}

void FlowController::executeCommand() {
	while (state == FlowController::executeCommandState) {
		int count = 0, amount = 0;
		unsigned char movement = 0;
		while (command[count] != 10 && command[count] != 0) {
			if (command[count] >= 48 && command[count] <= 57)
				amount = amount * 10 + command[count] - 48;
			else
				movement = command[count];

			if ((command[count + 1] >= 'A' && command[count + 1] <= 'Z') || (command[count + 1] == 10 || command[count + 1] == 0)) {
				motionController->resetCounts();
				switch (movement) {
				case 'F':
					motionController->moveForwardGrids(amount);
					break;
				case 'B':
					motionController->moveBackwardGrids(amount);
					break;
				case 'C':
					for (int i = 0; i < amount; i++)
						motionController->turn(true);
					break;
				case 'A':
					for (int i = 0; i < amount; i++)
						motionController->turn(false);
					break;
				case 'Z':
					motionController->calibratePos(amount);
					break;
				}
				amount = 0;
			}

			// Serial.print("X: ");
			// Serial.print(motionController->getPosX());
			// Serial.print("Y: ");
			// Serial.print(motionController->getPosY());
			// Serial.println();

			count++;
		}

		state = FlowController::writeSerialState;
	}
}

void FlowController::executeFastRun() {

}

void FlowController::fetchSerial() {
	while (state == FlowController::fetchSerialState) {
		while (!Serial);
		if (Serial.available() > 0) {
			command = Serial.readString();
			Serial.flush();

			state = FlowController::executeCommandState;
		}
	}
}

void FlowController::finish() {

}

void FlowController::startFSM() {
	while (1) {
		switch (state) {
		case FlowController::executeCommandState:
			this->executeCommand();
			break;
		case FlowController::executeFastRunState:
			this->executeFastRun();
			break;
		case FlowController::fetchSerialState:
			this->fetchSerial();
			break;
		case FlowController::finishState:
			this->finish();
			break;
		case FlowController::testState:
			this->test();
			break;
		case FlowController::waitForFastRunState:
			this->waitForFastRun();
			break;
		case FlowController::waitForStartState:
			this->waitForStart();
			break;
		case FlowController::warmUpState:
			this->warmUp();
			break;
		case FlowController::writeSerialState:
			this->writeSerial();
			break;
		}
	}
}

void FlowController::test() {
	// motionController->calibratePos(1);
	// sensorController->printSensorFeedbackCalibration();
	Serial.print("UL Left Reading: ");
	Serial.println(sensorController->getUlCM(Constants::UL_LEFT_PWM, Constants::UL_LEFT_TRIG));

	Serial.print("UL Right Reading: ");
	Serial.println(sensorController->getUlCM(Constants::UL_RIGHT_PWM, Constants::UL_RIGHT_TRIG));

	Serial.print("IR Short Front Middle Reading: ");
	Serial.println(sensorController->getAnalogReading(Constants::IR_SHORT_FM));
	// Serial.print("IR Long Left Reading: ");
	// Serial.println(sensorController->getIRLongCM(Constants::IR_LONG_L));
	// Serial.print("IR Short Left Reading: ");
	// Serial.println(sensorController->getIRShortCM(Constants::IR_SHORT_L));
	delay(500);
}

void FlowController::waitForFastRun() {

}

void FlowController::waitForStart() {
	while (state == FlowController::waitForStartState) {
		while (!Serial);
		if (Serial.available() > 0) {
			command = Serial.readString();
			Serial.flush();

			if (command.startsWith("S")) {
				state = FlowController::writeSerialState;
			}
		}
	}
}

void FlowController::warmUp() {

}

void FlowController::writeSerial() {
	while (state == FlowController::writeSerialState) {
		sensorController->printSensorFeedback();
		state = FlowController::fetchSerialState;
	}
}