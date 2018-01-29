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

void setup()
{
	Serial.begin(115200);
	//while (!Serial) { ; }// wait for serial port to connect
	Serial << "OLDE Tester started..." << endl;

	addSerialCommands();
	printAvailableCommands();

	OLED.begin();
	Serial << "Enter '?' for available commands." << endl;
}

void loop()
{
	SCmd.readSerial();
	OLED.update();
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
	Serial << "start         :start auto movement and blink " << endl;
	Serial << "*********************************************************************** " << endl;
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
	//OLED.startAutoMovement();

}

void unrecognized()
{
	Serial << "Unknown command." << endl;
}
