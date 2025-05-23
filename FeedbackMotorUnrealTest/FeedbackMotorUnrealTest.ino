struct feedbackConfig
{
  uint8_t pin = 0;
  uint8_t strength = 0;

  union
  {
    float value;
    byte buffer[4];
  } duration; // // Clamped to 2 s max
};

const int timeDelay = 20; // in ms
const float delayInS = timeDelay / 1000.f;
uint8_t dummyBuffer[4];
feedbackConfig config[5];

uint8_t alpha = 0;

/**
* 0 => thumb (pin 3)
* 1 => index (pin 5)
* 2 => middle (pin 6)
* 3 => ring (pin 9)
* 4 => pinky (pin 10)
*/

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  config[0].pin = A0;
  config[1].pin = A1;
  config[2].pin = A2;
  config[3].pin = A3;
  config[4].pin = A4;

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(D6, OUTPUT);
}

void processData() {
    // Received the sanitized data
  while (Serial.available() > 0)
  {
    // Make sure we have enough bytes to read the finger data
    if (Serial.available() < 8)
    {
      break;
    }

    // Read the incoming bytes
    uint8_t finger = Serial.read();
    uint8_t strength = Serial.read();
    if (finger > 4)
    {
      // Out of range, should never happen, but better to be safe, clean out the last 4 bytes too
      Serial.readBytes(dummyBuffer, 4);
      uint8_t padding0 = Serial.read();
      uint8_t padding1 = Serial.read();
      continue;
    }

    config[finger].duration.value = 0;
    size_t readData = Serial.readBytes(config[finger].duration.buffer, 4);

    if (config[finger].duration.value > 2.f)
    {
      config[finger].duration.value = 2.f;
    }

    // Overwrite the currently stored values
    config[finger].strength = strength;

    // Read and remove padding
    uint8_t padding0 = Serial.read();
    uint8_t padding1 = Serial.read();
  }

  for (int i = 0; i < 5; ++i)
  {
    if (config[i].duration.value > 0.02f)
    {
      analogWrite(config[i].pin, config[i].strength);
      
      config[i].duration.value -= delayInS;

      if (config[i].duration.value <= 0)
      {
        config[i].duration.value = 0;
        config[i].strength = 0;
      }
    }
    else
    {
      // Stop vibration
      analogWrite(config[i].pin, 0);
    }
  }

  delay(timeDelay);
}

void test() {
  analogWrite(config[0].pin, alpha);
  analogWrite(config[1].pin, alpha);
  analogWrite(config[2].pin, alpha);
  analogWrite(config[3].pin, alpha);
  analogWrite(config[4].pin, alpha);

  ++alpha %= 255;
  delay(20.f);
}

void loop() {
  //test();
  processData();
}
