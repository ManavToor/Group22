const string MENU[4] = {"M&M's", "Sprinkles", "Gummy Worms", "Sour Candy"};
//bool selected[4] = {false, false, false, false};
const int COLOURS[4] = {colorRed, colorBlue, colorYellow, colorGreen};

short COLOUR = S1;
short ULTRASONIC = S2;

void displayMenu(string* menu_items, bool* selected)
{
	displayCenteredBigTextLine(0, "Choose your");
	displayCenteredBigTextLine(2, "Toppings");
	displayTextLine(4, "Use Up/Down to navigate, right to select");

	int counter = 0;
	while(!getButtonPress(buttonEnter))
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

void moveUntilColourDetected(int speed, int colour)
{
	for(int i = 0; i <= speed; i++)
	{
		motor[motorA]=motor[motorB]=i;
		wait1Msec(100);
	}
	while(SensorValue[COLOUR] != colour)
	{}
	for(int i = speed; i >= 0; i--)
	{
		motor[motorA]=motor[motorB]=i;
		wait1Msec(10);
	}
	motor[motorA]=motor[motorB]=0;
}

void raiseArm()
{
	nMotorEncoder[motorC] = 0;
	while(nMotorEncoder[motorC] > -3)
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
			moveUntilColourDetected(10, COLOURS[i]);
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
	for(int i = 10; i > 0; i--)
	{
		motor[motorA]=motor[motorB]=-i;
		wait1Msec(10);
	}
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
	while(time1[T1] < 120000)
	{
		eraseDisplay();
		configureSensors();
		displayMenu(MENU, selected);
		time1[T1] = 0;

		eraseDisplay();
		displayCenteredBigTextLine(2, "Please place");
		displayCenteredBigTextLine(4, "your cup");

		bool detected = true;
		time1[T2] = 0;
		while(SensorValue[ULTRASONIC] > 15)
		{
			if(time1[T2] > 30000)
			{
				detected = false;
			}
		}
		if (!detected)
		{
			continue;
		}

		eraseDisplay();
		displayCenteredBigTextLine(2, "Dispensing");

		dispenseToppings(selected);
		playSound(soundUpwardTones);
		reset(selected);
		time1[T1] = 0;
	}
}
