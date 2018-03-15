
#include "LEDLITLIB.h"

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <Wire.h>
#include <PCA9685.h>

IPAddress	serverIPAddress(192, 168, 0, 75);	// The desired IP Address of this device
IPAddress	networkGateway(192, 168, 0, 1);		// Set gateway to match network settings
IPAddress	networkSubnet(255, 255, 255, 0);	// Set subnet to match network settings

#define			WIFI_AP "WiFi@Home"
#define			WIFI_PASSWORD "@#1A2B3C$%"

const int		WIFI_RETRYDELAY_ms = 5000;
const char*		MQTT_SERVER = "m23.cloudmqtt.com";
const int		MQTT_PORT = 15816;
const char*		MQTT_USER = "gaihleym";
const char*		MQTT_PASSWORD = "lRxHBxLB1z1r";
const char*		MQTT_CLIENTID = "thatsnomoon";

const char*		MQTT_SUBSCRIPTIONS[] = {	"rooms/cedric/control/#",  
											"apps/thatsnomoon/#"
};

const char*		MQTT_BASETOPIC = "thatsnomoon";


WiFiClient espClient;
PubSubClient mqttClient(espClient);

PCA9685 pwmController;

uint8_t wifiStatus = WL_IDLE_STATUS;

unsigned long lastSentTimestamp_ms = 0;
unsigned long lastUpdateTimestamp_ms = 0;

LED shipIncomingStrobes = LED("ShipIncoming", 5, true, 0, 1023);
LED shipIncomingThrusters = LED("ShipThrusters", 4, true, 0, 1023);
LED shipLandingStrobes = LED("ShipLanding", 0, true, 0, 1023);
LED antennaStrobes = LED("Antenna", 2, true, 0, 1023);
LED landingPadSpots = LED("Spots", 14, true, 0, 1023);

LED starStrand1 = LED("StarStrand1", 12, true, 0, 1023);
LED starStrand2 = LED("StarStrand2", 13, true, 0, 1023);
LED starStrand3 = LED("StarStrand3", 15, true, 0, 1023);


void setup()
{

	Serial.begin(115200);

	delay(1500);

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

	Wire.begin();                       // Wire must be started first
	Wire.setClock(400000);              // Supported baud rates are 100kHz, 400kHz, and 1000kHz
	pwmController.resetDevices();       // Software resets all PCA9685 devices on Wire line
	pwmController.init(B000000);        // Address pins A5-A0 set to B010101
	pwmController.setPWMFrequency(1500); // Default is 200Hz, supports 24Hz to 1526Hz

	Serial.println("Complete.");

	Serial.println("Testing outputs.");
	testOutputs();
	Serial.println("Complete.");

	Serial.println("Setting default values.");
	setToDay();
	Serial.println("Complete.");

	Serial.println("Connecting to wifi.");
	initializeWifi();
	Serial.println("Complete.");

	Serial.println("Connecting to MQTT broker.");
	initializeMQTT();
	Serial.println("Complete.");

	delay(100);

	setToDefaults();
}

void loop()
{

	wifiStatus = WiFi.status();
	
	if (wifiStatus != WL_CONNECTED) {
		connectToWifi();
	}
	
	if (!mqttClient.connected()) {
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
	
	mqttClient.loop();

	updateAll();

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

void waveLEDOneTime(LED led) {
	led.fadeWave(5000, 1);
	do {
		pwmController.setChannelPWM(15, led.update() << 2);
		yield();
		delay(10);
	} while (led.isFadeWaving() == true);
	delay(100);

}

void setToOff() {

	char topic[64] = "";
	strcat(topic, MQTT_BASETOPIC);
	strcat(topic, "/mode");

	Serial.println("Setting all outputs to off.");

	shipIncomingStrobes.setToOff();
	shipIncomingThrusters.setToOff();
	shipLandingStrobes.setToOff();
	antennaStrobes.setToOff();
	landingPadSpots.setToOff();
	starStrand1.setToOff();
	starStrand2.setToOff();
	starStrand3.setToOff();

	publishToMQTT(topic, "off");

}

void setToDefaults() {

	Serial.println("Setting all outputs to default.");

	shipIncomingStrobes.setToBlink(200, 1000, 0, 1023, 0);
	shipIncomingThrusters.setToConstant(800); //setToSparkle(0.95, 1023, 500, 20);
	shipLandingStrobes.setToBlink(200, 990, 5000);
	antennaStrobes.setToBlink(100, 2800, 254);
	landingPadSpots.setToConstant(1023);
	starStrand1.setToSparkle(0.7, 1023, 160, 10);
	starStrand2.setToSparkle(0.8, 1023, 240, 10);
	starStrand3.setToSparkle(0.9, 1023, 320, 10);

}

void setToDay() {

	char topic[64] ="";
	strcat(topic, MQTT_BASETOPIC);
	strcat(topic, "/mode");

	Serial.println("Setting all outputs to day-mode.");

	setToDefaults();

	setBrightness(1023);

	publishToMQTT(topic, "day");

}

void setToNight() {

	char topic[64] = "";
	strcat(topic, MQTT_BASETOPIC);
	strcat(topic, "/mode");

	Serial.println("Setting all outputs to night-mode.");

	setToDefaults();

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
}


void initializeWifi() {

	WiFi.config(serverIPAddress, networkGateway, networkSubnet);
	wifiStatus = WiFi.begin(WIFI_AP, WIFI_PASSWORD);

	WiFi.enableAP(false);

	connectToWifi();

}

void connectToWifi() {

	if (wifiStatus != WL_CONNECTED) {
		Serial.print("Trying to connect to wireless access point: \"");
		Serial.print(String(WIFI_AP));
		Serial.print("\" with ");
		Serial.print(WIFI_RETRYDELAY_ms);
		Serial.println("ms retry interval.");
	}
	else {
		return;
	}

	//delay(1000); //Give some time to connect.

	int retries = 0;
	wifiStatus = WiFi.status();

	while (wifiStatus != WL_CONNECTED) {								// As long as the wifi cannot connect, run trought this loop.
		
		switch (wifiStatus)
		{
			case WL_NO_SSID_AVAIL:
				Serial.println("Failed. Retrying...");
				Serial.println("The specified SSID cannot be reached.");

				showAvailableAccessPoints();

				break;

			case WL_NO_SHIELD:
				Serial.println("Failed. Retrying...");
				Serial.println("WiFi shield not present or not accessible.");
				Serial.println("If the shield is present, it could be that the baud-rate is incorrect.");
				break;

			case WL_CONNECT_FAILED:
				
				Serial.println("Failed. Retrying...");
				Serial.println("The specified password, or SSID is incorrect, or the acces point is out of reach.");

				showAvailableAccessPoints();

				break;

			case WL_IDLE_STATUS:

				WiFi.begin(WIFI_AP, WIFI_PASSWORD);
				break;

			default:
				Serial.println("Failed. Retrying...");
				showAvailableAccessPoints();

				break;
		}

		wifiStatus = WiFi.status(); //
		retries++;
		delay(WIFI_RETRYDELAY_ms);
	}
	Serial.println("Success!");
	Serial.print("Connected to: \"");
	Serial.print(String(WIFI_AP));
	Serial.print("\" with IP4: \"");
	Serial.print(WiFi.localIP());
	Serial.println("\".");
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
				else if (strcmp(token, "incomingthrusters") == 0)
				{
					token = strtok(NULL, "/");
					processLED(token, "thatsnomoon/incomingthrusters", shipIncomingThrusters, payloadBuffer);
				}
				else if (strcmp(token, "stars1") == 0)
				{
					token = strtok(NULL, "/");
					processLED(token, "thatsnomoon/stars1", starStrand1, payloadBuffer);
				}
				else if (strcmp(token, "stars2") == 0)
				{
					token = strtok(NULL, "/");
					processLED(token, "thatsnomoon/stars2", starStrand2, payloadBuffer);
				}
				else if (strcmp(token, "stars3") == 0)
				{
					token = strtok(NULL, "/");
					processLED(token, "thatsnomoon/stars3", starStrand3, payloadBuffer);
				}
				else if (strcmp(token, "incomingstrobes") == 0)
				{
					token = strtok(NULL, "/");
					processLED(token, "thatsnomoon/incomingstrobes", shipIncomingStrobes, payloadBuffer);
				}
				else if (strcmp(token, "landingstrobes") == 0)
				{
					token = strtok(NULL, "/");
					processLED(token, "thatsnomoon/landingstrobes", shipLandingStrobes, payloadBuffer);
				}
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


void processLED(char *token, const char* topic, LED &led, char * payloadBuffer) {

	char topicBuffer[64] = "";
	strcpy(topicBuffer, topic);

	if (strcmp(token, "brightness") == 0) {

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

		strcat(topicBuffer, "/variation");

		publishToMQTT(topicBuffer, led.getSparkleIntensity());

		token = strtok(NULL, "/");
	}
	else if (strcmp(token, "smoothing") == 0) {

		Serial.print("Setting smoothing to: ");
		Serial.println(payloadBuffer);

		float smoothing = atof(payloadBuffer);

		led.setSparkleSmoothing(smoothing);

		strcat(topicBuffer, "/smoothing");

		publishToMQTT(topicBuffer, led.getSparkleSmoothing());

		token = strtok(NULL, "/");
	}
	else if (strcmp(token, "ontime") == 0) {

		Serial.print("Setting on time to: ");
		Serial.println(payloadBuffer);

		unsigned int onTime = (unsigned int)atoi(payloadBuffer);

		led.setBlinkOnTime(onTime);

		strcat(topicBuffer, "/ontime");

		publishToMQTT(topicBuffer, led.getBlinkOnTime());

		token = strtok(NULL, "/");
	}
	else if (strcmp(token, "offtime") == 0) {

		Serial.print("Setting off time to: ");
		Serial.println(payloadBuffer);

		unsigned int offTime = (unsigned int)atoi(payloadBuffer);

		led.setBlinkOffTime(offTime);

		strcat(topicBuffer, "/offtime");

		publishToMQTT(topicBuffer, led.getBlinkOffTime());

		token = strtok(NULL, "/");
	}
	else if (strcmp(token, "offsettime") == 0) {

		Serial.print("Setting offset time to: ");
		Serial.println(payloadBuffer);

		unsigned int offsetTime = (unsigned int)atoi(payloadBuffer);

		led.setBlinkOffset(offsetTime);

		strcat(topicBuffer, "/offsettime");

		publishToMQTT(topicBuffer, led.getBlinkOffset());

		token = strtok(NULL, "/");
	}

}


void publishToMQTT(char * topic, char * message) {
	Serial.print(mqttClient.publish(topic, message) ? "Published: '" : "Failed to publish: '");
	Serial.print(message);
	Serial.print("' on topic: '");
	Serial.print(topic);
	Serial.println("'.");
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
	}
}

void connectToMQTT() {
	// Loop until connected
	while (!mqttClient.connected()) {

		Serial.print(F("Trying to connect to MQTT broker as '"));
		Serial.print(MQTT_CLIENTID);
		Serial.println(F("."));

		Serial.print(F("MQTT broker IP4:'"));
		Serial.print(MQTT_SERVER);
		Serial.print(F("' port: '"));
		Serial.print(MQTT_PORT);
		Serial.println(F("'."));

		// Attempt to connect (clientId, username, password)
		if (mqttClient.connect(MQTT_CLIENTID, MQTT_USER, MQTT_PASSWORD)) {
			Serial.println("Success!");
		}
		else {
			Serial.print("Failed. Client state: ");
			Serial.print(mqttClient.state());
			Serial.println(". Retrying in 5 seconds.");
			delay(5000);
		}

		mqttClient.loop();
	}

	delay(1000);

	mqttClient.publish("thatsnomoon/status", "ok");
	subscribeToMQTTTopics();
}

void initializeMQTT() {

	lastSentTimestamp_ms = 0;
	mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
	mqttClient.setCallback(onMQTTMessageReceived);
	connectToMQTT();

}

void showAvailableAccessPoints() {

	int8_t numberOfAPs = WiFi.scanNetworks();

	Serial.println("Found the following access points:");
	Serial.println();

	for (int8_t i = 0; i < numberOfAPs; i++)
	{
		Serial.print("--> '");
		Serial.print(WiFi.SSID(i));
		Serial.print("', signal strength: ");
		Serial.print(WiFi.RSSI(i));
		Serial.print("dBm ::: ");
		if (WiFi.RSSI(i) >= -50)
		{
			Serial.println("Excellent.");
		}
		else if (WiFi.RSSI(i) >= -60)
		{
			Serial.println("Good.");
		}
		else if (WiFi.RSSI(i) >= -70)
		{
			Serial.println("Fair.");
		}
		else
		{
			Serial.println("Bad.");
		}
	}

	Serial.println();
}

