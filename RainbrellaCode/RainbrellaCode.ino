/*
* CPSC 599 Final Project
* 
* RAINBRELLA
* By Sarah Walker
* 10162666
*
* This program uses a reed switch, temperature and humidity sensor, photoresistor, and a 
* relay to control a water pump. The reed switch is used to determine when an umbrella is open,
* which then triggers a value read from the temp/humidity sensor and photoresistor. If the photoresistor 
* has a value above 600, the temperature is above 25 degrees C, and the 
* humidity is beow 80%, the weather is considered good and the relay will be activated, thus triggering 
* the water pump.
*
*/

// Assuming that the water bladder is full, there are approximately 150 activations 
// of the spray before water runs out. This is to ensure that we don't run the pump
// when there is no water
#define ALLOWABLE_RUNS 150
int totalRuns = 0;

// Temperature and humidity sensor
#include "DHT.h"
#define DHTTYPE DHT11  

// LED declarations
#include <FastLED.h>
#define LED_PIN  8
#define NUM_LEDS    16
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
CRGB leds[NUM_LEDS];

// Sensor pins
const int SUN_SENSOR_PIN = A0;
const int OPEN_PIN = 4;
const int DHTPIN = 2;

// Setting up the temp/humidity sensor
DHT dht(DHTPIN, DHTTYPE);

const int RELAY_PIN = 7;

// Values that will be read
int sunValue;
float humidity;
float temperature;

// Threshold values - read values below these are considered
// bad weather for sun and temperature thresholds, and read values above humidity
// threshold are considered bad weather
const int sunThreshold = 600;
const float humidityThreshold = 80;
const float temperatureThreshold = 25;


void setup() {
  Serial.begin(9600);
  
  dht.begin();

  // Set sensors as input and relay (to water pump) as output
  pinMode(SUN_SENSOR_PIN,INPUT);
  pinMode(OPEN_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN,OUTPUT);
  stopSpray();

  // Setup LEDs
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Check if umbrella is open
  while(digitalRead(OPEN_PIN) == LOW)
  {
    sunValue = analogRead(SUN_SENSOR_PIN);
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();
  
    // Check if reading the sensors has failed
    // If so, restart the loop and try reading again
    if (isnan(humidity) || isnan(temperature)) 
    {
      return;
    }
    
    // Check for good weather && whether the water bladder has water left
    if(sunValue > sunThreshold && temperature > temperatureThreshold && humidity < humidityThreshold && totalRuns < ALLOWABLE_RUNS)
    {
      // Turn on LEDs
      leds[0].setRGB(50,0,255);
      leds[1].setRGB(50,0,255);
      leds[2].setRGB(50,0,255);
      leds[3].setRGB(50,0,255);
      FastLED.show();
      
      // Start the spray
      // Note: to ensure that the water bladder doesn't empty completely,
      //       the spray will only be activated for a maximum of 150*500 ms
      startSpray();
      delay(500);
      stopSpray();
      totalRuns++;

      FastLED.clear();
      FastLED.delay(1000 / UPDATES_PER_SECOND);
    } 
  }
}

void startSpray()
{
  digitalWrite(RELAY_PIN, LOW);
}

void stopSpray()
{
  digitalWrite(RELAY_PIN, HIGH);
}
