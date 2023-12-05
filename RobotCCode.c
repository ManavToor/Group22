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

	// Main loop for menu screen, this is where the user will
	// select which candies they want
	int counter = 0; // Keeps track of index value in MENU[] and COLOURS[]
	while(!getButtonPress(buttonEnter) && time1[T1] < 120000)
	{
		string topping = (string)menu_items[counter];
		displayCenteredBigTextLine(8, topping);
		
		// if a candy is selected, display the word
		// selected underneath so the user knows
		if(selected[counter])
		{
			displayCenteredTextLine(10, "Selected");
		} else
		{
			displayTextLine(10, "");
		}


		// down button to move to next candy, up
		// button to move to previous
		if(getButtonPress(buttonDown))
		{
			wait1Msec(500);
			counter++;
		} else if (getButtonPress(buttonUp))
		{
			wait1Msec(500);
			counter--;
		}
		
		// to not go out of the array's index value
		if (counter > 3)
		{
			counter = 0;
		} else if(counter < 0)
		{
			counter = 3;
		}

		// right button to select/deselect
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

	// Display all selected candies before returning
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
	SensorType[ULTRASONIC] = sensorEV3_Ultrasonic;
	SensorType[COLOUR] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[COLOUR] = modeEV3Color_Color;
	wait1Msec(50);
}

// slowly speeds-up/slows-down the robot until it either
// reaches cruising speed (or comes to a stop)
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

// moves the robot until an inputed colour is detected
// Takes in index value of colour in COLOURS[]
void moveUntilColourDetected(int speed, int colour_index)
{
	smoothStartStop(true, speed);
	while(SensorValue[COLOUR] != COLOURS[colour_index])
	{
		
		// if the next colour in COLOURS[] is detected (aka
		// the robot overshot the colour), stop, throw error
		if (SensorValue[COLOUR] == COLOURS[colour_index+1] && colour_index != 3)
		{
			eraseDisplay();
			displayBigTextLine(2, "Colour error");
			displayBigTextLine(4, "%d", SensorValue[COLOUR]); // Debugging
			wait1Msec(3000);
			error = true;
			break;
		}
	}
	smoothStartStop(false, speed);
	motor[motorA]=motor[motorB]=0;
}

// raise arm releasing candy for 2s
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

// combines raiseArm and moveUntilColourDetected
// into one function
void dispenseToppings(bool* selected)
{
	nMotorEncoder[motorB] = 0;

	// for every selected candy, get
	// that candy
	for (int i = 0; i <= 3; i++)
	{
		if(selected[i])
		{
			moveUntilColourDetected(10, i);
			if(error)
			{
				return;
			}
			wait1Msec(500);
			raiseArm();
		}
	}
	
	// return to original position
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
		wait1Msec(100);
	}
	motor[motorA]=motor[motorB]=0;
}

// sets all values in selected[] to false
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
	configureSensors();

	// array to ho hold values for which candies were selected
	bool selected[4] = {false, false, false, false};

	time1[T1] = 0; // Reset timer
	
	// Main code, will keep repeating until the bot is idle 
	// for two minutes or an error occurs
	while(time1[T1] < 120000 && error == false)
	{
		eraseDisplay();
		
		// Takes care of selecting candies
		displayMenu(MENU, selected);
		time1[T1] = 0;
		eraseDisplay();

		// If no cup is placed withing 30s an error will
		// occur
		displayCenteredBigTextLine(2, "Please place");
		displayCenteredBigTextLine(4, "your cup");
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
		eraseDisplay();
		displayCenteredBigTextLine(2, "Dispensing");
		
		// Dispense toppings and return to original spot
		dispenseToppings(selected);
		playSound(soundUpwardTones);
		reset(selected);
		time1[T1] = 0;
	}
	
	// If an error occurs, return to original position
	// before terminating
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
