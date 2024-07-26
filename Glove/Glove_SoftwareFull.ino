#include <math.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <MPU6050_tockn.h>

#define BLINKER_BLE
#include <Blinker.h>

BlinkerButton Button_Bending("Bending");
BlinkerSlider Slider_Bending("BendingAngle");
BlinkerSlider Slider_Bending_Times("BendingTimes");
BlinkerNumber Number_Bending("BendingAngleValue");

BlinkerButton Button_TremorRating("TremorRating");
BlinkerNumber Number_TremorTotalCount("TremorTotalCount");
BlinkerNumber Number_TremorOverCount("TremorOverCount");
BlinkerNumber Number_TremorRate("TremorPercentage");

BlinkerButton Button_LifeAssistance("TrackingMovement");
//BlinkerNumber Number_isMoving("MovingStatus");

BlinkerSlider Slider_userIDInput("UserIDInput");

#define magnValve0 30  //magnetic valve
#define magnValve1 31
#define magnValve2 28
#define magnValve3 29
#define magnValve4 26
#define magnValve5 27
#define magnValve6 24
#define magnValve7 25
#define magnValve8 23
#define magnValve9 22

#define adExtendA0 38  //ad extender
#define adExtendA1 39
#define adExtendA2 40
#define adExtendZ1 A0  // pwm
// #define en1 //connecting gnd - always working
#define adExtendB0 41
#define adExtendB1 42
#define adExtendB2 43
#define adExtendZ2 A1  // pwm
// #define en2

#define sAirPump1 48  //small air pump
#define sAirPump2 49
#define sAirPumpEna 8
#define bAirPump1 36  //big air pump
#define bAirPump2 37
#define bAirPumpEna 9


/* Function definitions */
// void pumpInterrupt_Safety();

void airPumpControl(String airPump, boolean state, int speed);  //airPump

void magnValveControl(int state);  //magnValve
void openValve(int valveBit);
void closeValve(int valveBit);

void command(String input);  // command

int readSensor(char board, int channel);  //read sensor data
void readFingerSensor();                  // sensor test

void mpu6050PeriodReading(int seconds, float averages[3], float parameters[3]);  //mpu6050 read in a given period of time and return avg
void mpu6050Calibration_10Sec(float avgs[], float& Stddev_Z);
void mpu6050ZAverageValue(float& avg);
void mpu6050PrintAxisValues();

void bendToAngle(int valve, int times, int angle);
void bendToAngle_Blinker(int valve, int times, int angle);

void mpuLifeAssistance(int seconds);

//Blinker
void Button_Bending_Callback(const String& state);
void Slider_Bending_Callback(int32_t value);
void Slider_Bending_Times_Callback(int32_t value);
void Number_Bending_ReadData(const String& data);
void Button_TremorRating_Callback(const String& state);
void Number_TremorTotalCount_ReadData(const String& data);
void Number_TremorOverCount_ReadData(const String& data);
void Number_TremorRate_ReadData(const String& data);
void Button_LifeAssistance_Callback(const String& data);
void Slider_userIDInput_Callback(int id);

// Blinker
int bendingAngle_Blinker = 0;
int bendingTimes = 0;
int tremorRatingTotalCount_Blinker = 0;
int tremorRatingOverCount_Blinker = 0;
int tremorRatingRate_Blinker = 0;
//user ID
int userID = 0;
//air speed
int airspeed = 30;

// for minimizing arrayList
struct MyData {
  float value;
};
void addItem(MyData*& myList, int& currentSize, int& itemCount, MyData item);
MyData getItem(MyData* myList, int itemCount, int index);

/* sensor values */
int thumb[2];
int finger1[4];
int finger2[4];
int finger3[4];
int finger4[2];

/* public variables*/
MPU6050 mpu6050(Wire);
double mpu6050AccZ_Silent = 0.88;                       // axis Z average
double mpu6050AccZ_Stddev = mpu6050AccZ_Silent * 0.03;  //TODO: change - 0.05

int valveState = 0x00;  // valve state

/* action booleans */
boolean mpu6050ZAvg = false;
boolean mpu6050TremorRate = false;
boolean mpu6050AxisValues = false;
boolean flexionExtensionCycle = false;
boolean readAllSensor = false;
boolean mpu6050LifeAssitance = false;
boolean test = false;
boolean flexionExtensionCycle_Blinker = false;
boolean mpu6050TremorRate_Blinker = false;
boolean espSend = false;

void setup() {
  // put your setup code here, to run once:
  //airPump
  pinMode(sAirPump1, OUTPUT);
  pinMode(sAirPump2, OUTPUT);
  pinMode(sAirPumpEna, OUTPUT);
  pinMode(bAirPump1, OUTPUT);
  pinMode(bAirPump2, OUTPUT);
  pinMode(bAirPumpEna, OUTPUT);

  //magnValve
  pinMode(magnValve0, OUTPUT);
  pinMode(magnValve1, OUTPUT);
  pinMode(magnValve2, OUTPUT);
  pinMode(magnValve3, OUTPUT);
  pinMode(magnValve4, OUTPUT);
  pinMode(magnValve5, OUTPUT);
  pinMode(magnValve6, OUTPUT);
  pinMode(magnValve7, OUTPUT);
  pinMode(magnValve8, OUTPUT);
  pinMode(magnValve9, OUTPUT);

  //ad extendor
  pinMode(adExtendA0, OUTPUT);
  pinMode(adExtendA1, OUTPUT);
  pinMode(adExtendA2, OUTPUT);
  pinMode(adExtendB0, OUTPUT);
  pinMode(adExtendB1, OUTPUT);
  pinMode(adExtendB2, OUTPUT);

  Serial.begin(115200);
  Serial2.begin(115200);

  mpu6050.begin();
  Wire.begin();
  mpu6050.calcGyroOffsets(true);

  //DEBUG
  // pinMode(LED_BUILTIN, OUTPUT);
  // digitalWrite(LED_BUILTIN, HIGH);

  //BLINKER_DEBUG.stream(Serial);
  Blinker.begin(51, 50);  //TXD:51

  Button_Bending.attach(Button_Bending_Callback);
  Slider_Bending.attach(Slider_Bending_Callback);
  Slider_Bending_Times.attach(Slider_Bending_Times_Callback);
  Blinker.attachData(Number_Bending_ReadData);

  Button_TremorRating.attach(Button_TremorRating_Callback);
  Blinker.attachData(Number_TremorTotalCount_ReadData);
  Blinker.attachData(Number_TremorOverCount_ReadData);
  Blinker.attachData(Number_TremorRate_ReadData);

  Button_LifeAssistance.attach(Button_LifeAssistance_Callback);
  // Blinker.attachData();

  Slider_userIDInput.attach(Slider_userIDInput_Callback);
}

void loop() {
  Blinker.run();

  if (Serial.available()) {  //command
    String input = Serial.readStringUntil('\n');
    Serial.print(input);
    command(input);
  }

  /* mpu6050 all axis values reading & silent test & tremor rating*/
  if (mpu6050ZAvg) {
    float zAvg;

    mpu6050ZAverageValue(zAvg);
    Serial.print("silent avg: ");
    Serial.println(zAvg);

    mpu6050AccZ_Silent = zAvg;
    Serial.println(mpu6050AccZ_Silent);

    command("stopmpu");
  }

  if (mpu6050TremorRate) {
    float avgs[3];
    float para[3];

    mpu6050PeriodReading(20, avgs, para);

    Serial.print("Total readings: ");
    Serial.println(para[0]);

    Serial.print("Z_overValues: ");
    Serial.println(para[1]);

    Serial.print("Tremor Rate: ");
    Serial.println(para[2]);

    command("stopmpu");
  }

  if (mpu6050TremorRate_Blinker) {  //&& tremorRatingButtonPressed_Blinker == 1
    float avgs[3];
    float para[3];

    mpu6050PeriodReading(20, avgs, para);

    Serial.print("Total readings: ");
    Serial.println(para[0]);
    tremorRatingTotalCount_Blinker = para[0];
    Serial.print("Z_overValues: ");
    Serial.println(para[1]);
    tremorRatingOverCount_Blinker = para[1];
    Serial.print("Tremor Rate: ");
    Serial.println(para[2]);
    tremorRatingRate_Blinker = para[2] * 100;

    Serial.print("total count-");
    Serial.println(tremorRatingTotalCount_Blinker);
    Number_TremorTotalCount.print(tremorRatingTotalCount_Blinker);
    Serial.print("over count-");
    Serial.println(tremorRatingOverCount_Blinker);
    Number_TremorOverCount.print(tremorRatingOverCount_Blinker);
    Serial.print("rate-");
    Serial.println(tremorRatingRate_Blinker);
    Number_TremorRate.print(tremorRatingRate_Blinker);

    String espLog2 = "2-";
    espLog2.concat(userID);
    espLog2.concat("-");
    espLog2.concat(tremorRatingRate_Blinker);
    espLog2.concat("-");
    Serial2.println(espLog2);

    tremorRatingTotalCount_Blinker = 0;
    tremorRatingOverCount_Blinker = 0;
    tremorRatingRate_Blinker = 0;

    mpu6050TremorRate_Blinker = false;
  }

  if (mpu6050AxisValues) {
    mpu6050PrintAxisValues();
  }

  if (flexionExtensionCycle) {
    int cycle = 0;
    while (cycle == 0) {
      if (Serial.available()) {
        cycle = Serial.parseInt();
        if (cycle > 0) {
          Serial.print("Cycle: ");
          Serial.println(cycle);

          int valve = 0x02;
          int angle = 20;
          bendToAngle(valve, cycle, angle);
        } else {
          Serial.println("Invalid input:");
        }
      }
      delay(100);
    }
    command("stopCycle");
  }

  if (flexionExtensionCycle_Blinker) {  //&& bendButtonPressed_Blinker == 1
    Serial.println("Blinker bending initialized");

    //bendToAngle(0x02, 3, bendingAngle_Blinker);
    bendToAngle_Blinker(0x02, bendingTimes, bendingAngle_Blinker);

    String espLog = "1-";
    espLog.concat(userID);
    espLog.concat("-");
    espLog.concat(bendingAngle_Blinker);
    espLog.concat("-");
    espLog.concat(airspeed);
    espLog.concat("-");
    Serial2.println(espLog);

    Number_Bending.print(bendingAngle_Blinker * 1.05);
    flexionExtensionCycle_Blinker = false;
    Serial.println("Bending Stopped");
  }

  if (test) {
    airPumpControl("big", true, 100);
    magnValveControl(0x02);             //valve has to be in form: 0xXX
    while (readSensor('B', 5) < 120) {  //hardcoded
      Serial.print(readSensor('B', 5));
      Serial.println(" - inflating");
    }
    magnValveControl(0x00);  //valve has to be in form: 0xXX
    airPumpControl("big", false, 100);
    Serial.println("Angle reached, extension starts");
    delay(1300);
    openValve(0x02);
    openValve(0x200);
    delay(2500);
    magnValveControl(0x00);  //valve has to be in form: 0xXX
    Serial.println("Extension completed, ready for flexion");
    //magnValveControl(0x200);

    /* bending sequence test*/
    /*
    openValve(0x02);
    delay(1000);
    closeValve(0x02);
    delay(1000);
    magnValveControl(0x202);
    delay(1000);
    magnValveControl(0x00);
    */

    command("stopTest");
  }

  if (mpu6050LifeAssitance) {
    mpuLifeAssistance(60);
    mpu6050LifeAssitance = false;
  }

  if (readAllSensor) {
    //readFingerSensor();
    //Serial.println(readSensor('B', 5)); //thumb flex sensor
    Serial.println(readSensor('B', 4));  //thumb presure
  }

  if (espSend) {
    String ESP = "2-123-78-";  //data type - userID - bending Angle - bending speed
    Serial.println(ESP);
    Serial2.println(ESP);
    espSend = false;
  }
}

void Slider_userIDInput_Callback(int32_t id) {
  Serial.print("ID received: ");
  userID = id;
  Serial.println(userID);
}

void Slider_Bending_Callback(int32_t value) {
  // Serial.print("Bending?-");
  // Serial.print(flexionExtensionCycle_Blinker);
  Serial.print("Angle received: ");
  bendingAngle_Blinker = value;
  Serial.println(bendingAngle_Blinker);
}

void Slider_Bending_Times_Callback(int32_t value) {
  Serial.print("Bending times: ");
  bendingTimes = value;
  Serial.println(bendingTimes);
}

void Button_Bending_Callback(const String& state) {
  //BLINKER_LOG("get button state: ", state);
  Serial.print("Bending initialized - ");
  Serial.print("target angle: ");
  Serial.println(bendingAngle_Blinker);
  if (state == BLINKER_CMD_BUTTON_PRESSED) {
    flexionExtensionCycle_Blinker = true;
  } else if (state == BLINKER_CMD_BUTTON_RELEASED) {
    flexionExtensionCycle_Blinker = false;
  }
  //DEBUG
  //digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  //bendButtonPressed_Blinker ++;
}

void Number_Bending_ReadData(const String& data) {
  BLINKER_LOG("Blinker readString: ", data);
  Number_Bending.unit("Degree");
  Number_Bending.print(readSensor('B', 5));
}

void Button_TremorRating_Callback(const String& state) {
  Serial.println("tremorTracking initialized");
  mpu6050TremorRate_Blinker = true;
  //tremorRatingButtonPressed_Blinker ++;
}

void Number_TremorTotalCount_ReadData(const String& data) {
  Number_TremorTotalCount.print(tremorRatingTotalCount_Blinker);
  Serial.println("totalCount printed ");
}

void Number_TremorOverCount_ReadData(const String& data) {
  Number_TremorOverCount.print(tremorRatingOverCount_Blinker);
  Serial.println("overCount printed ");
}

void Number_TremorRate_ReadData(const String& data) {
  Number_TremorRate.print(tremorRatingRate_Blinker);
  Serial.println("rate printed ");
}

void Button_LifeAssistance_Callback(const String& data) {
  Serial.println("LifeAssistance initialized");
  mpu6050LifeAssitance = true;
}

void command(String input) {
  if (input.equals("start")) {
    airPumpControl("big", true, 50);  //手套从110开始启动
    magnValveControl(0x02);
  }
  if (input.equals("stop")) {
    airPumpControl("big", false, 110);
    magnValveControl(0x0);
    //delay(500);
  }
  if (input.equals("release")) {
    airPumpControl("big", false, 110);
    magnValveControl(0x204);
    //delay(500);
  }
  if (input.equals("mpuZAverage")) {
    mpu6050ZAvg = true;
  }
  if (input.equals("mpuTremorRating")) {
    mpu6050TremorRate = true;
  }
  if (input.equals("mpuAllAxis")) {
    mpu6050AxisValues = true;
  }
  if (input.equals("stopmpu")) {
    mpu6050ZAvg = false;
    mpu6050TremorRate = false;
    mpu6050AxisValues = false;
  }
  if (input.equals("bendCycle")) {
    flexionExtensionCycle = true;
  }
  if (input.equals("stopCycle")) {
    flexionExtensionCycle = false;
    flexionExtensionCycle_Blinker = false;
  }
  if (input.equals("readAllSensor")) {
    readAllSensor = true;
  }
  if (input.equals("stopReadingSensor")) {
    readAllSensor = false;
  }
  if (input.equals("mpuLifeAssistance")) {
    mpu6050LifeAssitance = true;
  }
  if (input.equals("test")) {
    test = true;
  }
  if (input.equals("stopTest")) {
    test = false;
  }
  if (input.equals("espSend")) {
    espSend = true;
  }
}

void pumpInterrupt_Safety() {
  airPumpControl("small", false, 50);
  airPumpControl("big", false, 50);
  openValve(0x01);
  openValve(0x02);
  openValve(0x04);
  openValve(0x08);
  openValve(0x10);
  openValve(0x20);
  openValve(0x40);
  openValve(0x80);
  openValve(0x100);
  openValve(0x200);
  delay(800);
  magnValveControl(0x00);
}

int readSensor(char board, int channel) {
  if (board == 'A') {
    digitalWrite(adExtendA0, (channel & 0x01));
    digitalWrite(adExtendA1, (channel & 0x02));
    digitalWrite(adExtendA2, (channel & 0x04));
    return analogRead(adExtendZ1);
  }
  if (board == 'B') {
    digitalWrite(adExtendB0, (channel & 0x01));
    digitalWrite(adExtendB1, (channel & 0x02));
    digitalWrite(adExtendB2, (channel & 0x04));
    return analogRead(adExtendZ2);
  }
}

void readFingerSensor() {
  /* implementing readSensor Function */
  while (readAllSensor == true) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      command(input);
      if (readAllSensor == false) {
        break;
      }
    }
    Serial.print('A ');

    for (int i = 0; i < 8; i++) {
      int val1 = readSensor('A', i);
      Serial.print(val1);
      Serial.print(' ');
      delay(5);
    }

    Serial.print(' B ');

    for (int j = 0; j < 8; j++) {
      int val2 = readSensor('B', j);
      Serial.print(val2);
      Serial.print(' ');
      delay(5);
    }
    Serial.println(' ');
    delay(20);
  }
}

void bendToAngle(int valve, int times, int angle) {
  int flexValue = ceil(angle * 0.8 + 95);  //TODO: figure out the relationship
  for (int i = 0; i < times; i++) {
    airPumpControl("big", true, 122);
    magnValveControl(valve);                  //valve has to be in form: 0xXX
    while (readSensor('B', 5) < flexValue) {  //hardcoded: 140
      Serial.print(readSensor('B', 5));
      Serial.print(" - inflating; ");
      Serial.print("Tip pressure- ");
      Serial.println(readSensor('B', 4));  // tip pressure
    }
    magnValveControl(0x00);
    airPumpControl("big", false, 100);
    Serial.println("Angle reached, deflation starts");
    delay(2000);

    magnValveControl(0x202);
    delay(2000);
    magnValveControl(0x00);  //valve has to be in form: 0xXX
    Serial.println("Extension completed, ready for flexion");
  }
}

void bendToAngle_Blinker(int valve, int times, int angle) {
  int flexValue = ceil(angle * 0.91 + 84);  //TODO: figure out the relationship
  for (int i = 0; i < times; i++) {
    airPumpControl("big", true, 120);
    magnValveControl(valve);                  //valve has to be in form: 0xXX
    while (readSensor('B', 5) < flexValue) {  //hardcoded: 140
      int angleToReturn = (readSensor('B', 5) - 84) / 0.91;
      Serial.print(angleToReturn);
      Serial.println(" - inflating");
      //Number_Bending.print(readSensor('B', 5));
    }
    magnValveControl(0x00);
    airPumpControl("big", false, 100);
    Serial.println("Angle reached, deflation starts");
    delay(2000);

    magnValveControl(0x202);
    delay(2000);
    magnValveControl(0x00);  //valve has to be in form: 0xXX
    Serial.println("Extension completed, ready for flexion");
  }
}

void addItem(MyData*& myList, int& currentSize, int& itemCount, MyData item) {
  if (itemCount == currentSize) {
    // Increase the size of the array
    currentSize *= 2;
    MyData* tempList = new MyData[currentSize];
    for (int i = 0; i < itemCount; i++) {
      tempList[i] = myList[i];
    }
    delete[] myList;
    myList = tempList;
  }
  myList[itemCount] = item;
  itemCount++;
}

MyData getItem(MyData* myList, int itemCount, int index) {
  if (index >= 0 && index < itemCount) {
    return myList[index];
  }
  // Handle error for invalid index
}

void mpu6050PeriodReading(int seconds, float averages[3], float parameters[3]) {
  unsigned long duration = seconds * 1000;
  unsigned long startTime = millis();
  float xCounter = 0, yCounter = 0, zCounter = 0;
  int readings = 0;
  int z_overValue = 0;

  while (millis() - startTime < duration) {
    mpu6050.update();
    xCounter += mpu6050.getAccX();
    yCounter += mpu6050.getAccY();
    zCounter += mpu6050.getAccZ();
    Serial.println(mpu6050.getAccZ());
    if (mpu6050.getAccZ() > mpu6050AccZ_Silent + mpu6050AccZ_Stddev || mpu6050.getAccZ() < mpu6050AccZ_Silent - mpu6050AccZ_Stddev) {
      z_overValue++;
    }
    readings++;
  }

  if (readings > 0) {
    averages[0] = (float)xCounter / readings;
    averages[1] = (float)yCounter / readings;
    averages[2] = (float)zCounter / readings;
    parameters[0] = readings;
    parameters[1] = z_overValue;
    parameters[2] = (float)z_overValue / readings;
  } else {
    //averages[0] = averages[1] = averages[2] = 0;
  }
}

void mpu6050Calibration_10Sec(float averages[], float& Stddev_Z) {
  //timing count
  unsigned long duration = 10 * 1000;
  unsigned long startTime = millis();

  //data storage Z axis- ArrayList in java
  const int INITIAL_SIZE = 10;
  MyData* myList = new MyData[INITIAL_SIZE];
  int currentSize = INITIAL_SIZE;
  int itemCount = 0;
  // Function to add an item

  float xCounter = 0, yCounter = 0, zCounter = 0;
  int readings = 0;

  while (millis() - startTime < duration) {
    mpu6050.update();
    xCounter += mpu6050.getAccX();
    yCounter += mpu6050.getAccY();
    zCounter += mpu6050.getAccZ();
    MyData data;
    data.value = mpu6050.getAccZ();
    addItem(myList, currentSize, itemCount, data);
    readings++;
    Serial.println("Reading");
  }

  Serial.println("Done");
  averages[0] = (float)xCounter / readings;
  averages[1] = (float)yCounter / readings;
  averages[2] = (float)zCounter / readings;

  float zCounter_Stddev = 0;
  for (int i = 0; i < itemCount; i++) {
    MyData currentItem = getItem(myList, itemCount, i);
    zCounter_Stddev += sq(currentItem.value - averages[2]);
  }
  Stddev_Z = sqrt(zCounter_Stddev / (itemCount - 1));

  Serial.print("zAvg-");
  Serial.print(averages[2]);
  Serial.print(" zStddev-");
  Serial.print(Stddev_Z);


  delete[] myList;
}

void mpu6050ZAverageValue(float& avg) {
  unsigned long duration = 10 * 1000;
  unsigned long startTime = millis();
  float zValue = 0;
  int readings = 0;
  float zMax = 0;
  while (millis() - startTime < duration) {
    mpu6050.update();
    float z = mpu6050.getAccZ();
    Serial.println(z);
    if (z > zMax) {
      zMax = z;
    }
    zValue += mpu6050.getAccZ();
    readings++;
    // Serial.print("MPU6050_Z value: ");
    // Serial.println(mpu6050.getAccZ());
  }
  avg = zValue / readings;
}

void mpu6050PrintAxisValues() {
  while (mpu6050AxisValues == true) {
    if (Serial.available() > 0) {
      String input = Serial.readStringUntil('\n');
      command(input);
      if (mpu6050AxisValues == false) {
        break;  // Exit the loop if mpu6050AxisValues was set to false
      }
    }
    mpu6050.update();
    Serial.print("X - ");
    Serial.print(mpu6050.getAccX());
    Serial.print("; Y - ");
    Serial.print(mpu6050.getAccY());
    Serial.print("; Z - ");
    Serial.println(mpu6050.getAccZ());

    delay(50);
  }
}

void mpuLifeAssistance(int seconds) {
  float staticCounter = 0;
  int readings = 0;
  float staticAvg;
  unsigned long startTime = millis();

  while (millis() - startTime < 10 * 1000) {
    mpu6050.update();
    float totalAcc = abs(mpu6050.getAccX()) + abs(mpu6050.getAccY()) + abs(mpu6050.getAccZ());
    Serial.print("totalAcc: ");
    Serial.println(totalAcc);
    staticCounter += totalAcc;
    readings++;
  }

  staticAvg = staticCounter / (float)readings;
  float movementThreshold = staticAvg * 0.2;
  Serial.print("StaticAvg / movementThreshold: ");
  Serial.print(staticAvg);
  Serial.print(" / ");
  Serial.println(movementThreshold);

  int movementCounter = 0;
  const int debounceCount = 5;
  boolean isMoving = false;
  boolean movementDetected = false;

  unsigned long duration = seconds * 1000;
  unsigned long startTime1 = millis();

  while (millis() - startTime1 < duration) {
    mpu6050.update();
    float totalAcc = abs(mpu6050.getAccX()) + abs(mpu6050.getAccY()) + abs(mpu6050.getAccZ());
    Serial.print("Detecting Movement: totalAcc - ");
    Serial.print(totalAcc);
    if (!isMoving && totalAcc > staticAvg + movementThreshold) {
      movementCounter = 0;
      isMoving = true;
      Serial.println(" ** Movement started");
    } else if (isMoving && totalAcc <= staticAvg + movementThreshold) {
      // isMoving = false;
      // movementDetected = true;
      // Serial.println("Movement ended");
      movementCounter++;
      if (movementCounter >= debounceCount) {  // Check if the condition has been stable
        isMoving = false;
        movementDetected = true;
        Serial.println("Movement ended");
        movementCounter = 0;  // Reset the counter after detecting movement
      }
    } else {
      movementCounter = 0;
    }

    Serial.print(" ;isMoving - ");
    Serial.println(isMoving);

    if (movementDetected) {
      Serial.println(" ** Movement completed");

      airPumpControl("big", true, 100);
      openValve(0x02);
      delay(1000);
      closeValve(0x02);
      airPumpControl("big", false, 100);
      delay(500);
      magnValveControl(0x202);
      delay(500);
      magnValveControl(0x00);

      movementDetected = false;  // Reset for the next movement
      isMoving = false;
      movementDetected = false;
      readings = staticCounter = 0;
      break;
    }
  }
  mpu6050LifeAssitance = false;
}

void airPumpControl(String airPump, boolean state, int speed) {
  if (airPump.equals("small")) {
    if (state == true) {
      digitalWrite(sAirPump1, HIGH);
      digitalWrite(sAirPump2, LOW);
      analogWrite(sAirPumpEna, speed);
    } else {
      digitalWrite(sAirPump1, LOW);
      digitalWrite(sAirPump2, LOW);
    }
  } else if (airPump.equals("big")) {
    if (state == true) {
      digitalWrite(bAirPump1, HIGH);
      digitalWrite(bAirPump2, LOW);
      analogWrite(bAirPumpEna, speed);
    } else {
      digitalWrite(bAirPump1, LOW);
      digitalWrite(bAirPump2, LOW);
    }
  }
}

void openValve(int valveBit) {
  valveState |= valveBit;  // Set the bit for the specified valve
  magnValveControl(valveState);
}

void closeValve(int valveBit) {
  valveState &= ~valveBit;  // Clear the bit for the specified valve
  magnValveControl(valveState);
}

void magnValveControl(int state) {
  digitalWrite(magnValve0, (state & 0x01));
  digitalWrite(magnValve1, (state & 0x02));
  digitalWrite(magnValve2, (state & 0x04));
  digitalWrite(magnValve3, (state & 0x08));
  digitalWrite(magnValve4, (state & 0x10));
  digitalWrite(magnValve5, (state & 0x20));
  digitalWrite(magnValve6, (state & 0x40));
  digitalWrite(magnValve7, (state & 0x80));
  digitalWrite(magnValve8, (state & 0x100) >> 8);
  digitalWrite(magnValve9, (state & 0x200) >> 8);
}
