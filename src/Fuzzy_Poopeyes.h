/*
 Name:		Fuzzy_Poopeyes.h
 Created:	1/29/2018 9:15:08 PM
 Author:	georgychen
 Editor:	http://www.visualmicro.com
*/

#ifndef _Fuzzy_Poopeyes_h
#define _Fuzzy_Poopeyes_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#if defined (SAM)
#define Serial SerialUSB
#endif

#include "Adafruit_ZeroDMA_Local.h"
#include <Wire.h>

#define ENABLE_DEVICE_2


#define I2C_ADDRESS_1 0x3D //3D (0x7A on pcb) is placed at the left eye
#define I2C_ADDRESS_2 0x3C //3C (0x78 on pcb) is placed at the right eye
#define I2C_WRITE 0x00
#define I2C_READ 0x01
#define DISPLAY_OFF 0xAE
#define DISPLAY_ON 0xAF
#define CONTROL_BYTE_COMMAND 0x00
#define CONTROL_BYTE_DATA 0x40
#define SET_DISPLAY_CLOCK_DIV 0xD5
#define SET_MULTIPLEX_RATIO 0xA8
#define SET_DISPLAY_OFFSET 0xD3
#define SET_DISPLAY_START_LINE 0x40
#define SET_CHARGE_PUMP 0x8D
#define SET_NORMAL_DISPLAY 0xA6
#define SET_DISPLAY_ON_RESUME_RAM 0xA4
#define SET_INVERSE_DISPLAY 0xA7
#define SET_SEG_REMAPPED_FALSE 0xA0
#define SET_SEG_REMAPPED_TRUE 0xA1
#define SET_COM_SCAN_REMAPPED_TRUE 0xC8
#define SET_COM_SCAN_REMAPPED_FALSE 0xC0
#define SET_CONTRAST 0x81
#define SET_PRE_CHARGE_PERIOD 0xD9
#define SET_VCOMH_DESELECT_LEVEL 0xDB
#define SET_MEMORY_ADDRESS_MODE 0x20
#define SET_COM_PINS	0xDA
#define HORIZONTAL_MODE 0x00
#define VERTICAL_MODE 0x01
#define PAGE_ADDRESSING_MODE 0x02

#define SET_COLUMN_START_END_ADDRESS 0x21
#define SET_PAGE_START_END_ADDRESS 0x22

//These commands is only for page addressing mode
#define SET_PAGE_ADDRESS_PAGE_MODE 0xB0
#define SET_LN_COLUMN_ADDRESS_PAGE_MODE_MASK  0x0F
#define SET_HN_COLUMN_ADDRESS_PAGE_MODE  0x10

#define OLED_BUFFER_SIZE 1024
#define EXPANDED_OLED_BUFFER_SIZE (OLED_BUFFER_SIZE*3)
#define OLED_BUFFER_SIZE_32 (OLED_BUFFER_SIZE>>2)

#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64
#define OLED_SCREEN_COLUMN 128
#define OLED_SCREEN_COLUMN_32 (OLED_SCREEN_COLUMN>>2)
#define OLED_SCREEN_PAGE 8
#define PUPIL_WIDTH 64
#define PUPIL_HEIGHT 64
#define PUPIL_BUFFER_SIZE 512
#define PUPIL_BUFFER_SIZE_32 (PUPIL_BUFFER_SIZE>>2)
#define PUPIL_SIDE_MARGIN (OLED_SCREEN_WIDTH-PUPIL_WIDTH)/2
#define PUIIL_TOP_DOWN_MARGIN (OLED_SCREEN_HEIGHT-PUPIL_HEIGHT)/2
#define EOF_BRIGHT_INDEX_OFFSET 1
#define EOF_DARK_INDEX_OFFSET 2

#define BLINK_MIN_INTERVAL 500
#define BLINK_MAX_INTERVAL 6000
#define BLINK_UPDATE_INTERVAL 10
#define CHANGE_POSITION_MIN_INTERVAL 500
#define CHANGE_POSITION_BLINK_MAX_INTERVAL 3000
#define DEFAULT_BLINK_MOVEMENT_LENGTH 60
#define DEFAULT_BLINK_STEP 5
#define UPDATE_POSITION_INTERVAL 10
#define EYELID_MOVEMENT_DIVIDER 2

#define BRIGHT_32 0x00000000
#define BRIGHT_BYTE 0x00
#define BRIGHT_BIT 0x00

#define DARK_32 0xFFFFFFFF
#define DARK_BYTE 0xFF
#define DARK_BIT 0b1


enum EYE_INDEX
{
	LEFT_EYE = 0,
	RIGHT_EYE
};

union unionBufferType
{
	uint8_t buffer8[OLED_BUFFER_SIZE];
	uint32_t buffer32[OLED_BUFFER_SIZE >> 2];
};


union unionPupilBitmapArrayType
{
	uint8_t buffer8[PUPIL_BUFFER_SIZE + 2]; //additional two bytes for bright byte and dark byte
	uint32_t buffer32[PUPIL_BUFFER_SIZE_32];
};

union unionEyelidBitmapArrayType
{
	uint8_t buffer8[OLED_BUFFER_SIZE];
	uint32_t buffer32[OLED_BUFFER_SIZE_32 + 2]; //additional two byte32 for bright byte32 and dark byte32
};

class FuzzyOLEDDriver
{
public:
	FuzzyOLEDDriver();
	void begin();
	void setDisplay(bool value);
	void setContrast(uint8_t value);
	void clear();
	void writeBufferHorizontal(unionBufferType *buffer, EYE_INDEX eye);
	void setEyeArray(const unsigned char *_upperEyelidArray, const unsigned char *_pupilArray, const unsigned char *_lowerEyelidArray); //copy the bitmap array to related operating array, input left only
	void testEye();
	void blink();
	void update();
	void startAutoMovement();
	void stopAutoMovement();
	void fillHorizontal(uint8_t value);
	void setEyeTargetPosition(int16_t PupilX, int16_t PupilY, int16_t UpperEyelid, int16_t LowerEyelid, EYE_INDEX eye);

private:
	uint8_t _i2cAddress[2] = { I2C_ADDRESS_1,I2C_ADDRESS_2 };
	uint8_t oledBuffer[2][OLED_BUFFER_SIZE];
	unionBufferType unionScreenBuffer[2];
	unionPupilBitmapArrayType unionPupilArray;
	unionEyelidBitmapArrayType unionLeftUpperEyelidArray;
	unionEyelidBitmapArrayType unionLeftLowerEyelidArray;
	unionEyelidBitmapArrayType unionRightUpperEyelidArray;
	unionEyelidBitmapArrayType unionRightLowerEyelidArray;
	uint32_t BIT_MASK_RIGHT_SHIFT_32[9] =
	{
		0xFFFFFFFF, //11111111 11111111 11111111 11111111
		0x7F7F7F7F, //01111111 01111111 01111111 01111111
		0x3F3F3F3F, //00111111 00111111 00111111 00111111
		0x1F1F1F1F, //00011111 00011111 00011111 00011111
		0x0F0F0F0F, //00001111 00001111 00001111 00001111
		0x07070707, //00000111 00000111 00000111 00000111
		0x03030303, //00000011 00000011 00000011 00000011
		0x01010101, //00000001 00000001 00000001 00000001
		0x00000000, //00000000 00000000 00000000 00000000
	};

	uint32_t BIT_MASK_LEFT_SHIFT_32[9] =
	{
		0xFFFFFFFF, //11111111 11111111 11111111 11111111
		0xFEFEFEFE, //11111110 11111110 11111110 11111110
		0xFCFCFCFC, //11111100 11111100 11111100 11111100
		0xF8F8F8F8, //11111000 11111000 11111000 11111000
		0xF0F0F0F0, //11110000 11110000 11110000 11110000
		0xE0E0E0E0, //11100000 11100000 11100000 11100000
		0xC0C0C0C0, //11000000 11000000 11000000 11000000
		0x80808080, //10000000 10000000 10000000 10000000
		0x00000000, //00000000 00000000 00000000 00000000
	};

	int16_t leftUpperEyelidPositionOffset = 0; //range from -64 to 64
	int16_t leftLowerEyelidPositionOffset = 0;
	int16_t rightUpperEyelidPositionOffset = 0; //range from -64 to 64
	int16_t rightLowerEyelidPositionOffset = 0;

	int16_t leftPupilXPositionOffset = 0; //range from -96 to 96
	int16_t leftPupilYPositionOffset = 0; //range from -64 to 64
	int16_t rightPupilXPositionOffset = 0; //range from -96 to 96
	int16_t rightPupilYPositionOffset = 0; //range from -64 to 64		  

	void sendCommand(uint8_t command);//same command to both chips
	void sendCommand(uint8_t command1, uint8_t command2);//command both chips
	void sendData(uint8_t data);//same data to both chips
	void sendData(uint8_t data1, uint8_t data2);

	void drawEye(unionBufferType *screenBuffer, unionEyelidBitmapArrayType *upperEyelidArray, unionPupilBitmapArrayType *pupilArray, unionEyelidBitmapArrayType *lowerEyelidArray, EYE_INDEX eye);

	void randomBlink();
	uint32_t blinkTimer;
	uint32_t updateBlinkTimer;
	uint32_t blinkInterval;
	bool upperEyelidDirectionIsDownward;
	int16_t blinkUpperEyelidMovementLength;
	bool blinkInProgress = false;
	void updateBlink();
	int16_t upperEyelidBlinkYOffsetDeltaLeft;
	int16_t	upperEyelidBlinkYOffsetDeltaRight;
	int16_t lowerEyelidBlinkYOffsetDeltaLeft;
	int16_t	lowerEyelidBlinkYOffsetDeltaRight;

	void randomPosition();
	uint32_t changePositionTimer;
	uint32_t changePositionInterval;
	int16_t targetXLeft;
	int16_t targetYLeft;
	int16_t targetXRight;
	int16_t targetYRight;
	int16_t leftUpperEyelidPositionOffsetTarget = 0;
	int16_t leftLowerEyelidPositionOffsetTarget = 0;
	int16_t rightUpperEyelidPositionOffsetTarget = 0;
	int16_t rightLowerEyelidPositionOffsetTarget = 0;

	void updatePosition();
	uint32_t updatePositionTimer;
	bool autoMovement = false;

	Adafruit_ZeroDMA myDMA;
	ZeroDMAstatus    stat; // DMA status codes returned by some functions
	static void dma_callback(Adafruit_ZeroDMA *dma);
	volatile bool transfer_is_done = false;
	void writeBufferDMA(unionBufferType *buffer, EYE_INDEX eye);
	static FuzzyOLEDDriver* fuzzyOLEDPointer;
	void updateDMA();
	DmacDescriptor *descriptor;
	EYE_INDEX currentUpdatingDevice;
	uint32_t eyeDMAUpdatingTimer;
	uint32_t fpsTimer;
	uint16_t fps;
};




#include "sounddata.h"

#define DAC_8_NEUTRAL 128
#define DEFAULT_SAMPLE_RATE 22050
#define SHUTDOWN_PIN 4
#define ON true
#define OFF false

struct {
	// Current sample position
	uint_fast32_t samplePosition;

	// Current amplitude value
	// ------
	// MIC: Actual value should be integers within [0, 255], and its used for a 16-bit timer.
	// But I'm still setting it to int16 to avoid potential (lower-bound) overflow.
	// See value setting of OCR1A.
	int_fast16_t currentPCM;

} g_stat = { 0, 0 };


class FuzzyDACAudio
{
public:
	FuzzyDACAudio();
	void begin();
	void play8BitArray(const uint8_t* arrayName, uint32_t arraySize);
	void interruptHandler();
	bool isPlaying();
	void playHuffArrayBlocking(uint8_t trackIndex);
	void playHuffArrayBlocking(uint8_t trackIndex, uint16_t sampleRate);

private:
	uint32_t _sampleRate = DEFAULT_SAMPLE_RATE;
	void tcConfigure(uint32_t sampleRate);
	void tcReset();
	void tcDisable();
	bool tcIsSyncing();
	void tcStartCounter();
	uint32_t __nowPlayingSampleIndex = 0;
	uint32_t __arraySize;
	const uint8_t* __arrayName;
	bool volatile __isPlaying = false;

	inline int _get_bit(uint_fast32_t pos, boolean autoLoadOnBit0 = false);
	//const uint8_t* __SoundData;
	int_fast16_t _decode_huff(uint_fast32_t &pos, int_fast16_t const *huffDict);
	void loadSample();
	const int_fast16_t *_HuffDict;
	uint_fast32_t _SoundDataBits;
	const uint8_t * _SoundData;

	void setAmplifier(bool status); //turn amplifier on(true) or off(false)
};



#endif

