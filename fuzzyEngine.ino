#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>
Fuzzy* fuzzy = new Fuzzy();

FuzzySet* airtempCold = new FuzzySet(15, 15, 15, 27.5);
FuzzySet* airtempAverage = new FuzzySet(15, 27.5, 27.5, 40);
FuzzySet* airtempHot = new FuzzySet(27.5, 40, 40, 40);

FuzzySet* radtempCold = new FuzzySet(15, 15, 15, 27.5);
FuzzySet* radtempAverage = new FuzzySet(15, 27.5, 27.5, 40);
FuzzySet* radtempHot = new FuzzySet(27.5, 40, 40, 40);

FuzzySet* cold = new FuzzySet(-3 , -3 , -3 , -2);
FuzzySet* cool = new FuzzySet(-3 , -2 , -2 , -1);
FuzzySet* slightlyCool = new FuzzySet(-2 , -1 , -1 , 0);
FuzzySet* neutral = new FuzzySet(-1 , 0 , 0 , 1);
FuzzySet* slightlyWarm = new FuzzySet(0 , 1 , 1 , 2);
FuzzySet* warm = new FuzzySet(1 , 2 , 2 , 3);
FuzzySet* hot = new FuzzySet(2 , 3 , 3 , 3);

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#include <SFE_BMP180.h>

SFE_BMP180 pressure;

#include <Wire.h>

const int ADDR =0x40;
int X0,X1;
double X_out;

#include <ESP8266WiFi.h>
#include <WiFiClient.h>  
#include "ThingSpeak.h"

const char* ssid = "Nasi Lemak Anak Dara";  // ??
const char* password = "Daus_Gabbar94"; // ??

//const char* ssid = "redmi";  // ??
//const char* password = "071293azizi"; // ??
  
WiFiClient client;  

unsigned long myChannelNumber = 208758;  
const char * myWriteAPIKey = "ZU97ZGGT9ICVP4NZ";

void setup()
{
  Serial.begin(9600);
  
  lcd.begin(20, 4);
  lcd.clear();

  pressure.begin();

  Wire.begin();                                     
  Wire.beginTransmission(ADDR);
  Wire.endTransmission();

  WiFi.begin(ssid, password); 
   
  while (WiFi.status() != WL_CONNECTED)  
  {  
   delay(500);    
  }     
  ThingSpeak.begin(client);

  //fuzzy input
  FuzzyInput* airTemperature = new FuzzyInput(1);
  airTemperature->addFuzzySet(airtempCold);
  airTemperature->addFuzzySet(airtempAverage);
  airTemperature->addFuzzySet(airtempHot);
  fuzzy->addFuzzyInput(airTemperature);

  FuzzyInput* radTemperature = new FuzzyInput(2);
  radTemperature->addFuzzySet(radtempCold);
  radTemperature->addFuzzySet(radtempAverage);
  radTemperature->addFuzzySet(radtempHot);
  fuzzy->addFuzzyInput(radTemperature);

  // fuzzy output
  FuzzyOutput* pmvValue = new FuzzyOutput(1);
  FuzzySet* pmvCold = new FuzzySet(-3, -3, -3, -2);
  pmvValue->addFuzzySet(cold);
  FuzzySet* pmvCool = new FuzzySet(-3, -2, -2, -1);
  pmvValue->addFuzzySet(cool);
  FuzzySet* pmvSlightlyCool = new FuzzySet(-2, -1, -1, 0);
  pmvValue->addFuzzySet(slightlyCool);
  FuzzySet* pmvNeutral = new FuzzySet(-1, 0, 0, 1);
  pmvValue->addFuzzySet(neutral);
  FuzzySet* pmvSlightlyWarm = new FuzzySet(0, 1, 1, 2);
  pmvValue->addFuzzySet(slightlyWarm);
  FuzzySet* pmvWarm = new FuzzySet(1, 2, 2, 3);
  pmvValue->addFuzzySet(warm);
  FuzzySet* pmvHot = new FuzzySet(2, 3, 3, 3);
  pmvValue->addFuzzySet(hot);
  fuzzy->addFuzzyOutput(pmvValue);

  //fuzzy rules(1)
  FuzzyRuleAntecedent* AtCold1AndRtCold1 = new FuzzyRuleAntecedent();
  AtCold1AndRtCold1->joinWithAND(airtempCold, radtempCold);
  FuzzyRuleConsequent* pmvCool1 = new FuzzyRuleConsequent();
  pmvCool1->addOutput(cool);

  FuzzyRule* rule1 = new FuzzyRule(1, AtCold1AndRtCold1, pmvCool1);
  fuzzy->addFuzzyRule(rule1);

  //fuzzy rules(2)
  FuzzyRuleAntecedent* AtCold2AndRtAverage1 = new FuzzyRuleAntecedent();
  AtCold2AndRtAverage1->joinWithAND(airtempCold, radtempAverage);
  FuzzyRuleConsequent* pmvSlightlyCool1 = new FuzzyRuleConsequent();
  pmvSlightlyCool1->addOutput(slightlyCool);

  FuzzyRule* rule2 = new FuzzyRule(2, AtCold2AndRtAverage1, pmvSlightlyCool1);
  fuzzy->addFuzzyRule(rule2);

  //fuzzy rules(3)
  FuzzyRuleAntecedent* AtCold3AndRtHot1 = new FuzzyRuleAntecedent();
  AtCold3AndRtHot1->joinWithAND(airtempCold, radtempHot);
  FuzzyRuleConsequent* pmvNeutral1 = new FuzzyRuleConsequent();
  pmvNeutral1->addOutput(neutral);

  FuzzyRule* rule3 = new FuzzyRule(3, AtCold3AndRtHot1, pmvNeutral1);
  fuzzy->addFuzzyRule(rule3);

  //fuzzy rules(4)
  FuzzyRuleAntecedent* AtAverage1AndRtCold2 = new FuzzyRuleAntecedent();
  AtAverage1AndRtCold2->joinWithAND(airtempAverage, radtempCold);
  FuzzyRuleConsequent* pmvNeutral2 = new FuzzyRuleConsequent();
  pmvNeutral2->addOutput(neutral);

  FuzzyRule* rule4 = new FuzzyRule(4, AtAverage1AndRtCold2, pmvNeutral2);
  fuzzy->addFuzzyRule(rule4);

  //fuzzy rules(5)
  FuzzyRuleAntecedent* AtAverage2AndRtAverage2 = new FuzzyRuleAntecedent();
  AtAverage2AndRtAverage2->joinWithAND(airtempAverage, radtempAverage);
  FuzzyRuleConsequent* pmvSlightlyWarm1 = new FuzzyRuleConsequent();
  pmvSlightlyWarm1->addOutput(slightlyWarm);

  FuzzyRule* rule5 = new FuzzyRule(5, AtAverage2AndRtAverage2, pmvSlightlyWarm1);
  fuzzy->addFuzzyRule(rule5);

  //fuzzy rules(6)
  FuzzyRuleAntecedent* AtAverage3AndRtHot2 = new FuzzyRuleAntecedent();
  AtAverage3AndRtHot2->joinWithAND(airtempAverage, radtempHot);
  FuzzyRuleConsequent* pmvWarm1 = new FuzzyRuleConsequent();
  pmvWarm1->addOutput(warm);

  FuzzyRule* rule6 = new FuzzyRule(6, AtAverage3AndRtHot2, pmvWarm1);
  fuzzy->addFuzzyRule(rule6);

  //fuzzy rules(7)
  FuzzyRuleAntecedent* AtHot1AndRtCold3 = new FuzzyRuleAntecedent();
  AtHot1AndRtCold3->joinWithAND(airtempHot, radtempCold);
  FuzzyRuleConsequent* pmvSlightlyWarm2 = new FuzzyRuleConsequent();
  pmvSlightlyWarm2->addOutput(slightlyWarm);

  FuzzyRule* rule7 = new FuzzyRule(7, AtHot1AndRtCold3, pmvSlightlyWarm2);
  fuzzy->addFuzzyRule(rule7);

  //fuzzy rules(8)
  FuzzyRuleAntecedent* AtHot2AndRtAverage3 = new FuzzyRuleAntecedent();
  AtHot2AndRtAverage3->joinWithAND(airtempHot, radtempAverage);
  FuzzyRuleConsequent* pmvWarm2 = new FuzzyRuleConsequent();
  pmvWarm2->addOutput(warm);

  FuzzyRule* rule8 = new FuzzyRule(8, AtHot2AndRtAverage3, pmvWarm2);
  fuzzy->addFuzzyRule(rule8);

  //fuzzy rules(9)
  FuzzyRuleAntecedent* AtHot3AndRtHot3 = new FuzzyRuleAntecedent();
  AtHot3AndRtHot3->joinWithAND(airtempHot, radtempHot);
  FuzzyRuleConsequent* pmvHot1 = new FuzzyRuleConsequent();
  pmvHot1->addOutput(hot);

  FuzzyRule* rule9 = new FuzzyRule(9, AtHot3AndRtHot3, pmvHot1);
  fuzzy->addFuzzyRule(rule9);
}

void loop()
{
  char status;
  double T,radTemp;

  Wire.beginTransmission(ADDR);
  
  // Send temperature measurement command
  // SI7021
  // SCL D1 ,  SDA D2
  Wire.write(0xE3);
  Wire.endTransmission();
  
  delay(500);
  
  // Request 2 bytes of data
  Wire.requestFrom(ADDR,2);

  if(Wire.available() <= 2)
  {
   X0 = Wire.read();
   X1 = Wire.read();
   X0 = X0<<8;
   X_out = X0+X1;
  }
 
  // Convert the data
  
  float celsTemp = ((175.72 * X_out) / 65536.0) - 46.85;

  status = pressure.startTemperature();
  if (status != 0)
  {
    // SDA D2
    // SCl D1

    // green wire 3.3V
    // blue wire GND
    // silver wire D2
    // purple wire D1
    
    // Wait for the measurement to complete:

    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Use '&T' to provide the address of T to the function.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      radTemp = T;
     // Start a pressure measurement:
     // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
     // If request is successful, the number of ms to wait is returned.
     // If request is unsuccessful, 0 is returned.
    }
  }
  
  float TA = celsTemp;
  float RT = radTemp;
  float RH = 60;
  float AV = 0.1;
  float MR = 1;
  float CI = 1;

  fuzzy->setInput(1, TA);
  fuzzy->setInput(2, RT);

  Serial.println("      ");
  Serial.print("Air Temperature: ");
  Serial.println(TA);
  Serial.print("Radiant Temperature: ");
  Serial.println(RT);
  Serial.print("Relative Humidity: ");
  Serial.println(RH);
  Serial.print("Air Velocity: ");
  Serial.println(AV);
  Serial.print("Metabolic Rate: ");
  Serial.println(MR);
  Serial.print("Clothing Insulation: ");
  Serial.println(CI);
  
  fuzzy->fuzzify();

  lcd.clear(); 
  lcd.setCursor(0,0); 
  lcd.print("TA :");
  lcd.print(TA);
  
  lcd.setCursor(0,1); 
  lcd.print("RT :");
  lcd.print(RT);

  float PMV = fuzzy->defuzzify(1);
  
  Serial.print("Pmv Value: ");
  Serial.println(PMV);

  ThingSpeak.setField(1,TA);
  ThingSpeak.setField(2,RT);
  ThingSpeak.setField(3,PMV);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);   
  
  lcd.setCursor(0,2); 
  lcd.print("PMV :");
  lcd.print(PMV);

  if (PMV <= -2.5) {
    Serial.println("Pmv Condition : COLD");
    Serial.println("TA : +3");
    Serial.println("RT : Open");
    lcd.setCursor(0,3); 
    lcd.print("STAT: COLD");
    lcd.setCursor(10,0); 
    lcd.print("TA: +3");
    lcd.setCursor(10,1); 
    lcd.print("RT: Open");
  }
  else if (PMV <= -1.5) {
    Serial.println("Pmv Condition: COOL");
    Serial.println("TA : +2");
    Serial.println("RT : Open");
    lcd.setCursor(0,3); 
    lcd.print("STAT: COOL");
    lcd.setCursor(10,0); 
    lcd.print("TA: +2");
    lcd.setCursor(10,1); 
    lcd.print("RT: Open");
  }
  else if (PMV <= -0.5) {
    Serial.println("Pmv Condition: SLIGHTLY COOL");
    Serial.println("TA : +1");
    Serial.println("RT : Open");
    lcd.setCursor(0,3); 
    lcd.print("STAT: SLIGHTLY COOL");
    lcd.setCursor(10,0); 
    lcd.print("TA: +1");
    lcd.setCursor(10,1); 
    lcd.print("RT: Open");
  }
  else if (PMV > -0.5 && PMV < 0.5) {
    Serial.println("Pmv Condition: NEUTRAL");
    Serial.println("TA : 0");
    Serial.println("RT : Maintain");
    lcd.setCursor(0,3); 
    lcd.print("STATUS :NEUTRAL");
    lcd.setCursor(10,0); 
    lcd.print("TA : 0");
    lcd.setCursor(10,1); 
    lcd.print("RT : NC");
  }
  else if (PMV >= 0.5 && PMV < 1.5) {
    Serial.println("Pmv Condition: SLIGHTLY WARM");
    Serial.println("TA : -1");
    Serial.println("RT : Close");
    lcd.setCursor(0,3); 
    lcd.print("STAT: SLIGHTLY WARM");
    lcd.setCursor(10,0); 
    lcd.print("TA: -1");
    lcd.setCursor(10,1); 
    lcd.print("RT: Close");
  }
  else if (PMV >= 1.5 && PMV < 2.5) {
    Serial.println("Pmv Condition: WARM");
    Serial.println("TA : -2");
    Serial.println("RT : Close");
    lcd.setCursor(0,3); 
    lcd.print("STAT: WARM");
    lcd.setCursor(10,0); 
    lcd.print("TA: -2");
    lcd.setCursor(10,1); 
    lcd.print("RT: Close");
  }
  else if (PMV >= 2.5) {
    Serial.println("Pmv Condition: HOT");
    Serial.println("TA : -3");
    Serial.println("RT : Close");
    lcd.setCursor(0,3); 
    lcd.print("STAT: HOT");
    lcd.setCursor(10,0); 
    lcd.print("TA: -3");
    lcd.setCursor(10,1); 
    lcd.print("RT: Close");
  }
  Serial.println(" ");
  delay(60000);
}
