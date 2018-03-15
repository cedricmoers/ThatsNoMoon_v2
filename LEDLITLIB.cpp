// 
// 
// 

#include "LEDLITLIB.h"


#ifdef DEBUG_LED    //Macros are usually in all capital letters.
#define DEBUG_PRINT_HEADER(...) {							\
									Serial.print(millis()); \
									Serial.print("::");		\
									Serial.print(this->ID); \
									Serial.print("::");		\
								}
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__);
#define DEBUG_PRINT_F(...) Serial.print(__VA_ARGS__);
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__);
#define DEBUG_PRINTLN_F(...) Serial.println(__VA_ARGS__);
#else
#define DEBUG_PRINT_HEADER(...)     //now defines a blank line
#define DEBUG_PRINT(...)     //now defines a blank line
#define DEBUG_PRINT_F(...)     //now defines a blank line
#define DEBUG_PRINTLN(...)     //now defines a blank line
#define DEBUG_PRINTLN_F(...)     //now defines a blank line
#endif


//Constructor(s)
LED::LED(char* identificator, uint8_t ledPin, bool gammaCorrection /*= true*/, BRIGHTNESS_TYPE lowerLimit /*= BRIGHTNESS_TYPE_MIN*/, BRIGHTNESS_TYPE upperLimit /*= BRIGHTNESS_TYPE_MAX*/) {

	if (upperLimit == BRIGHTNESS_TYPE_MIN) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Upper limit must be greater than BRIGHTNESS_TYPE_MIN. Changing lower limit to zero and upper limit to BRIGHTNESS_TYPE_MAX.");
		this->brightnessLowerLimit = BRIGHTNESS_TYPE_MAX;
		this->brightnessUpperLimit = BRIGHTNESS_TYPE_MIN;
	}
	else if (upperLimit <= lowerLimit) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Upper limit must be greater than lower limit. Changing lower limit to zero.");
		this->brightnessLowerLimit = upperLimit;
		this->brightnessUpperLimit = BRIGHTNESS_TYPE_MIN;
	}
	else {
		this->brightnessLowerLimit = lowerLimit;
		this->brightnessUpperLimit = upperLimit;
	}
	this->pin = ledPin;
	this->gammaCorrectionEnabled = gammaCorrection;
	this->mode = MODE_OFF;
	this->ID = identificator;

	setPin(ledPin);
	setDefaults();

	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("New LED with ID: \"");
	DEBUG_PRINT(this->ID);
	DEBUG_PRINT_F("\" initialized on pin: ");
	DEBUG_PRINT(this->pin);
	DEBUG_PRINT_F("with gamma correction ");
	DEBUG_PRINT(gammaCorrectionEnabled ? "enabled" : "disabled");
	DEBUG_PRINT_F(", limited between:");
	DEBUG_PRINT(lowerLimit);
	DEBUG_PRINT_F(" and ")
		DEBUG_PRINT(upperLimit);
	DEBUG_PRINTLN_F(".")
}

void LED::setDefaults() {

	desiredBrightness = BRIGHTNESS_TYPE_MAX;

	fadeWaving = false;
	fading = false;

	fadeBrightness = BRIGHTNESS_TYPE_MIN;
	fadeSteps = 1;
	fadePulseInterval = 4;
	fadeWaveMinToMaxTime = 1000;

	blinkOnTime = 500;
	blinkOffTime = 500;
	blinkOffset = 0;

	blinkOnBrightness = BRIGHTNESS_TYPE_MAX;
	blinkOffBrightness = BRIGHTNESS_TYPE_MIN;

	sparkleUpdateInterval = 20;
	sparkleIntensity = 50;
	sparkleSmoothing = 0.8;
}


uint8_t LED::getPin() {
	return this->pin;
}

void LED::setPin(uint8_t ledPin) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting LED pin to: ");
	DEBUG_PRINTLN(ledPin);

	this->pin = ledPin;
	pinMode(this->pin, OUTPUT);
	digitalWrite(this->pin, LOW);
}

void LED::setGammaCorrection(bool state) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting Gamma Correction to ");
	DEBUG_PRINTLN(state ? "enabled." : "disabled.");
	this->gammaCorrectionEnabled = state;
}

void LED::setLowerLimit(BRIGHTNESS_TYPE lowerLimit) {
	if (lowerLimit >= this->brightnessUpperLimit) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Lower limit must be smaller than upper limit. Keeping previous value.");
		return;
	}
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting lower brightness limit to: ");
	DEBUG_PRINT(lowerLimit);
	DEBUG_PRINTLN_F(".");
	this->brightnessLowerLimit = lowerLimit;
}

BRIGHTNESS_TYPE LED::getLowerLimit() {
	return this->brightnessLowerLimit;
}

void LED::setUpperLimit(BRIGHTNESS_TYPE upperLimit) {
	if (upperLimit <= this->brightnessLowerLimit) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Upper limit must be greater than upper limit. Keeping previous value.");
		return;
	}
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting upper brightness limit to: ");
	DEBUG_PRINT(upperLimit);
	DEBUG_PRINT_F(".");
	this->brightnessUpperLimit = upperLimit;
}

BRIGHTNESS_TYPE LED::getUpperLimit() {
	return this->brightnessUpperLimit;
}

BRIGHTNESS_TYPE LED::getDesiredBrightness()
{
	return this->desiredBrightness;
}

BRIGHTNESS_TYPE LED::getBrightness()
{
	return this->finalBrightness;
}

BRIGHTNESS_TYPE LED::getLimitedBrightness()
{
	return this->finalBrightnessLimited;
}

BRIGHTNESS_TYPE LED::getGammaCorrectedBrightness()
{
	return this->finalBrightnessLimitedCorrected;
}

uint16_t LED::getBlinkOffTime() {
	return this->blinkOffTime;
}

uint16_t LED::getBlinkOnTime() {
	return this->blinkOnTime;
}

uint16_t LED::getBlinkOffset() {
	return this->blinkOffset;
}

BRIGHTNESS_TYPE LED::getSparkleIntensity() {
	return this->sparkleIntensity;
}
float LED::getSparkleSmoothing() {
	return this->sparkleSmoothing;
}

bool LED::isOn() {
	if (this->finalBrightness > BRIGHTNESS_TYPE_MIN) {
		return true;
	}
	else {
		return false;
	}
}

bool LED::isMax() {
	if (this->finalBrightness == BRIGHTNESS_TYPE_MAX) {
		return true;
	}
	else {
		return false;
	}
}

bool LED::isMin() {
	if (this->finalBrightness == BRIGHTNESS_TYPE_MIN) {
		return true;
	}
	else {
		return false;
	}
}

bool LED::isDesired() {
	if (this->finalBrightness == this->desiredBrightness) {
		return true;
	}
	else {
		return false;
	}
}

void LED::setToConstantMax() {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Setting LED brightness to maximum.");
	this->setToConstant(BRIGHTNESS_TYPE_MAX);
}

void LED::setToConstantMin() {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Setting LED brightness to minimum.");
	this->setToConstant(BRIGHTNESS_TYPE_MIN);
}

void LED::setToConstantToggle() {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Toggling LED.");
	this->isOn() ? setToConstantMin() : setToConstantMax();
}

void LED::setBrightness(BRIGHTNESS_TYPE brightness) {
	if (this->mode != MODE_OFF) {
		switch (mode) {
		case MODE_CONSTANT:
			desiredBrightness = brightness;
			break;

		case MODE_BLINK:
			setBlinkOnBrightness(brightness);
			break;

		case MODE_SPARKLE:
			setSparkleBrightness(brightness);
			break;
		}
	}
}

void LED::increaseBrightness(BRIGHTNESS_TYPE amount) {
	if (amount >= (BRIGHTNESS_TYPE_MAX - this->desiredBrightness)) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Brightness set to maximum.");
		setBrightness(BRIGHTNESS_TYPE_MAX);
	}
	else {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINT_F("Increasing brightness by ");
		DEBUG_PRINT(amount);
		DEBUG_PRINTLN_F(".");
		setBrightness(this->desiredBrightness + amount);
	}
}

void LED::decreaseBrightness(BRIGHTNESS_TYPE amount) {
	if (amount <= this->desiredBrightness) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Brightness set to minimum.");
		setBrightness(BRIGHTNESS_TYPE_MIN);
	}
	else {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINT_F("Decreasing brightness by ");
		DEBUG_PRINT(amount);
		DEBUG_PRINTLN_F(".");
		setBrightness(this->desiredBrightness - amount);
	}
}




void LED::setToOff() {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Setting LED to the off state.");
	this->mode = MODE_OFF;
}

void LED::setToConstant(BRIGHTNESS_TYPE brightness) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting LED to a constant brightness of: ");
	DEBUG_PRINT(brightness);
	DEBUG_PRINTLN_F(".");
	this->mode = MODE_CONSTANT;
	setBrightness(brightness);
}

void LED::setToConstant() {
	setToConstant(this->desiredBrightness);
}

void LED::setToBlink(uint16_t onTime, uint16_t offTime, uint16_t offset /*= BRIGHTNESS_TYPE_MIN*/, BRIGHTNESS_TYPE onBrightness /*= BRIGHTNESS_TYPE_MAX*/, BRIGHTNESS_TYPE offBrightness /*= BRIGHTNESS_TYPE_MIN*/) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Setting LED to blink.");

	setBlinkOnBrightness(onBrightness);
	setBlinkOffBrightness(offBrightness);

	setBlinkOnTime(onTime);
	setBlinkOffTime(offTime);
	setBlinkOffset(offset);

	resetBlinkTimer();

	this->mode = MODE_BLINK;
}

void LED::setToBlink() {
	setToBlink(this->blinkOnTime, this->blinkOffTime, this->blinkOffset, this->blinkOnBrightness, this->blinkOffBrightness);
}

void LED::resetBlinkTimer() {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Resetting blink timer.");
	this->blinkPreviousMillis = millis();
	this->blinkState = false;
	if (blinkOffset != 0) {
		this->blinkOffsetInProgress = true;
	}
	else {
		this->blinkOffsetInProgress = false;
	}
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Done.");
}

void LED::setBlinkOnTime(uint16_t onTime) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting blink on-time to: ");
	DEBUG_PRINT(onTime);
	DEBUG_PRINTLN_F(".");
	this->blinkOnTime = onTime;
}

void LED::setBlinkOffTime(uint16_t offTime) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting blink off-time to: ");
	DEBUG_PRINT(offTime);
	DEBUG_PRINTLN_F(".");
	this->blinkOffTime = offTime;
}

void LED::setBlinkOffset(uint16_t offset) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting blink offset to: ");
	DEBUG_PRINT(offset);
	DEBUG_PRINTLN_F(".");
	this->blinkOffset = offset;
}



void LED::setBlinkOnBrightness(BRIGHTNESS_TYPE brightness) {
	if (brightness <= blinkOffBrightness) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("The blink on-brightness should not be lower than or equal to the off-brightness. The value will be changed to BRIGHTNESS_TYPE_MAX.");
		this->desiredBrightness = BRIGHTNESS_TYPE_MAX;
	}
	else {
		this->desiredBrightness = brightness;
	}
}

void LED::setBlinkOffBrightness(BRIGHTNESS_TYPE brightness) {
	if (brightness >= desiredBrightness) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("The blink off-brightness should not be greater than or equal to the off-brightness. The value will be changed to BRIGHTNESS_TYPE_MIN.");
		this->desiredBrightness = BRIGHTNESS_TYPE_MIN;
	}
	else {
		this->blinkOffBrightness = brightness;
	}
}

void LED::setToSparkle(float smoothing, BRIGHTNESS_TYPE sparkAverage, BRIGHTNESS_TYPE sparkIntensity, BRIGHTNESS_TYPE updateInterval) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting LED to sparkle with an smoothing factor of: ");
	DEBUG_PRINT(smoothing);
	DEBUG_PRINT_F(", and an intensity of: ");
	DEBUG_PRINT(sparkIntensity);
	DEBUG_PRINTLN_F(".");

	setSparkleSmoothing(smoothing);
	this->desiredBrightness = sparkAverage;
	setSparkleIntensity(sparkIntensity);
	setSparkleBrightness(sparkAverage);
	setSparkleUpdateInterval(updateInterval);

	this->mode = MODE_SPARKLE;
}

void LED::setToSparkle() {
	setToSparkle(this->sparkleSmoothing, this->desiredBrightness, this->sparkleIntensity, this->sparkleUpdateInterval);
}

void LED::resetSparkleTimer() {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Resetting sparkle timer.");
	this->sparkleUpdatePreviousMillis = millis();
}

void LED::setSparkleUpdateInterval(uint16_t updateInterval) {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Setting sparkle update interval to: ");
	DEBUG_PRINT(updateInterval);
	DEBUG_PRINTLN_F(" ms.");
	this->sparkleUpdateInterval = updateInterval;
	resetSparkleTimer();
}

void LED::setSparkleSmoothing(float smoothing) {
	if (0.0 > smoothing || smoothing >= 1.0) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Smoothing factor needs to be higher or equal to 0.0 lower than 1.0, changing the value...");
		this->sparkleSmoothing = constrain(smoothing, 0.0, 1.0);
	}
	else {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINT_F("Setting sparkle smoothing to: ");
		DEBUG_PRINT(smoothing);
		DEBUG_PRINTLN_F(".");
		this->sparkleSmoothing = smoothing;
	}
}
void LED::setSparkleIntensity(BRIGHTNESS_TYPE sparkIntensity) {
	if (sparkIntensity == BRIGHTNESS_TYPE_MIN) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Spark intensity of zero will have no effect, writing the value anyway, but constant mode would be better in this case.");
		this->sparkleIntensity = BRIGHTNESS_TYPE_MIN;
	}
	else if (sparkIntensity > 511) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINTLN_F("Spark intensity should not exceed a value of 127. Changing value to 127.");
		this->sparkleIntensity = 511;
	}
	else {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINT_F("Setting sparkle intensity to: ");
		DEBUG_PRINT(sparkIntensity);
		DEBUG_PRINTLN_F(".");
		this->sparkleIntensity = sparkIntensity;
	}
}

void LED::setSparkleBrightness(BRIGHTNESS_TYPE brightness) {
	if (((int)brightness - (int)this->sparkleIntensity) < BRIGHTNESS_TYPE_MIN) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINT_F("The intensity will result in a brightness below zero. Changing the brightness to ");
		DEBUG_PRINT(this->sparkleIntensity);
		DEBUG_PRINTLN_F(" to prevent this.");
		this->desiredBrightness = this->sparkleIntensity;
	}
	else if ((int)brightness + (int)this->sparkleIntensity > BRIGHTNESS_TYPE_MAX) {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINT_F("The intensity will result in a brightness above BRIGHTNESS_TYPE_MAX. Changing the brightness to ");
		DEBUG_PRINT(BRIGHTNESS_TYPE_MAX - this->sparkleIntensity);
		DEBUG_PRINTLN_F(" to prevent this.");
		this->desiredBrightness = BRIGHTNESS_TYPE_MAX - this->sparkleIntensity;
	}
	else {
		DEBUG_PRINT_HEADER();
		DEBUG_PRINT_F("Setting sparkle brightness to: ");
		DEBUG_PRINT(brightness);
		DEBUG_PRINTLN_F(".");
		this->desiredBrightness = brightness;
	}

	if (this->mode == MODE_OFF || finalBrightness == BRIGHTNESS_TYPE_MIN) {
		this->sparklePreviousBrightness = BRIGHTNESS_TYPE_MIN;
	}
	else {
		this->sparklePreviousBrightness = this->desiredBrightness;
	}
}

uint8_t LED::getMode() {
	return this->mode;
}


void LED::fadeToValue(uint16_t time, BRIGHTNESS_TYPE brightness) {
	if (this->mode == MODE_OFF) {
		this->mode = MODE_CONSTANT;
	}

	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Fading to value: ");
	DEBUG_PRINT(brightness);
	DEBUG_PRINT_F(" within: ");
	DEBUG_PRINT(time);
	DEBUG_PRINTLN_F("ms.");

	setBrightness(brightness);
	int diffBrightness = (int)brightness - (int)this->finalBrightness;

	initFadeTimer(diffBrightness, time);

	resetFadeTimer();

	this->fading = true;
}

void LED::initFadeTimer(int brightnessDifference, uint16_t time) {
	if (abs((float)time / (float)(brightnessDifference)) >= 1.0) {
		this->fadePulseInterval = time / abs(brightnessDifference);
		this->fadeSteps = (brightnessDifference / abs(brightnessDifference));
	}
	else {
		this->fadePulseInterval = 1;
		this->fadeSteps = brightnessDifference / (int)time;
	}

	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("The difference in brightness is: ");
	DEBUG_PRINT(brightnessDifference);
	DEBUG_PRINT_F(" steps: ");
	DEBUG_PRINT(this->fadeSteps);
	DEBUG_PRINT_F(" interval: ");
	DEBUG_PRINT(this->fadePulseInterval);
	DEBUG_PRINTLN_F("ms.");
}

void LED::resetFadeTimer() {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Resetting fade timer.");
	this->fadePulsePreviousMillis = millis();
}


void LED::fadeToMax(uint16_t time)
{
	fadeToValue(time, BRIGHTNESS_TYPE_MAX);
}


void LED::fadeToMin(uint16_t time)
{
	fadeToValue(time, BRIGHTNESS_TYPE_MIN);
}

void LED::fadeToggle(uint16_t time)
{
	DEBUG_PRINT_HEADER();
	DEBUG_PRINTLN_F("Toggling fade.");
	if (this->mode == MODE_CONSTANT) {
		fadeToValue(time, (desiredBrightness > BRIGHTNESS_TYPE_MIN) ? BRIGHTNESS_TYPE_MIN : BRIGHTNESS_TYPE_MAX);
	}
	else if (this->mode == MODE_SPARKLE) {
		if (desiredBrightness <= sparkleIntensity) {
			fadeToValue(time, BRIGHTNESS_TYPE_MAX - sparkleIntensity);
		}
		else if (desiredBrightness >= BRIGHTNESS_TYPE_MAX - sparkleIntensity) {
			fadeToValue(time, sparkleIntensity);
		}
	}
	else if (this->mode == MODE_BLINK) {
		if (desiredBrightness <= (blinkOffBrightness + 1)) {
			fadeToValue(time, BRIGHTNESS_TYPE_MAX);
		}
		else {
			fadeToValue(time, blinkOffBrightness + 1);
		}
	}
}

void LED::fadeWave(uint16_t minToMaxTime, uint16_t numberOfTimes /*= 0*/) {
	if (this->mode == MODE_OFF) {
		this->mode = MODE_CONSTANT;
	}

	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Starting wave with min to max time: ");
	DEBUG_PRINT(minToMaxTime);
	DEBUG_PRINTLN_F("ms.");

	if (numberOfTimes == 0) {
		this->fadeWaveContinuous = true;
		this->fadeWavesRemaining = 0;
	}
	else {
		this->fadeWaveContinuous = false;
		this->fadeWavesRemaining = numberOfTimes;
	}

	setBrightness(BRIGHTNESS_TYPE_MAX);
	this->fadeWaving = true;
	this->fadeWaveMinToMaxTime = minToMaxTime;
	initFadeTimer(BRIGHTNESS_TYPE_MAX, minToMaxTime);
	resetFadeTimer();
	this->fading = true;
}



void LED::fadeStop() {
	DEBUG_PRINT_HEADER();
	DEBUG_PRINT_F("Stopping to fade at brightness: ");
	DEBUG_PRINT(this->finalBrightness);
	DEBUG_PRINTLN_F(".");

	this->fading = false;
	this->fadeWaving = false;
	this->desiredBrightness = this->finalBrightness;
}

bool LED::isFading() {
	return this->fading;
}

bool LED::isFadeWaving() {
	return this->fadeWaving;
}

BRIGHTNESS_TYPE LED::update()
{

	BRIGHTNESS_TYPE finalBrightnessPrevious;

	if (this->mode != MODE_OFF) {
		if (this->fading == true) {
			updateFade();
			this->finalBrightness = this->fadeBrightness;
		}
		else {

			finalBrightnessPrevious = finalBrightness;

			switch (mode) {

			case MODE_CONSTANT:
				this->finalBrightness = this->desiredBrightness;
				break;

			case MODE_BLINK:
				updateBlink();
				this->finalBrightness = this->blinkState ? this->desiredBrightness : this->blinkOffBrightness;
				break;

			case MODE_SPARKLE:
				updateSparkle();
				this->finalBrightness = this->sparkleCurrentBrightness;
				break;
			}
		}

		this->finalBrightnessLimited = map(finalBrightness, BRIGHTNESS_TYPE_MIN, BRIGHTNESS_TYPE_MAX, brightnessLowerLimit, brightnessUpperLimit);
		this->finalBrightnessLimitedCorrected = gamma10[finalBrightnessLimited];
		
		//analogWrite(pin, gammaCorrectionEnabled ? finalBrightnessLimitedCorrected : finalBrightnessLimited);

		if (finalBrightnessPrevious != this->finalBrightness) {

			finalBrightnessPrevious = this->finalBrightness;

			/*
			DEBUG_PRINT_HEADER();
			DEBUG_PRINT_F("The independent brightness is: ");
			DEBUG_PRINT(this->finalBrightness);
			DEBUG_PRINT_F(", the limited brightness is: ");
			DEBUG_PRINT(this->finalBrightnessLimited);
			DEBUG_PRINT_F(", the gamma-corrected brightness is: ");
			DEBUG_PRINT(this->finalBrightnessLimitedCorrected);
			DEBUG_PRINTLN_F(".");
			*/
		}



	}
	else {

		this->finalBrightness = BRIGHTNESS_TYPE_MIN;
		this->finalBrightnessLimited = BRIGHTNESS_TYPE_MIN;
		this->finalBrightnessLimitedCorrected = BRIGHTNESS_TYPE_MIN;

		//analogWrite(pin, BRIGHTNESS_TYPE_MIN);
	}

	return finalBrightnessLimitedCorrected;
}

void LED::updateBlink() {

	// check to see if it's time to change the state of the LED
	unsigned long now = millis();
	unsigned long elapsed;
	unsigned long delay;

	elapsed = now - this->blinkPreviousMillis;

	if (this->blinkOffsetInProgress == true){
		if (elapsed >= this->blinkOffset)
		{
			DEBUG_PRINT_HEADER();
			DEBUG_PRINT_F("Started blinking the LED after an offset of: ");
			DEBUG_PRINT(elapsed);
			DEBUG_PRINTLN(" ms");
			this->blinkPreviousMillis = now;
			this->blinkOffsetInProgress = false;
		}
		else {
			return;
		}
	}

	if (this->blinkOffsetInProgress == false) {
		if ((this->blinkState == true) && (elapsed >= this->blinkOnTime)) {
			DEBUG_PRINT_HEADER();
			DEBUG_PRINT_F("Setting blinking LED to off after ");
			DEBUG_PRINT(elapsed);
			DEBUG_PRINTLN(" ms");
			this->blinkState = false;  // Turn it off
			this->blinkPreviousMillis = now;
		}
		else if ((this->blinkState == false) && (elapsed >= this->blinkOffTime)) {
			DEBUG_PRINT_HEADER();
			DEBUG_PRINT_F("Setting blinking LED to on after ");
			DEBUG_PRINT(elapsed);
			DEBUG_PRINTLN(" ms");
			this->blinkState = true;  // Turn it on
			this->blinkPreviousMillis = now;
		}
		else {
			return;
		}
	}


}


void LED::updateFade() {

	unsigned long now = millis();
	unsigned long elapsed = now - this->fadePulsePreviousMillis;

	if (elapsed >= this->fadePulseInterval) {
		if (abs((int)desiredBrightness - (int)fadeBrightness) < abs(fadeSteps)) {
			DEBUG_PRINT_HEADER();
			DEBUG_PRINT_F("Desired brightness reached, ");
			this->fadeBrightness = this->desiredBrightness;
			this->finalBrightness = this->desiredBrightness;

			if (this->fadeWaving == true) {
				DEBUG_PRINT_F("LED is fade waving, ");
				if (this->fadeWaveContinuous == true) {
					DEBUG_PRINTLN_F("for eternity.");
					fadeToggle(this->fadeWaveMinToMaxTime);
				}
				else {
					if (isMin() == true || (mode == MODE_BLINK && finalBrightness == blinkOffBrightness+1) ) {
						DEBUG_PRINT_F("has reached a minimum, ");
						if (fadeWavesRemaining > 1) {
							fadeWavesRemaining--;
							DEBUG_PRINT_F("and continues for ");
							DEBUG_PRINT(fadeWavesRemaining);
							DEBUG_PRINTLN_F(" times remaining.");
							fadeToggle(this->fadeWaveMinToMaxTime);
						}
						else {
							DEBUG_PRINTLN_F("and has completed the number of waves.");
							fadeWavesRemaining = 0;
							this->fading = false;
							fadeStop();
						}
					}
					else {
						DEBUG_PRINTLN_F("and reached a maximum.");
						fadeToggle(this->fadeWaveMinToMaxTime);
					}
				}
			}
			else {
				DEBUG_PRINTLN_F(".");
				this->fading = false;
			}
		}
		else
		{
			fadeBrightness = fadeBrightness + fadeSteps;
		}
		this->fadePulsePreviousMillis = now;
	}
}


void LED::updateSparkle()
{
	unsigned long now = millis();
	unsigned long elapsed = now - this->sparkleUpdatePreviousMillis;

	if (elapsed >= sparkleUpdateInterval) {
		this->sparkleCurrentBrightness = this->desiredBrightness + random(-this->sparkleIntensity, this->sparkleIntensity);
		this->sparkleCurrentBrightness = (uint16)(this->sparkleSmoothing * (float)this->sparklePreviousBrightness) + (uint16)((1.0 - this->sparkleSmoothing) * (float)this->sparkleCurrentBrightness);
		this->sparklePreviousBrightness = this->sparkleCurrentBrightness;
		//DEBUG_PRINT_HEADER();
		//DEBUG_PRINT_F("New sparkle brightness (unlimited and uncorrected): ");
		//DEBUG_PRINT(this->sparkleCurrentBrightness);
		//DEBUG_PRINTLN(".");
		this->sparkleUpdatePreviousMillis = now;
	}
}
