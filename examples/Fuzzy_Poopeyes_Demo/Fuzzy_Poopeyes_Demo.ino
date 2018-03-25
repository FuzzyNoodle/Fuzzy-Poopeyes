/*
 Name:		Fuzzy_Poopeyes_Demo.ino
 Created:	1/29/2018 9:31:13 PM
 Author:	georgychen
*/
#include "Fuzzy_Poopeyes.h"
#include "Streaming_Local.h"
#include "SerialCommandLiteUSB_Local.h""
#include <Wire.h>
#include "Adafruit_ZeroDMA_Local.h"

SerialCommand SCmd;
FuzzyOLEDDriver OLED;
FuzzyDACAudio audio;

#define IR_RECEIVER_INPUT_PIN A1
#define IR_UPDATE_INTERVAL 5 //update period in milliseconds.
#define IR_DETECTION_THRESHOLD 400
#define IR_DETECTION_MIN_COUNT 2	//specify the minimum counts of IR detection to trigger an IR activation
#define ACCUMULATE_DURATION 10
#define MAX_PUPIL_INWARD_PIXEL 30


void setup()
{
	Serial.begin(115200);
	while (!Serial) { ; }// wait for serial port to connect
	Serial << "OLDE Tester started..." << endl;

	addSerialCommands();
	printAvailableCommands();

	OLED.begin();
	audio.begin();

	Serial << "Enter '?' for available commands." << endl;
	
	//pinMode(5, OUTPUT);
	//digitalWrite(5, HIGH);
	//delay(3000);
	//motor();

	//pinMode(IR_RECEIVER_INPUT_PIN, INPUT);
	eyeRotation();
}

void loop()
{
	SCmd.readSerial();
	OLED.update();

	updateIRSensor();

}

void addSerialCommands()
{
	SCmd.addCommand("?", printAvailableCommands);
	SCmd.addCommand("on", displayOn);
	SCmd.addCommand("off", displayOff);
	SCmd.addCommand("con", setContrast);
	SCmd.addCommand("fill", fill);
	SCmd.addCommand("clear", clear);
	SCmd.addCommand("begin", beginOled);
	SCmd.addCommand("start", start);
	SCmd.addCommand("stop", stop);
	SCmd.addCommand("motor", motor);
	SCmd.addCommand("play", play);
	SCmd.addCommand("set", set);
	SCmd.addCommand("exp", setExpresssion);
	SCmd.addDefaultHandler(unrecognized);
}

void printAvailableCommands()
{
	Serial << "*********************************************************************** " << endl;
	Serial << "on/off        :turn display on/off" << endl;
	Serial << "con 255       :set contrast 255 (0~255)" << endl;
	Serial << "fill 255      :fill screen with 255(bitmap, not ascii text), 0-255 " << endl;
	Serial << "clear         :clear screen" << endl;
	Serial << "init          :re-initialize the chip " << endl;
	Serial << "start         :start auto movement/blink " << endl;
	Serial << "stop          :stop auto movement/blink " << endl;
	Serial << "motor         :run motor for a few seconds " << endl;
	Serial << "play 1        :play sound file 1 (0~9)" << endl;
	Serial << "set 0 0 0 0   :set pupil x,y; upper eyelid, lower eyelid offsets. "<< endl;
	Serial << "exp 1         :set expression: 0=neutral 1=narrow 2=cross 3=wide" << endl;
	Serial << "*********************************************************************** " << endl;
}

void motor()
{
	Serial << "Motor on" << endl;
	pinMode(5, OUTPUT);
	digitalWrite(5, HIGH);
	delay(2000);
	digitalWrite(5, LOW);


	/*
	pinMode(5, OUTPUT);
	for(uint8_t i=0;i<255;i++)
	{
		analogWrite(5, i);
		delay(5);
	}
	delay(1000);
	for (uint8_t i = 255; i>=1; i--)
	{
		analogWrite(5, i);
		delay(5);
	}
	//digitalWrite(5, HIGH);
	analogWrite(5, 0);


	*/
	//digitalWrite(5, LOW);
	Serial << "Motor off" << endl;
}

void displayOn()
{
	OLED.setDisplay(true);
}

void displayOff()
{
	OLED.setDisplay(false);
}

void setContrast()
{
	char *arg1 = SCmd.next();
	if (arg1 != NULL)
	{
		uint8_t contrastLevel = atoi(arg1);
		OLED.setContrast(contrastLevel);
	}
}

void fill()
{
	char *arg1 = SCmd.next();
	if (arg1 != NULL)
	{
		uint8_t fillPattern = atoi(arg1);
		OLED.fillHorizontal(fillPattern);
	}
}

void beginOled()
{
	OLED.begin();
}

void clear()
{
	OLED.clear();
}

void start()
{
	OLED.startAutoMovement();
}

void stop()
{
	OLED.stopAutoMovement();
}

void play()
{
	char *arg1 = SCmd.next();
	if (arg1 != NULL)
	{
		uint8_t soundfile = atoi(arg1);
		audio.playHuffArrayBlocking(soundfile);
	}
}

void set()
{
	char *arg1 = SCmd.next();
	char *arg2 = SCmd.next();
	char *arg3 = SCmd.next();
	char *arg4 = SCmd.next();
	if (arg1 != NULL && arg2 != NULL)
	{
		int16_t px = atoi(arg1);
		int16_t py = atoi(arg2);
		int16_t upper = atoi(arg3);
		int16_t lower = atoi(arg4);
		OLED.setEyeTargetPosition(px, py, upper, lower, LEFT_EYE);
		OLED.setEyeTargetPosition(px, py, upper, lower, RIGHT_EYE);
	}
}

void unrecognized()
{
	Serial << "Unknown command." << endl;
}

void updateIRSensor()
{
	static uint32_t IRUpdateTimer;
	static uint8_t activationCounter = 0;
	static uint8_t accumulateCounter = 0;
	static uint32_t accumalateTimer;
	static int8_t pupilInwardPixel = 0;

	static bool actionIsArmed = false;
	if (millis() - IRUpdateTimer < IR_UPDATE_INTERVAL) return;
	IRUpdateTimer = millis();
	
	uint16_t readValue = analogRead(IR_RECEIVER_INPUT_PIN);
	if (readValue < IR_DETECTION_THRESHOLD)
	{
		if (activationCounter >= IR_DETECTION_MIN_COUNT)
		{
			if (actionIsArmed == false)
			{
				Serial << "Armed !" << endl;
				actionIsArmed = true;
				OLED.stopAutoMovement();
				OLED.setEyeTargetPosition(0, 0, 0, 0, LEFT_EYE);
				OLED.setEyeTargetPosition(0, 0, 0, 0, RIGHT_EYE);
				accumalateTimer = millis();
				pupilInwardPixel = 0;
			}
			else
			{
				if (millis() - accumalateTimer > ACCUMULATE_DURATION)
				{
					accumalateTimer = millis();
					if(pupilInwardPixel<MAX_PUPIL_INWARD_PIXEL)pupilInwardPixel++;
					OLED.setEyeTargetPosition(-pupilInwardPixel, 0, pupilInwardPixel/3, -pupilInwardPixel/3, LEFT_EYE);
					OLED.setEyeTargetPosition(pupilInwardPixel, 0, pupilInwardPixel / 3, -pupilInwardPixel / 3, RIGHT_EYE);
				}
			}
		}
		else
		{
			activationCounter++;
		}
	}
	else
	{
		if (actionIsArmed == true)
		{
			Serial << "Released !" << endl;
			actionIsArmed = false;

			//wide
			OLED.setEyeTargetPosition(0, 0, -10, 10, LEFT_EYE);
			OLED.setEyeTargetPosition(0, 0, -10, 10, RIGHT_EYE);
			//a fixed time update to make the previous expression take effect
			uint32_t updateTimer = millis();
			while (millis() - updateTimer < 500)
			{
				OLED.update();
			}
			
			uint8_t file = random(10);
			audio.playHuffArrayBlocking(file);

			//narrow
			OLED.setEyeTargetPosition(0, -10, 0, 0, LEFT_EYE);
			OLED.setEyeTargetPosition(0, -10, 0, 0, RIGHT_EYE);
			//a fixed time update to make the previous expression take effect
			updateTimer = millis();
			while (millis() - updateTimer < 500)
			{
				OLED.update();
			}

			OLED.startAutoMovement();
		}

		activationCounter = 0;
	}
	//Serial << readValue << endl;
}

void setExpresssion()
{
	char *arg1 = SCmd.next();
	if (arg1 != NULL )
	{
		uint8_t exp = atoi(arg1);
		switch (exp)
		{
			case 0: //neutral
			{
				OLED.setEyeTargetPosition(0, 0, 0, 0, LEFT_EYE);
				OLED.setEyeTargetPosition(0, 0, 0, 0, RIGHT_EYE);
				break;
			}
			case 1: //narrow
			{
				OLED.setEyeTargetPosition(0, 0, 10, -10, LEFT_EYE);
				OLED.setEyeTargetPosition(0, 0, 10, -10, RIGHT_EYE);
				break;
			}
			case 2: //cross
			{
				OLED.setEyeTargetPosition(-25, 0, 5, -5, LEFT_EYE);
				OLED.setEyeTargetPosition(25, 0, 5, -5, RIGHT_EYE);
				break;
			}
			case 3: //wide
			{
				OLED.setEyeTargetPosition(0, 0, -10, 10, LEFT_EYE);
				OLED.setEyeTargetPosition(0, 0, -10, 10, RIGHT_EYE);
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

void eyeRotation()
{
	uint32_t updateTimer;
	OLED.stopAutoMovement();
	
	const int16_t radius = 20;
	float deg = 0;
	int16_t intx, inty;

	updateTimer = millis();
	while (millis() - updateTimer < 2000)
	{
		intx = (int16_t)(sin(deg)*radius);
		inty = (int16_t)(cos(deg)*radius);
		//Serial << intx << " " << inty << endl;
		OLED.setEyeTargetPosition(intx, -inty, -20, 20, LEFT_EYE);
		OLED.setEyeTargetPosition(-intx, inty, -20, 20, RIGHT_EYE);
		OLED.update();
		delay(10);
		deg += 0.2;
	}
	OLED.startAutoMovement();
}