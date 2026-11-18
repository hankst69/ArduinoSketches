// requires sMQTTBroker library (Simple MQTT Broker)
#include <WiFi.h>
#include <sMQTTBroker.h>
#include <sMQTTEvent.h>
#include <ESP32Servo.h>

const char* ssid = "ESP32";
const char* password = "12345678";

unsigned long resetTimer = 0;
bool resetArmed = false;

#define MQTT_PORT 1883

// ------------------
// Servo Pins WROOM32
// ------------------

#define PIN_X      25
#define PIN_Y      26
#define PIN_ROT    27


#define PIN_TILT_LF  32
#define PIN_TILT_LH  14

#define PIN_TILT_RF  33
#define PIN_TILT_RH  13


// ------------------
// Taster Pins
// ------------------

#define BTN_X      18
#define BTN_Y      19
#define BTN_ROT    21
#define BTN_TILT   22

// ------------------
// Grenzwerte
// ------------------

#define X_MIN      60
#define X_MAX      120

#define Y_MIN      60
#define Y_MAX      120

#define ROT_MIN    60
#define ROT_MAX    120

#define TILT_HOME  90
#define TILT_MAX   150

// ------------------
// Tilt Kalibrierung
// ------------------

#define TILT_LF_OFFSET   -5
#define TILT_LH_OFFSET   0

#define TILT_RF_OFFSET   4
#define TILT_RH_OFFSET   6




Servo servoX;
Servo servoY;
Servo servoRot;

Servo servoTiltLF;
Servo servoTiltLH;

Servo servoTiltRF;
Servo servoTiltRH;


// Startpositionen

int posX = 90;
int posY = 90;
int posRot = 90;
int posTilt = 90;

// Tasterstatus

bool lastBtnX = HIGH;
bool lastBtnY = HIGH;
bool lastBtnRot = HIGH;
bool lastBtnTilt = HIGH;

// Funktionsprototyp

void moveServoSlow(Servo &servo, int startPos, int endPos);


// --------------------------------------------------
// MQTT Broker
// --------------------------------------------------

class MyMQTTBroker : public sMQTTBroker
{
public:

  bool onEvent(sMQTTEvent *event)
  {
    if(event->Type() == Public_sMQTTEventType)
    {
      sMQTTPublicClientEvent *evt =
        (sMQTTPublicClientEvent*)event;

      String topic =
        String(evt->Topic().c_str());

      String payload =
        String(evt->Payload().c_str());

      String numberText = "";

      for(int i = payload.length() - 1; i >= 0; i--)
      {
        char c = payload[i];

        if(isDigit(c))
        {
          numberText = c + numberText;
        }
        else if(numberText.length() > 0)
        {
          break;
        }
      }

      int angle = numberText.toInt();

      angle = constrain(angle, 0, 180);

      Serial.println();
      Serial.println("===== MQTT =====");

      Serial.print("Topic: ");
      Serial.println(topic);

      Serial.print("Winkel: ");
      Serial.println(angle);

      // ---------------- X ----------------

      if(topic.indexOf("wfauto/Ser1") >= 0)
    {
      int oldPos = posX;

      posX = constrain(angle, X_MIN, X_MAX);

      moveServoSlow(servoX, oldPos, posX);

    Serial.println("X bewegt");
    }

      // ---------------- Y ----------------

      else if(topic.indexOf("wfauto/Ser2") >= 0)
    {
      int oldPos = posY;

      posY = constrain(angle, Y_MIN, Y_MAX);

      moveServoSlow(servoY, oldPos, posY);

      Serial.println("Y bewegt");
    }

      // ------------- Rotation -----------

      else if(topic.indexOf("wfauto/Ser3") >= 0)
    {
      int oldPos = posRot;

      posRot = constrain(angle, ROT_MIN, ROT_MAX);

      moveServoSlow(servoRot, oldPos, posRot);

      Serial.println("Rotation bewegt");
    }

      // ---------------- Tilt ------------

      else if(topic.indexOf("wfauto/Ser4") >= 0)
      {
      
        posTilt = constrain(angle, TILT_HOME, TILT_MAX);


        servoTiltLF.write(posTilt + TILT_LF_OFFSET);
        servoTiltLH.write(posTilt + TILT_LH_OFFSET);

        servoTiltRF.write((180 - posTilt) + TILT_RF_OFFSET);
        servoTiltRH.write((180 - posTilt) + TILT_RH_OFFSET);

        Serial.println("Tilt bewegt");
      }

      Serial.println("================");
    }

    return true;
  }
};

MyMQTTBroker broker;

// --------------------------------------------------
// X
// --------------------------------------------------


void moveServoSlow(Servo &servo, int startPos, int endPos)
{
  if(startPos < endPos)
  {
    for(int p = startPos; p <= endPos; p++)
    {
      servo.write(p);
      delay(50);
    }
  }
  else
  {
    for(int p = startPos; p >= endPos; p--)
    {
      servo.write(p);
      delay(50);
    }
  }
}




void stepServoX()
{
  int oldPos = posX;

  if(posX < X_MAX)
  {
    posX += 5;

    if(posX > X_MAX)
      posX = X_MAX;
  }
  else
  {
    posX = X_MIN;
  }

  moveServoSlow(servoX, oldPos, posX);

  Serial.print("X Position: ");
  Serial.println(posX);
}
// --------------------------------------------------
// Y
// --------------------------------------------------

void stepServoY()
{
  int oldPos = posY;

  if(posY < Y_MAX)
  {
    posY += 5;

    if(posY > Y_MAX)
      posY = Y_MAX;
  }
  else
  {
    posY = Y_MIN;
  }

  moveServoSlow(servoY, oldPos, posY);

  Serial.print("Y Position: ");
  Serial.println(posY);
}

// --------------------------------------------------
// Rotation
// --------------------------------------------------

void stepServoRot()
{
  int oldPos = posRot;

  if(posRot < ROT_MAX)
  {
    posRot += 5;

    if(posRot > ROT_MAX)
      posRot = ROT_MAX;
  }
  else
  {
    posRot = ROT_MIN;
  }

  moveServoSlow(servoRot, oldPos, posRot);

  Serial.print("Rotation Position: ");
  Serial.println(posRot);
}

//--------------------------------
//Tilt
//----------------------------------

void stepServoTilt()
{
  if(posTilt < TILT_MAX)
  {
    posTilt += 5;

    if(posTilt > TILT_MAX)
      posTilt = TILT_MAX;
  }
  else
  {
    posTilt = TILT_HOME;
  }

  servoTiltLF.write(posTilt + TILT_LF_OFFSET);
  servoTiltLH.write(posTilt + TILT_LH_OFFSET);

  servoTiltRF.write((180 - posTilt) + TILT_RF_OFFSET);
  servoTiltRH.write((180 - posTilt) + TILT_RH_OFFSET);

  Serial.print("Tilt Position: ");
  Serial.println(posTilt);
}


// --------------------------------------------------
// Setup
// --------------------------------------------------

void setup()
{
  Serial.begin(115200);

  delay(2000);

  Serial.println("Starte WLAN Hotspot");

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servoX.setPeriodHertz(50);
  servoY.setPeriodHertz(50);
  servoRot.setPeriodHertz(50);
  
  servoTiltLF.setPeriodHertz(50);
  servoTiltLH.setPeriodHertz(50);

  servoTiltRF.setPeriodHertz(50);
  servoTiltRH.setPeriodHertz(50);

  servoX.attach(PIN_X, 500, 2500);
  servoY.attach(PIN_Y, 500, 2500);

  servoY.write(60);
  delay(1000);

  servoY.write(120);
  delay(1000);

  servoY.write(90);
  delay(1000);



  servoRot.attach(PIN_ROT, 500, 2500);
  
  servoTiltLF.attach(PIN_TILT_LF, 500, 2500);
  servoTiltLH.attach(PIN_TILT_LH, 500, 2500);

  servoTiltRF.attach(PIN_TILT_RF, 500, 2500);
  servoTiltRH.attach(PIN_TILT_RH, 500, 2500);

  servoX.write(posX);
  servoY.write(posY);
  servoRot.write(posRot);

  servoTiltLF.write(posTilt + TILT_LF_OFFSET);
  servoTiltLH.write(posTilt + TILT_LH_OFFSET);

  servoTiltRF.write((180 - posTilt) + TILT_RF_OFFSET);
  servoTiltRH.write((180 - posTilt) + TILT_RH_OFFSET);

  pinMode(BTN_X, INPUT_PULLUP);
  pinMode(BTN_Y, INPUT_PULLUP);
  pinMode(BTN_ROT, INPUT_PULLUP);
  pinMode(BTN_TILT, INPUT_PULLUP);

  broker.init(MQTT_PORT);

  Serial.println("MQTT Broker gestartet");
}

// --------------------------------------------------
// Loop
// --------------------------------------------------

void loop()
{
  broker.update();

  bool stateX = digitalRead(BTN_X);
  bool stateY = digitalRead(BTN_Y);
  bool stateRot = digitalRead(BTN_ROT);
  bool stateTilt = digitalRead(BTN_TILT);

    // -----------------------------------
  // Software Reset über X + Y
  // -----------------------------------

  if(stateX == LOW && stateY == LOW)
  {
    if(!resetArmed)
    {
      resetArmed = true;
      resetTimer = millis();

      Serial.println("Reset vorbereitet...");
    }

    if(millis() - resetTimer > 3000)
    {
      Serial.println("ESP Neustart");

      delay(200);

      ESP.restart();
    }
  }
  else
  {
    resetArmed = false;
  }



  // -------- X --------

  if(lastBtnX == HIGH && stateX == LOW)
  {
    Serial.println("Taster X");
    stepServoX();
    delay(150);
  }

  // -------- Y --------

  if(lastBtnY == HIGH && stateY == LOW)
  {
    Serial.println("Taster Y");
    stepServoY();
    delay(150);
  }

  // ----- Rotation ----

  if(lastBtnRot == HIGH && stateRot == LOW)
  {
    Serial.println("Taster Rotation");
    stepServoRot();
    delay(150);
  }

  // ------- Tilt ------

  if(lastBtnTilt == HIGH && stateTilt == LOW)
  {
    Serial.println("Taster Tilt");

  
    posTilt += 5;

    if(posTilt > TILT_MAX)
    {
    posTilt = TILT_HOME;
    }


    servoTiltLF.write(posTilt + TILT_LF_OFFSET);
    servoTiltLH.write(posTilt + TILT_LH_OFFSET);

    servoTiltRF.write((180 - posTilt) + TILT_RF_OFFSET);
    servoTiltRH.write((180 - posTilt) + TILT_RH_OFFSET);

    Serial.print("Neue Tilt Position: ");
    Serial.println(posTilt);

    delay(150);
  }

  lastBtnX = stateX;
  lastBtnY = stateY;
  lastBtnRot = stateRot;
  lastBtnTilt = stateTilt;

  delay(5);
}