#include <Arduino.h>
#include <hamming.hpp>
#define MIN(a,b) a>b?b:a
#define MAX(a,b) a>b?a:b

#define FASTADC 1

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

int LASER = PD6;
// SMD
int SENSOR_OUT = A4;
int SENSOR_VCC = A2;
int SENSOR_GND = A3;
// COMMON
/*int SENSOR_OUT = A3;
int SENSOR_VCC = A2;
int SENSOR_GND = A4;*/
int DELAY_US = 0;

char* MESSAGE = "Pozdrav z Marsu o7";
int CHAR_LEN = 8;
int MESSAGE_LEN = strlen(MESSAGE);

int thresshold = 0;
int minReceived = INT16_MAX;
int maxReceived = INT16_MIN;
int sent = 0;
int received = 0;

int receive() {
  delayMicroseconds(DELAY_US);
  int value = analogRead(SENSOR_OUT);
  minReceived = MIN(minReceived, value);
  maxReceived = MAX(maxReceived, value);
  received++;
  return value;
}

bool toData(int value, int thresshold) {
  bool data = value > thresshold;
  return data;
}

void transmit(bool data) {
  //digitalWrite(LASER, data);
  if(data) PORTD |= 1<<LASER;
  else     PORTD &= ~(1<<LASER);
  sent++;
}

/// @brief Computes current thresshold value from sensor data 
int quickSync() {
  char mask = 0b10111001;
  int thresshold = 0;
  bool synced = false;
  DELAY_US = 0;
  do {
    int highMin = INT16_MAX;
    int highMax = INT16_MIN;
    int lowMin = INT16_MAX;
    int lowMax = INT16_MIN;
    int values[8];
    // Send and receive to get both values and range
    for(int i = 0; i < 8; i++) {
      bool bit = (mask >> i) & 1;
      transmit(bit);
      int value = receive();
      if(bit) {
        highMin = MIN(highMin, value);
        highMax = MAX(highMax, value);
      } else {  
        lowMin = MIN(lowMin, value);
        lowMax = MAX(lowMax, value);
      }
      values[i] = value;
    }

    transmit(LOW);
    // Compute thresshold for binary encoding
    thresshold = (highMin - lowMax) / 2 + lowMax;
    
    Serial.print(">thresshold:");
    Serial.println(thresshold);
    Serial.print(">high_min:");
    Serial.println(highMin);
    Serial.print(">high_max:");
    Serial.println(highMax);
    Serial.print(">low_min:");
    Serial.println(lowMin);
    Serial.print(">low_max:");
    Serial.println(lowMax);
    Serial.print(">delay:");
    Serial.print(DELAY_US);
    Serial.println("§us");
    
    // Encode values using thresshold and validate data
    if(thresshold > highMin || thresshold < lowMax) {      
      Serial.print(">err:");
      Serial.println(2);
      Serial.flush();
      DELAY_US++;
      if(DELAY_US > 1000) DELAY_US = 1;
      break;
    }

    for(int i = 0; i < 8; i++) {
      bool sent = (mask >> i) & 1;
      bool received = toData(values[i], thresshold);
      synced = sent == received;
      if(!synced) {    
        Serial.print(">err:");
        Serial.println(1);
        Serial.flush();
        DELAY_US++;
        if(DELAY_US > 1000) DELAY_US = 1;
        break;
      }
    }
    
    Serial.print(">err:");
    Serial.println(0);
  } while(!synced);
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
  Serial.print(">connected:");
  Serial.println(ok);
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
  pinMode(SENSOR_VCC, OUTPUT);
  digitalWrite(SENSOR_VCC, HIGH);
  pinMode(SENSOR_GND, OUTPUT);
  digitalWrite(SENSOR_GND, LOW);
  pinMode(SENSOR_OUT, OUTPUT);
  digitalWrite(SENSOR_OUT, LOW);
  pinMode(SENSOR_OUT, INPUT);

  
  #if FASTADC
  // set prescale to 16
  sbi(ADCSRA,ADPS2) ;
  cbi(ADCSRA,ADPS1) ;
  cbi(ADCSRA,ADPS0) ;
  #endif
}

void reset(char* receivedMessage) {
  for(int i = 0; i < MESSAGE_LEN; i++) receivedMessage[i] = '~';
  
  thresshold = 0;
  minReceived = INT16_MAX;
  maxReceived = INT16_MIN;
  sent = 0;
  received = 0;
}

void log() {
  Serial.print(">received:");
  Serial.println(received);
  Serial.print(">sent:");
  Serial.println(sent);
  Serial.print(">lux_min:");
  Serial.println(minReceived);
  Serial.print(">lux_max:");
  Serial.println(maxReceived);
  Serial.flush();
}

bool toggle = false;
// put your main code here, to run repeatedly
void loop() {
  char receivedMessage[MESSAGE_LEN];
  reset(receivedMessage);

  // Sync transmitter and reciever, to get light thressholds
  int thresshold = quickSync();
  // Wait for connection between those two
  /*bool connected = quickTest(thresshold);
  if(!connected) {
    // Skip rest if disconnected
    log();
    return;
  }*/

  /*bool in = !toData(receive(), thresshold);
  toggle = !toggle;
  transmit(toggle);
  bool out = receive();

  Serial.print(">received:");
  Serial.println(out);
  Serial.print(">sent:");
  Serial.println(in);
  Serial.flush();

  return;*/

  // Transmit & Receive message  
  long start_msg = micros();
  long flashes_msg = 0;
  int valid = 0;
  long time_char_min = INT32_MAX;
  long time_char_max = INT32_MIN;
  long flashes_char_min = INT32_MAX;
  long flashes_char_max = INT32_MIN;
  for(int i = 0; i < MESSAGE_LEN; i++){
    
    char encoded_lenght = 0;
    int letter = encode_data(MESSAGE[i], CHAR_LEN, &encoded_lenght);
    int receivedLetter = 0;
    
    long start_char = micros();
    long flashes_char = 0;
    for(int j = 0; j < encoded_lenght; j++) {
      // Transmit bit
      bool sent = (letter >> j) & 1;
      transmit(sent);
      flashes_char++;
      flashes_msg++;

      // Receive bit
      bool received = toData(receive(), thresshold);
      receivedLetter |= received << j;

      // Serial monitor output
      if(sent == received) valid++;
    }
    long end_char = micros();
    long timeChar = end_char - start_char;
    time_char_min = MIN(time_char_min, timeChar);
    time_char_max = MAX(time_char_max, timeChar);
    flashes_char_min = MIN(flashes_char_min, flashes_char);
    flashes_char_max = MAX(flashes_char_max, flashes_char);

    char received = decode_data(receivedLetter, encoded_lenght);
    if(isPrintable(received)) {
      receivedMessage[i] = received;
    } else {
      receivedMessage[i] = '#';
    }
  }
  long end_msg = micros();
  
  // Stop transmitting
  transmit(LOW);
  
  Serial.print(">t_min_char:");
  Serial.print(time_char_min);
  Serial.println("§us");
  Serial.print(">t_max_char:");
  Serial.print(time_char_max);
  Serial.println("§us");
  Serial.print(">f_min_char:");
  Serial.println(flashes_char_min);
  Serial.print(">f_max_char:");
  Serial.println(flashes_char_max);
  Serial.print(">t_msg:");
  Serial.print(end_msg-start_msg);
  Serial.println("§us");
  Serial.print(">f_msg:");
  Serial.println(flashes_msg);
  Serial.print(">valid:");
  Serial.println(valid);
  Serial.print(">msg_got:");
  Serial.write(receivedMessage, MESSAGE_LEN);
  Serial.println("|t");
  log();
}