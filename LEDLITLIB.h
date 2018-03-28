// LEDLITLIB.h

#ifndef _LEDLITLIB_h
#define _LEDLITLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#define RES_10BIT

#ifdef RES_10BIT
#define BRIGHTNESS_TYPE uint16_t 
#else
#define BRIGHTNESS_TYPE uint8_t 
#endif


//#define DEBUG_LED //Comment out to disable debug to serial monitor.

//Gammacorrection achieves a more linear perception of the LED when increasing or decreasing the brightness.
#define GAMMACORRECTION_ON		true			//Gammacorrection enabled
#define GAMMACORRECTION_OFF		false			//Gammacorrection disabled

//The modes in which the led can operate
#define MODE_OFF				1			//Led is in the complete "off" state, but is enabled.
#define MODE_CONSTANT			2			//Led is in constant mode.
#define MODE_SPARKLE			3			//Led is sparkling.
#define MODE_BLINK				4			//Led is blinking.


const BRIGHTNESS_TYPE BRIGHTNESS_TYPE_MAX = 1023;
const BRIGHTNESS_TYPE BRIGHTNESS_TYPE_MIN = 0;


//Lookup table for gamma correction.

const uint8_t PROGMEM gamma8[] = {
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
	1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
	2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
	5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
	10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
	17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
	25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
	37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
	51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
	69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
	90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
	115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
	144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
	177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
	215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

const uint16_t gamma10[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,
	2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,
	3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,4,
	4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,6,
	6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,
	7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,
	9,10,10,10,10,10,10,10,11,11,11,11,11,11,12,12,
	12,12,12,12,12,13,13,13,13,13,13,14,14,14,14,14,
	15,15,15,15,15,15,16,16,16,16,16,17,17,17,17,17,
	18,18,18,18,18,19,19,19,19,20,20,20,20,20,21,21,
	21,21,22,22,22,22,23,23,23,23,24,24,24,24,25,25,
	25,25,26,26,26,26,27,27,27,27,28,28,28,29,29,29,
	29,30,30,30,31,31,31,31,32,32,32,33,33,33,34,34,
	34,35,35,35,35,36,36,36,37,37,37,38,38,38,39,39,
	40,40,40,41,41,41,42,42,42,43,43,43,44,44,45,45,
	45,46,46,46,47,47,48,48,48,49,49,50,50,50,51,51,
	52,52,52,53,53,54,54,55,55,55,56,56,57,57,58,58,
	58,59,59,60,60,61,61,62,62,63,63,63,64,64,65,65,
	66,66,67,67,68,68,69,69,70,70,71,71,72,72,73,73,
	74,74,75,75,76,76,77,77,78,79,79,80,80,81,81,82,
	82,83,83,84,85,85,86,86,87,87,88,89,89,90,90,91,
	92,92,93,93,94,95,95,96,96,97,98,98,99,99,100,101,
	101,102,103,103,104,105,105,106,106,107,108,108,109,110,110,111,
	112,112,113,114,115,115,116,117,117,118,119,119,120,121,122,122,
	123,124,124,125,126,127,127,128,129,130,130,131,132,132,133,134,
	135,136,136,137,138,139,139,140,141,142,143,143,144,145,146,146,
	147,148,149,150,151,151,152,153,154,155,155,156,157,158,159,160,
	161,161,162,163,164,165,166,167,167,168,169,170,171,172,173,174,
	175,175,176,177,178,179,180,181,182,183,184,185,186,186,187,188,
	189,190,191,192,193,194,195,196,197,198,199,200,201,202,203,204,
	205,206,207,208,209,210,211,212,213,214,215,216,217,218,219,220,
	221,222,223,224,225,226,228,229,230,231,232,233,234,235,236,237,
	238,239,241,242,243,244,245,246,247,248,249,251,252,253,254,255,
	256,257,259,260,261,262,263,264,266,267,268,269,270,272,273,274,
	275,276,278,279,280,281,282,284,285,286,287,289,290,291,292,294,
	295,296,297,299,300,301,302,304,305,306,308,309,310,311,313,314,
	315,317,318,319,321,322,323,325,326,327,329,330,331,333,334,336,
	337,338,340,341,342,344,345,347,348,349,351,352,354,355,356,358,
	359,361,362,364,365,366,368,369,371,372,374,375,377,378,380,381,
	383,384,386,387,389,390,392,393,395,396,398,399,401,402,404,405,
	407,408,410,412,413,415,416,418,419,421,423,424,426,427,429,431,
	432,434,435,437,439,440,442,444,445,447,448,450,452,453,455,457,
	458,460,462,463,465,467,468,470,472,474,475,477,479,480,482,484,
	486,487,489,491,493,494,496,498,500,501,503,505,507,509,510,512,
	514,516,518,519,521,523,525,527,528,530,532,534,536,538,539,541,
	543,545,547,549,551,553,554,556,558,560,562,564,566,568,570,572,
	574,575,577,579,581,583,585,587,589,591,593,595,597,599,601,603,
	605,607,609,611,613,615,617,619,621,623,625,627,629,631,633,635,
	637,640,642,644,646,648,650,652,654,656,658,660,663,665,667,669,
	671,673,675,678,680,682,684,686,688,690,693,695,697,699,701,704,
	706,708,710,712,715,717,719,721,724,726,728,730,733,735,737,739,
	742,744,746,749,751,753,755,758,760,762,765,767,769,772,774,776,
	779,781,783,786,788,790,793,795,798,800,802,805,807,810,812,814,
	817,819,822,824,827,829,831,834,836,839,841,844,846,849,851,854,
	856,859,861,864,866,869,871,874,876,879,881,884,887,889,892,894,
	897,899,902,905,907,910,912,915,918,920,923,925,928,931,933,936,
	939,941,944,947,949,952,955,957,960,963,965,968,971,973,976,979,
	982,984,987,990,992,995,998,1001,1004,1006,1009,1012,1015,1017,1020,1023};



class LED {
public:
	LED(char* identificator, uint8_t ledPin, bool gammaCorrection, BRIGHTNESS_TYPE lowerLimit = BRIGHTNESS_TYPE_MIN, BRIGHTNESS_TYPE upperLimit = BRIGHTNESS_TYPE_MAX);

	//LED::General::Setters:

	void setPin(uint8_t ledPin);									//Sets the pin to a new value.
	void setGammaCorrection(bool state);							//Sets the gamma correction to on/off.
	void setLowerLimit(BRIGHTNESS_TYPE lowerLimit);					//Sets the lower limit of the LED.
	void setUpperLimit(BRIGHTNESS_TYPE upperLimit);					//Sets the upper limit of the LED.

	//LED::General::Getters:

	BRIGHTNESS_TYPE getLowerLimit();								//Returns the lower limit.
	BRIGHTNESS_TYPE getUpperLimit();								//Returns the upper limit.
	BRIGHTNESS_TYPE getDesiredBrightness();							//Returns the desired brightness.
	BRIGHTNESS_TYPE getBrightness();								//Returns the value of the current brightness.
	BRIGHTNESS_TYPE getLimitedBrightness();							//Returns the value of the current limited brightness.
	BRIGHTNESS_TYPE getGammaCorrectedBrightness();					//Returns the value of the current gamma corrected brightness.
	uint16_t getBlinkOffTime();										//Returns the off time of the led in blink mode.
	uint16_t getBlinkOnTime();										//Returns the on time of the led in blink mode.
	uint16_t getBlinkOffset();									//Returns the offset time of the led in blink mode.
	BRIGHTNESS_TYPE getSparkleIntensity();							//Returns the intensity of the sparkler.
	float getSparkleSmoothing();									//Returns the smoothing factor of the sparkler.

	uint8_t getPin();												//Returns the pin-number.
	uint8_t getMode();												//Returns the mode as an unsigned number.

	bool isOn();													//Indicates if the LED is ON or dimmed.
	bool isMax();													//Indicates if the LED is at its high limit.
	bool isMin();
	bool isDesired();												//Indicates if the brightness has reached the desired value.

	bool isFading();												//Indicates if the LED is currently fading.
	bool isFadeWaving();											//Indicates if the LED is currently fading in a wavelike pattern.

	//LED::General::Methods:

	BRIGHTNESS_TYPE update();

	void setBrightness(BRIGHTNESS_TYPE brightness);					//Sets the brightness to a certain value.
	void increaseBrightness(BRIGHTNESS_TYPE amount);				//Increase the current brightness by the given amount.
	void decreaseBrightness(BRIGHTNESS_TYPE amount);				//Decrease the current brightness by the given amount.

	void setToOff();												//Sets the mode to off. The LED will not light up.

	void fadeToValue(uint16_t time, BRIGHTNESS_TYPE brightness);	//Fade the led to a given value, within the given time.
	void fadeToMax(uint16_t time);									//Fade the led to fully on within the given time.
	void fadeToMin(uint16_t time);									//Fade the led to fully off within the given time.
	void fadeToggle(uint16_t time);									//Fade the led to fully off within the given time. Depending on its current state.
	void fadeWave(uint16_t minToMaxTime, uint16_t numberOfTimes = 0);	//Fade the LED sequentially on and off.
	void fadeStop();												//Stops the LED from fading.
	void resetFadeTimer();											//Resets the fade timer;

	//LED::MODE_CONSTANT::Methods:

	void setToConstant(BRIGHTNESS_TYPE brightness);					//Sets the led to a new constant brightness.
	void setToConstant();											//Sets the led to a pre-defined constant brightness.
	void setToConstantMax();										//Sets the LED to a fully on state.
	void setToConstantMin();										//Sets the LED to a fully off state.
	void setToConstantToggle();										//Sets the LED to a fully on/off state, opposite to what it was.

	//LED::MODE_SPARKLE::Setters:

	void setToSparkle(float smoothing, BRIGHTNESS_TYPE sparkAverage, BRIGHTNESS_TYPE sparkIntensity, BRIGHTNESS_TYPE updateInterval);						//Sets the LED to sparkle with new parameters.
	void setToSparkle();											//Sets the LED to sparkle with pre-defined parameters.
	void setSparkleSmoothing(float smoothing);						//Sets the smoothing factor of the low-pass filter used for the sparkle value (0.9999... is maximal smoothing, 0 is no smoothing.)
	void setSparkleIntensity(BRIGHTNESS_TYPE sparkIntensity);		//Sets the intensity of the sparkling.
	void setSparkleUpdateInterval(uint16_t updateInterval);			//Sets the update interval on which new sparks will be generated.
	void resetSparkleTimer();										//Resets the sparkle timer;

	//LED::MODE_BLINK::Setters:

	void setToBlink(uint16_t onTime, uint16_t offTime, uint16_t offset = 0, BRIGHTNESS_TYPE onBrightness = BRIGHTNESS_TYPE_MAX, BRIGHTNESS_TYPE offBrightness = BRIGHTNESS_TYPE_MIN);  //Sets the LED to blink with new parameters.
	void setToBlink();												//Sets the LED to blink with pre-defined parameters.
	void setBlinkOnTime(uint16_t onTime);
	void setBlinkOffTime(uint16_t offTime);
	void setBlinkOffset(uint16_t offset);
	void setBlinkOnBrightness(BRIGHTNESS_TYPE brightness);
	void setBlinkOffBrightness(BRIGHTNESS_TYPE brightness);
	void resetBlinkTimer();											//Resets the blink timer;

	char*			ID;

private:


	uint8_t			pin;
	uint8_t			mode;
	bool gammaCorrectionEnabled;
	void setDefaults();

	BRIGHTNESS_TYPE brightnessLowerLimit;
	BRIGHTNESS_TYPE brightnessUpperLimit;

	BRIGHTNESS_TYPE desiredBrightness;
	BRIGHTNESS_TYPE finalBrightness;
	BRIGHTNESS_TYPE finalBrightnessLimited;
	BRIGHTNESS_TYPE finalBrightnessLimitedCorrected;

	BRIGHTNESS_TYPE fadeBrightness;
	int				fadeSteps;
	uint32_t		fadePulsePreviousMillis;

	bool			fadeWaving;
	bool			fading;
	bool			fadeWaveContinuous;

	uint16_t		fadeWavesRemaining;
	uint16_t		fadePulseInterval;
	uint16_t		fadeWaveMinToMaxTime;

	void initFadeTimer(int brightnessDifference, uint16_t time); //Difference needs to be signed.
	void updateFade();

	uint16_t		blinkOnTime;
	uint16_t		blinkOffTime;
	uint16_t		blinkOffset;
	bool			blinkOffsetInProgress;
	uint32_t		blinkPreviousMillis;
	bool			blinkState;
	BRIGHTNESS_TYPE blinkOnBrightness;
	BRIGHTNESS_TYPE blinkOffBrightness;

	void updateBlink();

	BRIGHTNESS_TYPE sparklePreviousBrightness;
	BRIGHTNESS_TYPE sparkleCurrentBrightness;

	uint16_t		sparkleUpdateInterval;
	uint32_t		sparkleUpdatePreviousMillis;
	BRIGHTNESS_TYPE sparkleIntensity;
	float			sparkleSmoothing;

	void setSparkleBrightness(BRIGHTNESS_TYPE brightness);					//Sets the sparkle average brightness.
	void updateSparkle();

};


#endif
