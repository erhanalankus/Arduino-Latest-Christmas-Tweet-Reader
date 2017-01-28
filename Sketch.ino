#include <WiFi101.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

char ssid[] = "YOUR-WIFI-SSID";
char pass[] = "YOUR-WIFI-PASSWORD";
int status = WL_IDLE_STATUS;
WiFiClient client;
char server[] = "api.twitter.com";

const int button = 6;
const int redLED = 8;
const int greenLED = 7;

const long debounceDelay = 50;
int stateButton = 0;
int lastStateButton = 0;
int readingButton = 0;
long lastDebounceTimeButton = 0;

bool printOn = false;
int countUntilTweet = 0;
bool printCheck = true;
int printedCharacterCount = 0;

Adafruit_PCD8544 display = Adafruit_PCD8544(1, 2, 3, 4, 5);

void setup() {
	Serial.begin(9600);
	pinMode(button, INPUT);
	pinMode(redLED, OUTPUT);
	pinMode(greenLED, OUTPUT);

	display.begin();
	display.setContrast(50);
	display.display();
	delay(1000);

	while (status != WL_CONNECTED)
	{
		display.clearDisplay();
		display.print("Connecting to wifi");
		display.display();
		status = WiFi.begin(ssid, pass);
		delay(10000);
	}
	display.clearDisplay();
	display.print("Press button to read a    Christmas    tweet!");
	display.display();
	Serial.println("hello");
}

void loop() {
	CheckButton();

	if (client.available())
	{
		digitalWrite(redLED, 0);
		digitalWrite(greenLED, 1);
		char c = client.read();
		Serial.print(c);
		if (printOn)
		{
			if (printedCharacterCount == 78)
			{
				display.clearDisplay();
			}
			display.print(c);
			display.display();
			printedCharacterCount++;
			delay(75);
		}
		if (printCheck)
		{
			if (c == '"')
			{
				countUntilTweet++;
				Serial.print(countUntilTweet);
				if (countUntilTweet == 15)
				{
					printOn = true;
				}
				if (countUntilTweet == 16)
				{
					printOn = false;
					printCheck = false;
				}
			}
		}
	}
	else
	{
		digitalWrite(redLED, 1);
		digitalWrite(greenLED, 0);
	}	
}

void CheckButton() {
	readingButton = digitalRead(button);
	if (readingButton != lastStateButton)
	{
		lastDebounceTimeButton = millis();
	}
	if (millis() - lastDebounceTimeButton > debounceDelay)
	{
		if (readingButton != stateButton)
		{
			stateButton = readingButton;
			if (stateButton == LOW)
			{
				Button();
			}
		}
	}
	lastStateButton = readingButton;
}

void Button() {
	display.clearDisplay();
	display.print("Searching...");
	display.display();

	client.stop();
	printCheck = true;
	countUntilTweet = 0;
	printedCharacterCount = 0;
	delay(100);
	if (client.connectSSL(server, 443))
	{
		client.println("GET /1.1/search/tweets.json?q=%40\"merry%20christmas\"&lang=en&result_type=recent&count=1 HTTP/1.1");
		client.println("Host: api.twitter.com");
		client.println("Authorization: Bearer AAAAAAAAAAAAAAAAAAAAAODMywAAAAAAfvZVgnQgq5UapDrh9ahPe1XrXU0%3DKhcrzTjg1IkyYYETQeuBfCgcAPpsdgSv4uhq9CC5L5f57H2Lw2");
		client.println("Connection: close");
		client.println();
		display.clearDisplay();
	}
	else
	{
		display.clearDisplay();
		display.print("Try again");
		display.display();
	}
}
