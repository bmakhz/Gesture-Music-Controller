#include <Wire.h>
#include <Adafruit_VL53L0X.h>
#include <BleKeyboard.h>

// initialize sensors
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
BleKeyboard bleKeyboard("ESP32 Music Controller", "ESP32", 100);

// pins
const int TOUCH_PIN = 27; // TTP223 OUT
const int TRIG_PIN = 12;  // HC-SR04 Trig
const int ECHO_PIN = 13;  // HC-SR04 Echo

// VL53L0X constants 
const int MIN_DISTANCE_VL = 50;  // 5cm (mm)
const int MAX_DISTANCE_VL = 300; // 30cm (mm)
const int DISTANCE_THRESHOLD_VL = 50; // 50mm for volume gesture
const unsigned long GESTURE_DEBOUNCE_VL = 500; // 500ms

// HC-SR04 constants 
const int SWIPE_DISTANCE = 100; // 10cm (mm)
const unsigned long SWIPE_MIN_DURATION = 100; // min swipe time (ms)
const unsigned long SWIPE_MAX_DURATION = 1000; // max swipe time (ms)
const unsigned long RIGHT_SWIPE_THRESHOLD = 500; // right swipe < 500ms
const unsigned long SWIPE_DEBOUNCE = 1000; // 1s between swipes

int lastDistanceVL = 0;
unsigned long lastGestureTimeVL = 0;
unsigned long lastSwipeTime = 0;
unsigned long swipeStartTime = 0;
bool inSwipe = false;
bool lastTouchState = LOW;
unsigned long lastTouchChange = 0;
const unsigned long TOUCH_DEBOUNCE = 50; // 50ms

void setup() {
  Serial.begin(115200);
  Serial.println(F("Starting ESP32 Music Controller..."));

  // start I2C for VL53L0X
  Wire.begin();
  if (!lox.begin()) {
    Serial.println(F("ERROR: VL53L0X failed to initialize!"));
    while (1);
  }
  Serial.println(F("VL53L0X initialized"));

  // initialize pins
  pinMode(TOUCH_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println(F("TTP223 and HC-SR04 initialized"));

  // start Bluetooth
  bleKeyboard.begin();
  Serial.println(F("Bluetooth keyboard started"));
}

void loop() {
  // check Bluetooth connection
  if (bleKeyboard.isConnected()) {
    Serial.println(F("Bluetooth connected"));

    // VL53L0X for volume control
    VL53L0X_RangingMeasurementData_t measure;
    lox.rangingTest(&measure, false);
    if (measure.RangeStatus != 4) {
      int currentDistance = measure.RangeMilliMeter;
      Serial.print(F("VL53L0X Distance (mm): "));
      Serial.println(currentDistance);

      if (currentDistance >= MIN_DISTANCE_VL && currentDistance <= MAX_DISTANCE_VL &&
          millis() - lastGestureTimeVL > GESTURE_DEBOUNCE_VL) {
        if (lastDistanceVL != 0) {
          int distanceChange = currentDistance - lastDistanceVL;
          if (distanceChange > DISTANCE_THRESHOLD_VL) {
            Serial.println(F("Backward gesture: Volume DOWN"));
            bleKeyboard.press(KEY_MEDIA_VOLUME_DOWN);
            delay(10);
            bleKeyboard.release(KEY_MEDIA_VOLUME_DOWN);
            lastGestureTimeVL = millis();
          } else if (distanceChange < -DISTANCE_THRESHOLD_VL) {
            Serial.println(F("Forward gesture: Volume UP"));
            bleKeyboard.press(KEY_MEDIA_VOLUME_UP);
            delay(10);
            bleKeyboard.release(KEY_MEDIA_VOLUME_UP);
            lastGestureTimeVL = millis();
          }
        }
        lastDistanceVL = currentDistance;
      }
    } else {
      Serial.println(F("VL53L0X: Out of range"));
      lastDistanceVL = 0;
    }

    // HC-SR04 
    long distance = getDistance(TRIG_PIN, ECHO_PIN);
    if (distance >= 0) {
      Serial.print(F("HC-SR04 Distance (mm): "));
      Serial.println(distance);
    } else {
      Serial.println(F("HC-SR04: Invalid reading"));
    }

    if (millis() - lastSwipeTime > SWIPE_DEBOUNCE) {
      if (distance > 0 && distance <= SWIPE_DISTANCE && !inSwipe) {
        Serial.println(F("HC-SR04: Hand detected, starting swipe"));
        swipeStartTime = millis();
        inSwipe = true;
      } else if ((distance > SWIPE_DISTANCE || distance < 0) && inSwipe) {
        unsigned long swipeDuration = millis() - swipeStartTime;
        if (swipeDuration >= SWIPE_MIN_DURATION && swipeDuration <= SWIPE_MAX_DURATION) {
          if (swipeDuration < RIGHT_SWIPE_THRESHOLD) {
            Serial.println(F("Right swipe: Next track"));
            bleKeyboard.press(KEY_MEDIA_NEXT_TRACK);
            delay(10);
            bleKeyboard.release(KEY_MEDIA_NEXT_TRACK);
          } else {
            Serial.println(F("Left swipe: Previous track"));
            bleKeyboard.press(KEY_MEDIA_PREVIOUS_TRACK);
            delay(10);
            bleKeyboard.release(KEY_MEDIA_PREVIOUS_TRACK);
          }
          lastSwipeTime = millis();
        } else {
          Serial.print(F("Swipe rejected, duration: "));
          Serial.println(swipeDuration);
        }
        inSwipe = false;
      }
    }

    // TTP223 
    bool touchState = digitalRead(TOUCH_PIN);
    if (touchState != lastTouchState && millis() - lastTouchChange > TOUCH_DEBOUNCE) {
      if (touchState == HIGH) {
        Serial.println(F("TTP223: Play/Pause triggered"));
        bleKeyboard.press(KEY_MEDIA_PLAY_PAUSE);
        delay(10);
        bleKeyboard.release(KEY_MEDIA_PLAY_PAUSE);
      }
      lastTouchState = touchState;
      lastTouchChange = millis();
    }
  } else {
    Serial.println(F("Bluetooth NOT connected. Please pair with 'ESP32 Music Controller'."));
    delay(1000); // slow loop when not connected
  }
  delay(50);
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.34 / 2; // Distance in mm
}