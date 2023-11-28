const string MENU[4] = {"M&M's", "Sprinkles", "Gummy Worms", "Sour Candy"};
const int COLOURS[4] = {colorRed, colorBlue, colorYellow, colorGreen};

bool error = false;

const short COLOUR = S1;
const short ULTRASONIC = S2;

void displayMenu(string* menu_items, bool* selected)
{
	displayCenteredBigTextLine(0, "Choose your");
	displayCenteredBigTextLine(2, "Toppings");
	displayTextLine(4, "Use Up/Down to navigate, right to select");

	int counter = 0;
	while(!getButtonPress(buttonEnter) && time1[T1] < 120000)
	{
		string topping = (string)menu_items[counter];
		displayCenteredBigTextLine(8, topping);
		if(selected[counter])
		{
			displayCenteredTextLine(10, "Selected");
		} else
		{
			displayTextLine(10, "");
		}

		if(getButtonPress(buttonDown))
		{
			wait1Msec(500);
			counter++;
		} else if (getButtonPress(buttonUp))
		{
			wait1Msec(500);
			counter--;
		}
		if (counter > 3)
		{
			counter = 0;
		} else if(counter < 0)
		{
			counter = 3;
		}

		if (getButtonPress(buttonRight))
		{
			if (!selected[counter])
			{
				playTone(540, 10);
				selected[counter] = true;
			} else
			{
				playTone(340, 10);
				selected[counter] = false;
			}
			wait1Msec(500);
		}
	}

	eraseDisplay();
	displayCenteredBigTextLine(0, "Items Selected");

	for (int i = 0; i <= 3; i++)
	{
		if(selected[i])
		{
			string topping = (string)menu_items[i];
			displayCenteredTextLine(i+3, topping);
		}
	}

	playSound(soundFastUpwardTones);
	wait1Msec(1000);
}

void configureSensors()
{
	SensorType[S2] = sensorEV3_Ultrasonic;
	SensorType[S3] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Color_Color;
	wait1Msec(50);
}

void smoothStartStop(bool direction = true, int speed)
{
	if (direction)
	{
		for(int i = 0; i <= speed; i++)
		{
			motor[motorA]=motor[motorB]=i;
			wait1Msec(100);
		}
	} else
	{
		for(int i = speed; i >= 0; i--)
		{
			motor[motorA]=motor[motorB]=i;
			wait1Msec(10);
		}
		motor[motorA]=motor[motorB]=0;
	}
}

void moveUntilColourDetected(int speed, int colour_index)
{
	smoothStartStop(true, speed);
	while(SensorValue[COLOUR] != COLOURS[colour_index])
	{
		/*if (SensorValue[COLOUR] == COLOURS[colour_index+1] && colour_index != 4)
		{
			eraseDisplay();
			displayBigTextLine(2, "Colour error");
			displayBigTextLine(4, "%d", SensorValue[COLOUR]); // Debugging
			wait1Msec(3000);
			error = true;
			break;
		}*/
	}
	smoothStartStop(false, speed);
	motor[motorA]=motor[motorB]=0;
}

void raiseArm()
{
	nMotorEncoder[motorC] = 0;
	while(nMotorEncoder[motorC] > -75)
	{
		motor[motorC] = -3;
	}
	wait1Msec(2000);
	while(nMotorEncoder[motorC] < 0)
	{
		motor[motorC] = 2;
	}
	motor[motorC] = 0;
}

void dispenseToppings(bool* selected)
{
	nMotorEncoder[motorB] = 0;

	for (int i = 0; i <= 3; i++)
	{
		if(selected[i])
		{
			moveUntilColourDetected(10, i);
			wait1Msec(500);
			raiseArm();
		}
	}
	for(int i = 0; i <= 10; i++)
	{
		motor[motorA]=motor[motorB]=-i;
		wait1Msec(100);
	}
	while(nMotorEncoder[motorB] > 0)
	{}
	smoothStartStop(false, 10);
	motor[motorA]=motor[motorB]=0;
}

bool* reset(bool* selected)
{
	for (int i = 0; i <= 3; i++)
	{
		selected[i] = false;
	}
	return selected;
}

task main()
{
	bool selected[4] = {false, false, false, false};

	time1[T1] = 0;
	while(time1[T1] < 120000 && error == false)
	{
		eraseDisplay();
		//displayString(8, "%d %d", SensorValue[COLOUR], time1[T1]); //DEBUG
		configureSensors();
		displayMenu(MENU, selected);
		time1[T1] = 0;
		//displayString(8, "%d %d", SensorValue[COLOUR], time1[T1]); //DEBUG
		eraseDisplay();
		displayCenteredBigTextLine(2, "Please place");
		displayCenteredBigTextLine(4, "your cup");
		//displayString(8, "%d %d", SensorValue[COLOUR], time1[T1]); //DEBUG
		time1[T2] = 0;
		while(SensorValue[ULTRASONIC] > 15)
		{
			if(time1[T2] > 30000)
			{
        eraseDisplay();
				displayCenteredBigTextLine(2, "No cup error");
				wait1Msec(3000);
				error = true;
			}
		}
		//displayString(8, "%d %d", SensorValue[COLOUR], time1[T1]); //DEBUG
		eraseDisplay();
		displayCenteredBigTextLine(2, "Dispensing");
		//displayString(8, "%d %d", SensorValue[COLOUR], time1[T1]); //DEBUG
		dispenseToppings(selected);
		playSound(soundUpwardTones);
		reset(selected);
		time1[T1] = 0;
	}
	if (error == true)
	{
		for(int i = 0; i <= 10; i++)
		{
			motor[motorA]=motor[motorB]=-i;
			wait1Msec(100);
		}
		while(nMotorEncoder[motorB] > 0)
		{}
		smoothStartStop(false, 10);
		motor[motorA]=motor[motorB]=0;
	}
	return;
}
