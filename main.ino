#include <LiquidCrystal.h>
#include <string.h>

// #define SERIAL_TESTING
#define SERIAL_PORT 9600

#define echoPin 9
#define trigPin 8

#define buzzerPin 10

#define lcd_RS_Pin 12
#define lcd_EN_Pin 11
#define lcd_DATA4_Pin 5
#define lcd_DATA5_Pin 4
#define lcd_DATA6_Pin 3
#define lcd_DATA7_Pin 2
#define lcd_XCount 16
#define lcd_YCount 2

#define LOOP_DELAY 50 // milisecond(s)
#define DISTANCE_CHARACTER "|"
#define DISTANCE_SECURE "SAFE"
#define DISTANCE_SECURE2 "GUVENLI"
#define DISTANCE_UNIT_TEXT "CM."

const size_t maxRange = 150; // cm
const size_t minRange = 0; // cm

int BUZZER_FREQUENCY = 400;

LiquidCrystal lcd(lcd_RS_Pin, lcd_EN_Pin, lcd_DATA4_Pin, lcd_DATA5_Pin, lcd_DATA6_Pin, lcd_DATA7_Pin);

struct LCDPixel { byte bytes[8]; };
LCDPixel car[4][2] = {
	{ // 4
		{ // 2
			B00000,
			B00000,
			B00001,
			B00011,
			B00110,
			B01100,
			B11000,
			B10000,
		},
		{ // 2
			B10001,
			B10010,
			B11110,
			B00010,
			B00001,
			B00000,
			B00000,
			B00000,
		},
	},
	{ // 4
		{ // 2
			B00000,
			B00000,
			B11111,
			B00000,
			B00000,
			B00000,
			B00000,
			B00000,
		},
		{ // 2
			B10000,
			B01000,
			B01111,
			B01000,
			B10000,
			B00000,
			B00000,
			B00000,
		},
	},
	{ // 4
		{ // 2
			B00000,
			B00000,
			B11100,
			B00110,
			B00011,
			B00001,
			B00000,
			B00000,
		},
		{ // 2
			B00001,
			B00010,
			B11110,
			B00010,
			B00001,
			B00000,
			B00000,
			B00000,
		},
	},
	{ // 4
		{ // 2
			B00000,
			B00000,
			B00000,
			B00000,
			B00000,
			B10000,
			B11110,
			B00001,
		},
		{ // 2
			B10001,
			B01001,
			B01110,
			B01000,
			B10000,
			B00000,
			B00000,
			B00000,
		},
	},
};

void PrintDistance(int distance, int realDistance);
void BuzzerWithDelay(int dly);

void setup()
{
	#ifdef SERIAL_TESTING
	Serial.begin(SERIAL_PORT);
	#endif

	// setup lcd
	{
		// lcd size
		lcd.begin(lcd_XCount, lcd_YCount);

		// car pixels to lcd ram
		byte charIndex = 0;
		for (int i = 0; i < 4; i++)
		{
			for (int ii = 0; ii < 2; ii++)
			{
				lcd.createChar((int)charIndex++, car[i][ii].bytes);
			}
		}
	}

	// setup distance sensor
	{
		pinMode(trigPin, OUTPUT);
		pinMode(echoPin, INPUT);
	}

	// setup buzzer sensor
	{
		pinMode(buzzerPin, OUTPUT);
		// BUZZER_FREQUENCY = random(300, 600);
	}
}

void loop()
{
	int distance = CalculateDistance();

	#ifdef SERIAL_TESTING
	Serial.println(distance);
	#endif

	if (distance == -1) // if distance is too long
	{
		PrintDistance(-1, -1); // area clear
		BuzzerWithDelay(0); // clear buzzer
	}
	else
	{
		int mappedDistance = map(distance, minRange, maxRange, 0, lcd_XCount-4-3); // distance to led range
		PrintDistance(mappedDistance, distance); // draw lcd
		if (mappedDistance == 0) // if its too near
		{
			BuzzerWithDelay(-1); // continuous buzzer
		}
		else
		{
			BuzzerWithDelay(distance);
		}
	}


	delay(LOOP_DELAY);
}

void PrintDistance(int distance, int realDistance)
{
	#ifdef SERIAL_TESTING
	Serial.println(distance);
	#endif

	// clean lcd screen
	lcd.clear();

	if (distance == -1) // if distance as so long then draw safe
	{
		lcd.setCursor(0, 0);
		for (int i = 0; i < (lcd_XCount-strlen(DISTANCE_SECURE))/2; i++)
			lcd.print(" ");
		lcd.print(DISTANCE_SECURE);

		lcd.setCursor(0, 1);
		for (int i = 0; i < (lcd_XCount-strlen(DISTANCE_SECURE2))/2; i++)
			lcd.print(" ");
		lcd.print(DISTANCE_SECURE2);
		return;
	}

	// first line
	{
		lcd.setCursor(0, 0);
		lcd.print(DISTANCE_CHARACTER);
		// Draw distance as space
		for (int i = 0; i < distance; i++)
			lcd.print(" ");
		// Draw car 0, 2, 4, 6
		for (int i = 0; i < 8; i+=2)
			lcd.write(i);

		// Draw lenght
		lcd.setCursor(lcd_XCount-3, 0);
		lcd.print(realDistance, DEC);
	}

	// second line
	{
		lcd.setCursor(0, 1);
		lcd.print(DISTANCE_CHARACTER);
		// Draw distance as space
		for (int i = 0; i < distance; i++)
			lcd.print(" ");
		// Draw car 1, 3, 5, 7
		for (int i = 1; i < 8; i+=2)
			lcd.write(i);

		// Draw cm
		lcd.setCursor(lcd_XCount-3, 1);
		lcd.print(DISTANCE_UNIT_TEXT);
	}
}


/**
 * int CalculateDistance()
 * @return calculate distance and return as cm
 * ! if distnce bigger than maxDistance, return -1
 */
int CalculateDistance()
{
	long duration, distance;

	digitalWrite(trigPin,LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	duration = pulseIn(echoPin, HIGH);
	distance = duration / 58.2;
	// delay(50);

	if (distance > maxRange)
		return -1;

	return distance;
}

int CalculateTemperature()
{
	return 0;
}

/**
 * void BuzzerWithDelay(int dly)
 * @param dly wait after tone
 */
void BuzzerWithDelay(int dly)
{
	// BUZZER_FREQUENCY = map(dly, minRange, maxRange, 300, 500);

	if (dly == -1)
	{
		tone(buzzerPin, BUZZER_FREQUENCY);
	}
	else if (dly == 0)
	{
		noTone(buzzerPin);
	}
	else
	{
		tone(buzzerPin, BUZZER_FREQUENCY);
		delay(dly);
		noTone(buzzerPin);
		delay(dly);
	}

}