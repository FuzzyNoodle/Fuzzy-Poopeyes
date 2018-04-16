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
#include "sounddata.h"

//Include the eyes bitmap arrays. Array names are passed in the oled.begin() function.
#include "Eyes_UpperEyelid_128x64.h"
#include "Eyes_Pupil_64x64.h"
#include "Eyes_LowerEyelid_128x64.h"

SerialCommand SCmd;
FuzzyOLEDDriver OLED;
FuzzyDACAudio audio;

#define IR_RECEIVER_INPUT_PIN A1
#define IR_UPDATE_INTERVAL 5 //update period in milliseconds.
#define IR_DETECTION_THRESHOLD 400
#define IR_DETECTION_MIN_COUNT 2	//specify the minimum counts of IR detection to trigger an IR activation
#define ACCUMULATE_DURATION 10
#define MAX_PUPIL_INWARD_PIXEL 30
#define MOTOR_CONTROL_PIN 5

void setup()
{
	Serial.begin(115200);
	//while (!Serial) { ; }// wait for serial port to connect
	Serial << "OLDE Tester started..." << endl;
	 
	addSerialCommands();
	printAvailableCommands();

	OLED.begin(Eyes_UpperEyelid_128x64, Eyes_Pupil_64x64, Eyes_LowerEyelid_128x64);
	audio.begin();

	Serial << "Enter '?' for available commands." << endl;
	
	//setup motor
	pinMode(MOTOR_CONTROL_PIN, OUTPUT);
	digitalWrite(MOTOR_CONTROL_PIN, LOW);
	

	

	wakeUp();
	//eyeRotation();
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
	
	for(uint8_t i=1;i<255;i++)
	{
		analogWrite(5, i);
		delay(2);
	}
	delay(1000);
	for (uint8_t i = 255; i>=1; i--)
	{
		analogWrite(5, i);
		delay(2);
	}
	
	analogWrite(5, 0);
	digitalWrite(5, LOW);

	Serial << "Motor off" << endl;
}

void motorOn()
{
	Serial << "Motor on" << endl;
	for (uint8_t i = 1; i<255; i++)
	{
		analogWrite(5, i);
		delay(2);
	}
}

void motorOff()
{
	Serial << "Motor off" << endl;
	for (uint8_t i = 255; i >= 1; i--)
	{
		analogWrite(5, i);
		delay(2);
	}
	analogWrite(5, 0);
	digitalWrite(5, LOW);
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
	OLED.begin(Eyes_UpperEyelid_128x64, Eyes_Pupil_64x64, Eyes_LowerEyelid_128x64);
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
		play(atoi(arg1));
	}
}

void play(uint8_t trackNumber)
{
	switch (trackNumber)
	{
		case 0:
		{
			audio.playHuffArray(HuffDict_Yawing, SoundDataBits_Yawing, SoundData_Yawing);
			break;
		}
		case 1:
		{
			audio.playHuffArray(HuffDict_short_1, SoundDataBits_short_1, SoundData_short_1);
			break;
		}
		case 2:
		{
			audio.playHuffArray(HuffDict_short_2, SoundDataBits_short_2, SoundData_short_2);
			break;
		}
		case 3:
		{
			audio.playHuffArray(HuffDict_short_3, SoundDataBits_short_3, SoundData_short_3);
			break;
		}
		case 4:
		{
			audio.playHuffArray(HuffDict_short_4, SoundDataBits_short_4, SoundData_short_4);
			break;
		}
		case 5:
		{
			audio.playHuffArray(HuffDict_short_5, SoundDataBits_short_5, SoundData_short_5);
			break;
		}
		case 6:
		{
			audio.playHuffArray(HuffDict_short_6, SoundDataBits_short_6, SoundData_short_6);
			break;
		}
		case 7:
		{
			audio.playHuffArray(HuffDict_medium_1, SoundDataBits_medium_1, SoundData_medium_1);
			break;
		}
		case 8:
		{
			audio.playHuffArray(HuffDict_medium_2, SoundDataBits_medium_2, SoundData_medium_2);
			break;
		}
		case 9:
		{
			audio.playHuffArray(HuffDict_medium_3, SoundDataBits_medium_3, SoundData_medium_3);
			break;
		}
		case 24:
		{
			//audio.playHuffArray(HuffDict_medium_4, SoundDataBits_medium_4, SoundData_medium_4);
			break;
		}
		case 10:
		{
			audio.playHuffArray(HuffDict_long_1, SoundDataBits_long_1, SoundData_long_1);
			break;
		}
		case 32:
		{
			//audio.playHuffArray(HuffDict_long_2, SoundDataBits_long_2, SoundData_long_2);
			break;
		}

		default:
		{
			break;
		}
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
	static uint8_t accumalatedStrength = 0;

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
				accumalatedStrength = 0;
			}
			else
			{
				if (millis() - accumalateTimer > ACCUMULATE_DURATION)
				{
					accumalateTimer = millis();
					if (accumalatedStrength < 100)accumalatedStrength++;
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
			Serial << "Released ! " << accumalatedStrength <<endl;
			actionIsArmed = false;

			//wide eye
			OLED.setEyeTargetPosition(0, 0, -10, 10, LEFT_EYE);
			OLED.setEyeTargetPosition(0, 0, -10, 10, RIGHT_EYE);

			
			//1-6 short, 7-9 medium, 10 long
			if (accumalatedStrength < 20)
			{
				//short sound + wide eye
				uint8_t trackNumber = random(4);
				play(trackNumber + 1);
			}
			else if (accumalatedStrength < 60)
			{
				//short or medium sound + wide eye
				uint8_t trackNumber = random(9);
				play(trackNumber + 1);
			}
			else if (accumalatedStrength < 80)
			{
				//medium or long + wide eye
				uint8_t trackNumber = random(4);
				play(trackNumber + 7);
			}
			else
			{
				//long sound + rotate eye
				
				play(10);
				eyeRotation();
				while (audio.isPlaying());
				motorOn();
				delay(500);
				motorOff();
			}

			//a fixed time update to make the previous expression take effect
			uint32_t updateTimer = millis();
			//while (millis() - updateTimer < 500)
			//{
			//	OLED.update();
			//}
			
			while (audio.isPlaying())
			{
				OLED.update();
			}

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
	
	float radius = 30;
	float deg = 0;
	int16_t intx, inty;

	updateTimer = millis();
	while (millis() - updateTimer < 3000)
	{
		
		intx = (int16_t)(sin(deg)*(int16_t)radius);
		inty = (int16_t)(cos(deg)*(int16_t)radius);
		//Serial << intx << " " << inty << endl;
		OLED.setEyeTargetPosition(intx, -inty, -20, 20, LEFT_EYE);
		OLED.setEyeTargetPosition(-intx, inty, -20, 20, RIGHT_EYE);
		OLED.update();
		delay(10);
		deg += 0.2;
		radius -= 0.1;
	}
}

void wakeUp()
{
	uint32_t updateTimer;
	OLED.stopAutoMovement();

	
	OLED.setEyePosition(0, 20, 32, -32, LEFT_EYE);
	OLED.setEyePosition(0, 20, 32, -32, RIGHT_EYE);
	OLED.update();

	int16_t h = 0;

	updateTimer = millis();
	while (millis() - updateTimer < 1000)
	{
		OLED.setEyePosition(0, 20 - h, 32 - h, -32 + h, LEFT_EYE);
		OLED.setEyePosition(0, 20 - h, 32 - h, -32 + h, RIGHT_EYE);
		OLED.update();
		h++;
		if (h > 20)h = 20;
		delay(50);
	}

	audio.playHuffArray(HuffDict_Yawing, SoundDataBits_Yawing, SoundData_Yawing);
	delay(1000);
	updateTimer = millis();
	while (millis() - updateTimer < 2000)
	{
		OLED.setEyePosition(0, 0, 32-h, -32+h, LEFT_EYE);
		OLED.setEyePosition(0, 0, 32-h, -32+h, RIGHT_EYE);
		OLED.update();
		h++;
		if (h > 40)h = 40;
		delay(30);
	}
	OLED.startAutoMovement();
}