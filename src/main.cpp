#include <Arduino.h>
#define MIN(a,b) a>b?b:a
#define MAX(a,b) a>b?a:b

int LASER = PD6;
int SENSOR = PC4;
int DELAY_US = 10000;

char* MESSAGE = "Pozdrav z Marsu o7";
int CHAR_LEN = 8;
int MESSAGE_LEN = strlen(MESSAGE);

int thresshold = 0;
bool connected = false;
bool sent = false;
bool received = false;
bool valid = false;

void log() {
  long time = millis();
  Serial.print(">thresshold:");
  Serial.print(time);
  Serial.print(":");
  Serial.println(thresshold);
  Serial.print(">connected:");
  Serial.print(time);
  Serial.print(":");
  Serial.println(connected);
  Serial.print(">sent:");
  Serial.print(time);
  Serial.print(":");
  Serial.println(sent);
  Serial.print(">received:");
  Serial.print(time);
  Serial.print(":");
  Serial.println(received);
  Serial.print(">valid:");
  Serial.print(time);
  Serial.print(":");
  Serial.println(valid);
}

int receive() {
  int value = 1024 - analogRead(SENSOR);
  long time = millis();
  Serial.print(">raw:");
  Serial.print(time);
  Serial.print(":");
  Serial.println(value);
  return value;
}

bool toData(int value, int thresshold) {
  bool data = value > thresshold;
  return data;
}

void transmit(bool data) {
  digitalWrite(LASER, data);
  delayMicroseconds(DELAY_US);
}

/// @brief Computes current thresshold value from sensor data 
int quickSync() {
  char mask = 0b10111001;
  int minHigh = INT16_MAX;
  int maxLow = INT16_MIN;
  for(int i = 0; i < 8; i++) {
    bool bit = (mask >> i) & 1;
    transmit(bit);
    int value = receive();
    if(bit) {
      minHigh = MIN(minHigh, value);
    } else {  
      maxLow = MAX(maxLow, value);
    }
  }
  transmit(LOW);
  thresshold = (minHigh - maxLow) / 2 + maxLow;
  return thresshold;
}

/// @brief Tests connection link
bool quickTest(int thresshold) {
  char mask = 0b11001101;
  bool ok = true;
  for(int i = 0; i < 8; i++) {
    bool bit = (mask >> i) & 1;
    transmit(bit);
    if(toData(receive(), thresshold) != bit) {
      ok = false;
      break;
    }
  }
  transmit(LOW);
  connected = ok;
  return ok;
}


// put your setup code here, to run once
void setup() {
  // Serial monitor output stup
  Serial.begin(230400);
  Serial.println("Serial monitor connected.");
  Serial.print("Message: ");
  Serial.println(MESSAGE);
  Serial.print("Size: ");
  Serial.print(MESSAGE_LEN);
  Serial.println(" bytes");
  Serial.print(">msg_sent:");
  Serial.print(MESSAGE);
  Serial.println("|t");
  Serial.print(">msg_got:");
  Serial.print(MESSAGE);
  Serial.println("|t");
  Serial.flush();
  
  // Setup sender pins
  pinMode(LASER, OUTPUT);
  digitalWrite(LASER, LOW);

  // Setup receiver pins
  pinMode(SENSOR, INPUT);
}

// put your main code here, to run repeatedly
void loop() {
  // Sync transmitter and reciever, to get light thressholds
  int thresshold = quickSync();
  // Wait for connection between those two
  bool connected = quickTest(thresshold);
  if(!connected) {
    // Skip rest if disconnected
    log();
    Serial.flush();
    return;
  }

  // Transmit & Receive message
  char receivedMessage[MESSAGE_LEN];
  for(int i = 0; i < MESSAGE_LEN; i++) receivedMessage[i] = '~';
  Serial.print(">msg_got:");
  Serial.write(receivedMessage, MESSAGE_LEN);
  Serial.println("|t");
  Serial.flush();
  
  long start_msg = millis();
  long flashes_msg = 0;
  for(int i = 0; i < MESSAGE_LEN; i++){
    
    char letter = MESSAGE[i];
    char receivedLetter = 0;
    
    long start_char = millis();
    long flashes_char = 0;
    for(int j = 0; j < CHAR_LEN; j++) {
      // Transmit bit
      sent = (letter >> j) & 1;
      transmit(sent);
      flashes_char++;
      flashes_msg++;

      // Receive bit
      received = toData(receive(), thresshold);
      receivedLetter |= received << j;

      // Serial monitor output
      valid = sent == received;
      log();
    }
  
    long end_char = millis();
    Serial.print(">t_char:");
    Serial.print(end_char-start_char);
    Serial.println("§ms");
    Serial.print(">f_char:");
    Serial.println(flashes_char);

    if(isPrintable(receivedLetter)) {
      receivedMessage[i] = receivedLetter;
    } else {
      receivedMessage[i] = '#';
    }

    Serial.print(">msg_got:");
    Serial.write(receivedMessage, MESSAGE_LEN);
    Serial.println("|t");
    Serial.flush();
  }
  long end_msg = millis();
  
  // Stop transmitting
  transmit(LOW);

  Serial.print(">t_msg:");
  Serial.print(end_msg-start_msg);
  Serial.println("§ms");
  Serial.print(">f_msg:");
  Serial.println(flashes_msg);
  Serial.flush();
}