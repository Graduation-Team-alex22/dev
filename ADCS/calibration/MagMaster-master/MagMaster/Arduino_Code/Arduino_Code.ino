#include "Wire.h"
#include <QMC5883LCompass.h>

QMC5883LCompass compass; //Copy the folder "HMC5883L" in the folder "C:\Program Files\Arduino\libraries" and restart the arduino IDE.

float xv, yv, zv;

void setup()
{   
  Serial.begin(9600);
  Wire.begin();  
  compass.init();       
}

void loop()
{
  getHeading();

  Serial.flush(); 
  Serial.print(xv); 
  Serial.print(",");
  Serial.print(yv);
  Serial.print(",");
  Serial.print(zv);
  Serial.println();

  delay(100); 
} 


 
void getHeading()
{ 
  compass.read();
  xv = compass.getX();
  yv = compass.getY();
  zv = compass.getZ();
}
