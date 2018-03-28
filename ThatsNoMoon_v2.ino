
#include <EEPROM.h>
#include "LEDLITLIB.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>

#include <Wire.h>
#include <PCA9685.h>

IPAddress	serverIPAddress(192, 168, 0, 75);	// The desired IP Address of this device
IPAddress	networkGateway(192, 168, 0, 1);		// Set gateway to match network settings
IPAddress	networkSubnet(255, 255, 255, 0);	// Set subnet to match network settings

unsigned long previousMQTTConnectionAttempt = 0;
unsigned long retryMQTTConnectionInterval = 30000; //ms

char*		mqttServer = "m23.cloudmqtt.com";
int			mqttPort = 15816;
char*		mqttUsername = "gaihleym";
char*		mqttPassword = "lRxHBxLB1z1r";

int tempPrev = 0;

const char*	MQTT_CLIENTID = "thatsnomoon";

const char*	MQTT_SUBSCRIPTIONS[] = {	"rooms/cedric/control/#",  
											"apps/thatsnomoon/#"
};

const char*	MQTT_BASETOPIC = "thatsnomoon";

WiFiClient		espClient;
PubSubClient	mqttClient(espClient);
WiFiManager		wifiManager;

PCA9685 pwmController;

uint8_t wifiStatus = WL_IDLE_STATUS;

unsigned long lastSentTimestamp_ms = 0;
unsigned long lastUpdateTimestamp_ms = 0;

LED shipIncomingStrobes =		LED("ShipIncoming", 5, true, 0, 1023);
LED shipIncomingThrusters =		LED("ShipThrusters", 4, true, 0, 1023);
LED shipLandingStrobes =		LED("ShipLanding", 0, true, 0, 1023);
LED antennaStrobes =			LED("Antenna", 2, true, 0, 1023);
LED landingPadSpots =			LED("Spots", 14, true, 0, 1023);

LED starStrand1 =				LED("StarStrand1", 12, true, 0, 1023);
LED starStrand2 =				LED("StarStrand2", 13, true, 0, 1023);
LED starStrand3 =				LED("StarStrand3", 15, true, 0, 1023);

const uint16_t					EEPROM_ADDRESS_SHIPINCOMINGSTROBES		= 0;
const uint16_t					EEPROM_ADDRESS_SHIPINCOMINGTHRUSTERS	= 32;
const uint16_t					EEPROM_ADDRESS_SHIPLANDINGSTROBES		= 64;
const uint16_t					EEPROM_ADDRESS_ANTENNASTROBES			= 96;
const uint16_t					EEPROM_ADDRESS_LANDINGPADSPOTS			= 128;
const uint16_t					EEPROM_ADDRESS_STARSTRAND1				= 160;
const uint16_t					EEPROM_ADDRESS_STARSTRAND2				= 192;
const uint16_t					EEPROM_ADDRESS_STARSTRAND3				= 224;


void setup()
{
	Serial.begin(921600);

	//Wait for a short time.
	yield();

	//Print a nice looking header:
	Serial.println(F("_____________________________________________________"));
	Serial.println();
	Serial.println(F("          -'That's no moon... It's a space station.'-"));
	Serial.println();
	Serial.println(F("                             -'Obi-Wan 'Ben' Kenobi'-"));
	Serial.println(F("                             -'Star Wars Episode IV'-"));
	Serial.println(F("                             -'A New Hope,     1977'-"));
	Serial.println();
	Serial.println(F("_____________________________________________________"));
	Serial.println();

	Serial.println("Initializing LEDs.");

	Wire.begin();							// Wire must be started first
	Wire.setClock(400000);					// Supported baud rates are 100kHz, 400kHz, and 1000kHz

	pwmController.resetDevices();			// Software resets all PCA9685 devices on Wire line
	pwmController.init(B000000);			// Address pins A5-A0 set to B010101
	pwmController.setPWMFrequency(1500);	// Default is 200Hz, supports 24Hz to 1526Hz

	Serial.println("Complete.");

	//Fade all the different outputs up and down, to test them.
	Serial.println("Testing outputs.");
	testOutputs();
	Serial.println("Complete.");

	//Init EEPROM 
	//Take 32 bytes per led. To have some spare. -> total of 8 leds -> 256 bytes for EEPROM
	EEPROM.begin(256);

	//Set the painting to day mode, to load the default values.
	Serial.println("Setting default values.");
	setToDay();
	Serial.println("Complete.");

	//Startup the wifi connection.
	Serial.println("Connecting to wifi.");
	initializeWifi();
	Serial.println("Complete.");

	//Connect to the MQTT server/broker.
	Serial.println("Connecting to MQTT broker.");
	initializeMQTT();
	Serial.println("Complete.");

	delay(100);

	setToDefaults();

	loadAllLEDs();
}

void loop()
{

	if (!mqttClient.connected()) {

		antennaStrobes.setToOff();
		connectToMQTT();
	}
	
	unsigned long now = millis();
	unsigned long elapsed = now - lastSentTimestamp_ms;
	
	if (elapsed >= 10000) { 

		char topic[64] = "";
		char message[64] = "";

		strcat(topic, MQTT_BASETOPIC);
		strcat(topic, "/wifi/rssi");

		publishToMQTT(topic, WiFi.RSSI());

		strcpy(topic, "");
		strcat(topic, MQTT_BASETOPIC);
		strcat(topic, "/wifi/ap");

		WiFi.SSID().toCharArray(message, 64);

		publishToMQTT(topic, message);

		lastSentTimestamp_ms = millis();
	}

	updateAll();

	mqttClient.loop();
}

void testOutputs() {

	waveLEDOneTime(shipIncomingStrobes);
	waveLEDOneTime(shipIncomingThrusters);
	waveLEDOneTime(shipLandingStrobes);
	waveLEDOneTime(antennaStrobes);
	waveLEDOneTime(landingPadSpots);
	waveLEDOneTime(starStrand1);
	waveLEDOneTime(starStrand2);
	waveLEDOneTime(starStrand3);

}

void waveLEDOneTime(LED &led) {
	Serial.println("waving led");
	led.fadeWave(50, 1);
	do {

		updateAll();

		yield();
	} while (led.isFadeWaving() == true);
	delay(100);
	Serial.println("ended waving led");
}

void setToOff() {

	char topic[64] = "";
	strcat(topic, MQTT_BASETOPIC);
	strcat(topic, "/mode");

	Serial.println("Setting all outputs to off.");

	shipIncomingStrobes		.setToOff();
	shipIncomingThrusters	.setToOff();
	shipLandingStrobes		.setToOff();
	antennaStrobes			.setToOff();
	landingPadSpots			.setToOff();
	starStrand1				.setToOff();
	starStrand2				.setToOff();
	starStrand3				.setToOff();

	publishToMQTT(topic, "off");

}

void setToDefaults() {

	Serial.println("Setting all outputs to default.");

	shipIncomingStrobes			.setToBlink(200, 1000, 0, 1023, 0);
	shipIncomingThrusters		.setToSparkle(0.95, 1023, 500, 20);
	shipLandingStrobes			.setToBlink(200, 990, 5000);
	antennaStrobes				.setToBlink(100, 2800, 254);
	landingPadSpots				.setToConstant(1023);
	starStrand1					.setToSparkle(0.7, 1023, 160, 10);
	starStrand2					.setToSparkle(0.8, 1023, 240, 10);
	starStrand3					.setToSparkle(0.9, 1023, 320, 10);

}

//Take 32 bytes per led. To have some spare. -> total of 8 leds -> 256 bytes for EEPROM
struct {
	uint16_t brightness;	//2 bytes
	uint16_t variation;		//2 bytes
	uint16_t onTime;		//2 bytes
	uint16_t offTime;		//2 bytes
	uint16_t offsetTime;	//2 bytes
	float smoothing;		//4 bytes
} ledParameters;

void saveLED(LED &led, uint16_t startingAddress, bool commit) {

	ledParameters.brightness	= led.getBrightness();
	ledParameters.offsetTime	= led.getBlinkOffset();
	ledParameters.onTime		= led.getBlinkOnTime();
	ledParameters.offTime		= led.getBlinkOffTime();
	ledParameters.variation		= led.getSparkleIntensity();
	ledParameters.smoothing		= led.getSparkleSmoothing();

	Serial.printf("Saving led %s with parameters:\n\tBrightness: %i \n\tOffset: %i ms \n\tOffTime: %i ms \n\tOnTime: %i ms\n",
		led.ID,
		ledParameters.brightness,
		ledParameters.offsetTime,
		ledParameters.offTime,
		ledParameters.onTime
	);

	EEPROM.put(startingAddress, ledParameters);

	if (commit) {
		EEPROM.commit();
	}
}

void loadLED(LED &led, uint16_t startingAddress) {

	EEPROM.get(startingAddress, ledParameters);

	Serial.printf("Loaded led %s parameters:\n\tBrightness: %i \n\tOffset: %i ms \n\tOffTime: %i ms \n\tOnTime: %i ms\n",
		led.ID,
		ledParameters.brightness,
		ledParameters.offsetTime,
		ledParameters.offTime,
		ledParameters.onTime
	);

	led.setBrightness(ledParameters.brightness);
	led.setBlinkOffset(ledParameters.offsetTime);
	led.setBlinkOnTime(ledParameters.onTime);
	led.setBlinkOffTime(ledParameters.offTime);
	led.setSparkleIntensity(ledParameters.variation);
	led.setSparkleSmoothing(ledParameters.smoothing);

}

void saveAllLEDs() {

	Serial.println("Saving all LEDs ...");

	saveLED(shipIncomingStrobes,		EEPROM_ADDRESS_SHIPINCOMINGSTROBES, 	false);
	saveLED(shipIncomingThrusters,		EEPROM_ADDRESS_SHIPINCOMINGTHRUSTERS,	false);
	saveLED(shipLandingStrobes,			EEPROM_ADDRESS_SHIPLANDINGSTROBES, 		false);
	saveLED(antennaStrobes,				EEPROM_ADDRESS_ANTENNASTROBES, 			false);
	saveLED(landingPadSpots,			EEPROM_ADDRESS_LANDINGPADSPOTS,			false);
	saveLED(starStrand1,				EEPROM_ADDRESS_STARSTRAND1, 			false);
	saveLED(starStrand2,				EEPROM_ADDRESS_STARSTRAND2, 			false);
	saveLED(starStrand3,				EEPROM_ADDRESS_STARSTRAND3, 			false);

	EEPROM.commit();

	Serial.println("Saving all LEDs complete!");
}

void loadAllLEDs() {

	Serial.println("Loading all LEDs ...");

	loadLED(shipIncomingStrobes,		EEPROM_ADDRESS_SHIPINCOMINGSTROBES			);
	loadLED(shipIncomingThrusters,		EEPROM_ADDRESS_SHIPINCOMINGTHRUSTERS		);
	loadLED(shipLandingStrobes,			EEPROM_ADDRESS_SHIPLANDINGSTROBES 			);
	loadLED(antennaStrobes,				EEPROM_ADDRESS_ANTENNASTROBES 				);
	loadLED(landingPadSpots,			EEPROM_ADDRESS_LANDINGPADSPOTS				);
	loadLED(starStrand1,				EEPROM_ADDRESS_STARSTRAND1 					);
	loadLED(starStrand2,				EEPROM_ADDRESS_STARSTRAND2 					);
	loadLED(starStrand3,				EEPROM_ADDRESS_STARSTRAND3 					);

	Serial.println("Loading all LEDs complete!");
}

void setToDay() {

	char topic[64] ="";
	strcat(topic, MQTT_BASETOPIC);
	strcat(topic, "/mode");

	Serial.println("Setting all outputs to day-mode.");

	setBrightness(1023);

	publishToMQTT(topic, "day");

}

void setToNight() {

	char topic[64] = "";
	strcat(topic, MQTT_BASETOPIC);
	strcat(topic, "/mode");

	Serial.println("Setting all outputs to night-mode.");

	setBrightness(512);

	publishToMQTT(topic, "night");

}

void setBrightness(unsigned int value) {

	Serial.print("Setting overal brightness to: ");
	Serial.print(value);
	Serial.println(".");

	shipIncomingStrobes.setUpperLimit(value);
	shipIncomingThrusters.setUpperLimit(value);
	shipLandingStrobes.setUpperLimit(value);
	antennaStrobes.setUpperLimit(value);
	landingPadSpots.setUpperLimit(value);
	starStrand1.setUpperLimit(value);
	starStrand2.setUpperLimit(value);
	starStrand3.setUpperLimit(value);
}

void updateAll() {

	shipIncomingStrobes.update();
	shipIncomingThrusters.update();
	shipLandingStrobes.update();
	antennaStrobes.update();
	landingPadSpots.update();
	starStrand1.update();
	starStrand2.update();
	starStrand3.update();

	unsigned long now = millis();

	if (now - lastSentTimestamp_ms >= 20) {

		uint16_t pwms[5];

		pwms[0] = shipIncomingThrusters.getGammaCorrectedBrightness() * 4;
		pwms[1] = landingPadSpots.getGammaCorrectedBrightness() * 4;
		pwms[2] = 0;
		pwms[3] = shipIncomingStrobes.getGammaCorrectedBrightness() * 4;
		pwms[4] = antennaStrobes.getGammaCorrectedBrightness() * 4;

		pwmController.setChannelsPWM(8, 5, pwms);

		pwms[0] = starStrand1			.getGammaCorrectedBrightness() *4;//Ok
		pwms[1] = starStrand2			.getGammaCorrectedBrightness() * 4;//ok
		pwms[2] = starStrand3			.getGammaCorrectedBrightness() * 4;//ok
		pwms[3] = shipLandingStrobes	.getGammaCorrectedBrightness() * 4;

		pwmController.setChannelsPWM(0, 4, pwms);

		yield();



		int temp = landingPadSpots.getGammaCorrectedBrightness() * 4;
		if (tempPrev != temp) {
			Serial.println(landingPadSpots.getGammaCorrectedBrightness() * 4);
		}

		tempPrev = temp;

		lastUpdateTimestamp_ms = now;

		yield();

		delay(8);
	}


}


void initializeWifi() {

	char portString[10];
	itoa(mqttPort, portString, 10);

	//wifiManager.resetSettings();

	WiFiManagerParameter customMQTTServer("mqttServer", "MQTT Broker Address", mqttServer, 40);
	wifiManager.addParameter(&customMQTTServer);

	WiFiManagerParameter customMQTTPort("mqttPort", "MQTT Broker Port", portString, 10);
	wifiManager.addParameter(&customMQTTPort);

	WiFiManagerParameter customMQTTUsername("mqttUsernameNAME", "MQTT Broker Username", mqttUsername, 40);
	wifiManager.addParameter(&customMQTTUsername);

	WiFiManagerParameter customMQTTPassword("mqttPassword", "MQTT Broker Password", mqttPassword, 40);
	wifiManager.addParameter(&customMQTTPassword);

	wifiManager.setDebugOutput(true);

	wifiManager.setConfigPortalTimeout(180);

	wifiManager.autoConnect("ThatsNoMoonAP");

	mqttServer = (char *) customMQTTServer.getValue();
	mqttUsername = (char *) customMQTTUsername.getValue();

	mqttPort = atoi(customMQTTPort.getValue());

	mqttPassword = (char *) customMQTTPassword.getValue();

}

void onMQTTMessageReceived(char * topic, byte * payload, unsigned int length) {

	char topicBuffer[64] = "";

	char payloadBuffer[64] = "";

	Serial.print("Message arrived on topic: '");

	Serial.print(topic);

	Serial.print("' message: '");

	for (int i = 0; i < length; i++) {
		payloadBuffer[i] = (char)payload[i];
		Serial.print((char)payload[i]);
	}

	Serial.println("'.");

	strcpy(topicBuffer, topic);

	char *token = strtok(topicBuffer, "/");

	while (token != NULL)
	{
		Serial.print("subtopic:");
		Serial.println(token);

		if (strcmp(token, "thatsnomoon") == 0) {
			Serial.println("found thatsnomoon subtopic.");

			token = strtok(NULL, "/");
			Serial.print("subtopic:");
			Serial.println(token);

			if (token != NULL) {

				if (strcmp(token, "mode") == 0)
				{
					if (strcmp(payloadBuffer, "off") == 0) {
						setToOff();
					}
					else if (strcmp(payloadBuffer, "day") == 0) {
						setToDay();
					}
					else if (strcmp(payloadBuffer, "night") == 0) {
						setToNight();
					}
					else {
						Serial.println("Topic recognized but message is not.");
					}

					token = strtok(NULL, "/");
				}
				else if (strcmp(token, "brightness") == 0)
				{
					unsigned int brightness = (unsigned int)atoi(payloadBuffer);
					setBrightness(brightness);
					token = strtok(NULL, "/");
				}
				else if (strcmp(token, "save") == 0)
				{
					saveAllLEDs();
					token = strtok(NULL, "/");
				}
				else if (strcmp(token, "load") == 0)
				{
					saveAllLEDs();
					token = strtok(NULL, "/");
				}
				else if (processLED(token, "incomingthrusters",		"thatsnomoon/incomingthrusters",		shipIncomingThrusters,		payloadBuffer,		EEPROM_ADDRESS_SHIPINCOMINGTHRUSTERS))	{return;}
				else if (processLED(token, "stars1",				"thatsnomoon/stars1",					starStrand1,				payloadBuffer,		EEPROM_ADDRESS_STARSTRAND1 ))			{return;}
				else if (processLED(token, "stars2",				"thatsnomoon/stars2",					starStrand2,				payloadBuffer,		EEPROM_ADDRESS_STARSTRAND2))			{return;}
				else if (processLED(token, "stars3",				"thatsnomoon/stars3",					starStrand2,				payloadBuffer,		EEPROM_ADDRESS_STARSTRAND3))			{return;}
				else if (processLED(token, "incomingstrobes",		"thatsnomoon/incomingstrobes",			shipIncomingStrobes,		payloadBuffer,		EEPROM_ADDRESS_SHIPINCOMINGSTROBES ))	{return;}
				else if (processLED(token, "landingstrobes",		"thatsnomoon/landingstrobes",			shipLandingStrobes,			payloadBuffer,		EEPROM_ADDRESS_SHIPLANDINGSTROBES ))	{return;}
				else if (processLED(token, "platformspots",			"thatsnomoon/platformspots",			landingPadSpots,			payloadBuffer,		EEPROM_ADDRESS_LANDINGPADSPOTS ))		{return;}
				else
				{
					Serial.println("Topic is unhandled.");
					token = strtok(NULL, "/");
				}
			}
			else {
				Serial.println("Found no subtopic of 'thatsnomoon'");
			}
		}
		else {
			token = strtok(NULL, "/");
		}
	}
}

//Returns a boolean true if the name matches the current subtopic.
bool processLED(char *token, const char * name, const char* topic, LED &led, char * payloadBuffer, uint16_t eepromAddress) {

	if (strcmp(token, name) == 0){ //Compares the name with the current (sub) topic. 'strcmp' returns logic false if the string is matched.
		token = strtok(NULL, "/");

		char topicBuffer[64] = "";
		strcpy(topicBuffer, topic);

		if (strcmp(payloadBuffer, "save") == 0) {

			saveLED(led, eepromAddress, true);

		}
		else if (strcmp(payloadBuffer, "load") == 0) {

			loadLED(led, eepromAddress);

		}
		else if (strcmp(token, "brightness") == 0) {

			Serial.print("Setting brightness to: ");
			Serial.println(payloadBuffer);

			unsigned int brightness = (unsigned int)atoi(payloadBuffer);

			led.setBrightness(brightness);

			strcat(topicBuffer, "/brightness");

			publishToMQTT(topicBuffer, led.getDesiredBrightness());

			token = strtok(NULL, "/");
		}
		else if (strcmp(token, "variation") == 0) {

			Serial.print("Setting variation to: ");
			Serial.println(payloadBuffer);

			unsigned int variation = (unsigned int)atoi(payloadBuffer);

			led.setSparkleIntensity(variation);

			token = strtok(NULL, "/");
		}
		else if (strcmp(token, "smoothing") == 0) {

			Serial.print("Setting smoothing to: ");
			Serial.println(payloadBuffer);

			float smoothing = atof(payloadBuffer);

			led.setSparkleSmoothing(smoothing);

			token = strtok(NULL, "/");
		}
		else if (strcmp(token, "ontime") == 0) {

			Serial.print("Setting on time to: ");
			Serial.println(payloadBuffer);

			unsigned int onTime = (unsigned int)atoi(payloadBuffer);

			led.setBlinkOnTime(onTime);

			token = strtok(NULL, "/");
		}
		else if (strcmp(token, "offtime") == 0) {

			Serial.print("Setting off time to: ");
			Serial.println(payloadBuffer);

			unsigned int offTime = (unsigned int)atoi(payloadBuffer);

			led.setBlinkOffTime(offTime);

			token = strtok(NULL, "/");
		}
		else if (strcmp(token, "offsettime") == 0) {

			Serial.print("Setting offset time to: ");
			Serial.println(payloadBuffer);

			unsigned int offsetTime = (unsigned int)atoi(payloadBuffer);

			led.setBlinkOffset(offsetTime);

			token = strtok(NULL, "/");
		}
		else
		{
			//Do nothing;
		}

		publishLEDVariables(topic, led);

		return true;
	}
	else {
		return false;
	}
}

void publishLEDVariables(const char* topic, LED &led) {

	char topicBuffer[64] = "";
	strcpy(topicBuffer, topic);

	strcat(topicBuffer, "/variation");
	publishToMQTT(topicBuffer, led.getSparkleIntensity());

	memset(topicBuffer, 0, sizeof(topicBuffer));
	strcpy(topicBuffer, topic);

	strcat(topicBuffer, "/smoothing");
	publishToMQTT(topicBuffer, led.getSparkleSmoothing());

	memset(topicBuffer, 0, sizeof(topicBuffer));
	strcpy(topicBuffer, topic);

	strcat(topicBuffer, "/ontime");
	publishToMQTT(topicBuffer, led.getBlinkOnTime());

	memset(topicBuffer, 0, sizeof(topicBuffer));
	strcpy(topicBuffer, topic);

	strcat(topicBuffer, "/offtime");
	publishToMQTT(topicBuffer, led.getBlinkOffTime());

	memset(topicBuffer, 0, sizeof(topicBuffer));
	strcpy(topicBuffer, topic);

	strcat(topicBuffer, "/offsettime");
	publishToMQTT(topicBuffer, led.getBlinkOffset());

}


void publishToMQTT(char * topic, char * message) {
	if (mqttClient.connected()) {
		Serial.print(mqttClient.publish(topic, message) ? "Published: '" : "Failed to publish: '");
		Serial.print(message);
		Serial.print(F("' on topic: '"));
		Serial.print(topic);
		Serial.println(F("."));
	}
	else {
		Serial.print(F("ERR::Tried to send MQTT message: '")); 
		Serial.print(message); 
		Serial.print(F("' on topic: '"));
		Serial.print(topic); 
		Serial.println(F("', but there is no connection with the MQTT server"));
	}
}

void publishToMQTT(char * topic, double number) {
	char str[16] = "";
	dtostrf(number, 6, 6, str);
	publishToMQTT(topic, str);
}

void publishToMQTT(char * topic, int number) {
	char str[16] = "";
	sprintf(str, "%d", number);
	publishToMQTT(topic, str);
}

void publishToMQTT(char * topic, unsigned int number) {
	char str[16] = "";
	sprintf(str, "%u", number);
	publishToMQTT(topic, str);
}

void subscribeToMQTTTopics() {
	for (int i = 0; i < (sizeof(MQTT_SUBSCRIPTIONS) / sizeof(int)); i++) {
		bool succes = mqttClient.subscribe(MQTT_SUBSCRIPTIONS[i]);
		Serial.print(succes ? "Succesfully subscribed to: '" : "Failed to subscribe to: '");
		Serial.print(MQTT_SUBSCRIPTIONS[i]);
		Serial.print(".");
		mqttClient.loop();
		Serial.println();
		yield();
	}
}

void connectToMQTT() {

	unsigned long now = millis();

	if (now - previousMQTTConnectionAttempt >= retryMQTTConnectionInterval) {

		if (WiFi.isConnected() == true) {

			Serial.print(F("Trying to connect to MQTT broker as '"));
			Serial.print(MQTT_CLIENTID);
			Serial.println(F("."));

			Serial.print(F("MQTT broker:'"));
			Serial.print(mqttServer);
			Serial.print(F("' port: '"));
			Serial.print(mqttPort);
			Serial.println(F("'."));

			mqttClient.loop();

			// Attempt to connect (clientId, username, password)
			if (mqttClient.connect(MQTT_CLIENTID, mqttUsername, mqttPassword)) {

				Serial.println("Success!");
				mqttClient.publish("thatsnomoon/status", "ok");
				subscribeToMQTTTopics();
				antennaStrobes.setToBlink(100, 2800, 254);

				mqttClient.loop();

			}
			else {

				Serial.print("Failed. Client state: ");
				Serial.print(mqttClient.state());
				Serial.println(". Retrying in 5 seconds.");
			}
		}
		else {
			Serial.println(F("Not connected to a wifi access point. Reset the device to startup the configuration AP."));
		}

		previousMQTTConnectionAttempt = now;
	}
}

void initializeMQTT() {

	lastSentTimestamp_ms = 0;
	mqttClient.setServer(mqttServer, mqttPort);
	mqttClient.setCallback(onMQTTMessageReceived);
	connectToMQTT();

}

