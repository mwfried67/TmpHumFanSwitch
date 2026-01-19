//
//    FILE: dht11_test.ino
//  AUTHOR: Rob Tillaart
// PURPOSE: DHT library test sketch for DHT11 && Arduino
//     URL: https://github.com

#include "dht.h"
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// --- Function Prototype Declaration ---
// C++ requires functions to be declared or defined before they are called.
float getBilinearZ(float x, float y); 

// Initialize LCD: Address 0x27, 20 columns, 4 rows
LiquidCrystal_I2C lcd(0x27, 20, 4); 

//dht DHT;

// Two instances for two sensors
dht DHT_OUT;
dht DHT_IN;

#define DHT_OUT_PIN     5  // Outdoor Sensor
#define DHT_IN_PIN      6  // Indoor Sensor (Added)

// 1. Define global breakpoints and table
const int NUM_X = 6;
const int NUM_Y = 6;
/////////////////////////50  59  68  77  86  95
float x_values[NUM_X] = {10, 15, 20, 25, 30, 35};//Temperature
float y_values[NUM_Y] = {60, 70, 80, 85, 90, 95};//Humdity

int z_table[NUM_Y][NUM_X] = {//Fan on or off eventually
//{10, 15, 20, 25, 30, 35}
  {0,  1,  1,  1,  1,  1},//60% RH
  {0,  1,  1,  1,  1,  1},//70% RH
  {0,  1,  1,  1,  1,  1},//80% RH
  {0,  1,  1,  1,  1,  1},//85% RH
  {0,  1,  1,  1,  1,  1},//90% RH
  {0,  0,  0,  0,  0,  0}//95% RH
  };

// Global variables to hold data
float outTemp, outHum;
float inTemp, inHum;
int i = 0;      // Re-added for commented debug loop
int result = 0; // Restored global result variable

// Variables for Fraction and Time tracking
unsigned long totalLoops = 0;
unsigned long loopsOn = 0;
float frac_on = 0.0;
float tot_tm = 0.0; // Total time in hours

void setup()
{
  lcd.init();
  lcd.backlight();

  delay(10000); // Wait for serial monitor to come online.
  Serial.begin(115200);
  Serial.println();
  Serial.println(__FILE__);
  Serial.print("DHT_LIB_VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
  pinMode(13, OUTPUT);
  Serial.println("Dual DHT11 Test");
  Serial.println("In_T,\tIn_H,\tOut_T,\tOut_H,\tFan");
  }


void loop()
{
  //  READ DATA
  // 1. READ OUTDOOR SENSOR (Pin 5)
  int chk1 = DHT_OUT.read11(DHT_OUT_PIN);
  outHum  = DHT_OUT.humidity;
  outTemp = DHT_OUT.temperature;

  delay(1000); // Small delay between sensor reads to prevent timing collisions

  // 2. READ INDOOR SENSOR (Pin 6)
  int chk2 = DHT_IN.read11(DHT_IN_PIN);
  inHum  = DHT_IN.humidity;
  inTemp = DHT_IN.temperature;
// Debug Loop
 // if (i == 3) {
 //   result = 1;
 //   i = 0;
 //   delay(3000);
 // } 
//  else {
 //   result = 0;
//    i = i + 1;
//  }

// Removed 'int' prefix to use global result variable. 
// Added (int) cast because getBilinearZ returns float.
result = (int)getBilinearZ(inTemp, inHum);// Determine Fan State = on

  // Calculate Frac_on and tot_tm (hours)
  totalLoops++;
  if (result == 1) loopsOn++;
  frac_on = 100*(float)loopsOn / (float)totalLoops;
  tot_tm = (float)millis() / 3600000.0; // 3,600,000 milliseconds in 1 hour

  //  Serial display data DHT_OUT 
  Serial.print("DHT_OUT: ,\t");
    switch (chk1)
  {
    case DHTLIB_OK:
    Serial.print("OK,\t");
    break;
    case DHTLIB_ERROR_CHECKSUM:
    Serial.print("Checksum error,\t");
    break;
    case DHTLIB_ERROR_TIMEOUT:
    Serial.print("Time out error,\t");
    break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default:
    Serial.print("Unknown error,\t");
    break;
  }

  Serial.print(outHum, 1);
  Serial.print(",\t");
  Serial.print(outTemp, 1);
  Serial.print(",\t");
  Serial.println(result); // Removed extra ', 1' as result is int

  //  Serial display data DHT_IN 
  Serial.print("DHT_IN: ,\t");
    switch (chk2)
  {
    case DHTLIB_OK:
    Serial.print("OK,\t");
    break;
    case DHTLIB_ERROR_CHECKSUM:
    Serial.print("Checksum error,\t");
    break;
    case DHTLIB_ERROR_TIMEOUT:
    Serial.print("Time out error,\t");
    break;
    case DHTLIB_ERROR_CONNECT:
        Serial.print("Connect error,\t");
        break;
    case DHTLIB_ERROR_ACK_L:
        Serial.print("Ack Low error,\t");
        break;
    case DHTLIB_ERROR_ACK_H:
        Serial.print("Ack High error,\t");
        break;
    default:
    Serial.print("Unknown error,\t");
    break;
  }

  Serial.print(inHum, 1);
  Serial.print(",\t");
  Serial.print(inTemp, 1);
  Serial.print(",\t");
  Serial.println(result);

  digitalWrite(13, result); // Control fan
  
// Drive 20X4 LCD Display******************
  // Clear the display before updating values to prevent "ghost" characters
  lcd.clear(); 
  
  // Line 1: Out Temperature and Humidity
  lcd.setCursor(0, 0); 
  lcd.print("Out: ");
  lcd.print(outTemp, 0);    
  lcd.print(" DegC ");
  lcd.print(outHum, 0);    
  lcd.print("% RH");

  // Line 2: In Temperature and Humidity
  lcd.setCursor(0, 1); 
  lcd.print("In : ");
  lcd.print(inTemp, 0);    
  lcd.print(" DegC ");
  lcd.print(inHum, 0);    
  lcd.print("% RH");


  // Line 3: Fan Status
  lcd.setCursor(0, 2); 
  lcd.print("Fan: ");
  lcd.print(result);

  // Line 4: Stats and Time Running
  lcd.setCursor(0, 3);
  lcd.print("%On:");
  lcd.print(frac_on, 0);
  lcd.print(" TotHr:");
  lcd.print(tot_tm, 0);

   // This delay determines how often the sensor is read and display updated.
  delay(2000); 
}

// Bilinear Interpolation Function defined outside loop()
float getBilinearZ(float x, float y) {
  // ... (Function body remains the same) ...
  x = constrain(x, x_values[0], x_values[NUM_X-1]);
  y = constrain(y, y_values[0], y_values[NUM_Y-1]);

  int i = 0;
  while (i < NUM_X - 2 && x > x_values[i + 1]) i++;
  int j = 0;
  while (j < NUM_Y - 2 && y > y_values[j + 1]) j++;

  float x_frac = (x - x_values[i]) / (x_values[i+1] - x_values[i]);
  float y_frac = (y - y_values[j]) / (y_values[j+1] - y_values[j]);

  int z00 = z_table[j][i];
  int z10 = z_table[j][i+1];
  int z01 = z_table[j+1][i];
  int z11 = z_table[j+1][i+1];

  float z_bottom = z00 + x_frac * (z10 - z00);
  float z_top = z01 + x_frac * (z11 - z01);

  return z_bottom + y_frac * (z_top - z_bottom);
}


//  -- END OF FILE --
