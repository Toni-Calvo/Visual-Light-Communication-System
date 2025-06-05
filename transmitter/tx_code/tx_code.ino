const int bit_duration = 10000; // us
const int half_bit_duration = bit_duration / 2;
const int quarter_bit_duration = bit_duration / 4;
char data;            // transmission data
bool tx;              // transmitting flag
unsigned long time;   // time between pulse start and logic
int offset = 3;       // bits between transmission


void setup() {
  // Initialization

  Serial.begin(9600);
  pinMode(13, OUTPUT);
  tx = false;
  while (!Serial) {;}
  Serial.println("TX Ready");
}


void loop() {
  if (tx) {
    // start transmission

    sendData();
    tx = false;
  }
  else {
    // no transmission

    sendBit(0);
    time = micros();
    tx = getData();   // check for wanted transmissions
    delayMicroseconds(half_bit_duration - micros() + time);
  } 
}


void sendBit(int bit) {
  // Sends a bit through the channel

  if (bit == 0) {
    digitalWrite(13, LOW);
    delayMicroseconds(half_bit_duration);
    digitalWrite(13, HIGH);
  }
  else {
    digitalWrite(13, HIGH);
    delayMicroseconds(half_bit_duration);
    digitalWrite(13, LOW);
  }
}


bool getData() {
  // Checks if the user wants to transmit data

  if (Serial.available() > 0 && offset == 0) {
    // data found

    offset = 3;
    data = Serial.read();
    Serial.print("Character '");
    Serial.print(data);
    Serial.println("'");
    return true;
  }
  if (offset != 0) {
    // delay between bytes

    offset--;
  }

  return false;
}


void sendData() {
  // Sends the character in data through the channel

  startSequence();
  
  for (int i = 7; i >= 0; i--) {
    if ((data >> i) & 0x01) {
      // sends bit 1

      sendBit(1);
      Serial.print("1");
      delayMicroseconds(half_bit_duration);
    }
    else {
      // sends bit 0

      sendBit(0);
      Serial.print("0");
      delayMicroseconds(half_bit_duration);
    }
  }
  Serial.println();
}


void startSequence() {
  // Sends the start sequence (bit start)

  digitalWrite(13, LOW);
  delayMicroseconds(quarter_bit_duration);
  digitalWrite(13, HIGH);
  delayMicroseconds(quarter_bit_duration);
  digitalWrite(13, LOW);
  delayMicroseconds(quarter_bit_duration);
  digitalWrite(13, HIGH);
  delayMicroseconds(quarter_bit_duration);
}