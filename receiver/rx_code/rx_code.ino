const int bit_duration = 10000; // us
const int idle_duration = bit_duration * 3/8;      // maximum time for the bit start HIGH state
const int same_bit_duration = bit_duration * 3/4;  // maximum time for a state (H or L) of the same bit

unsigned long dt;           // delta time (time between edges)
unsigned long last_change;  // time stamp of the last edge
bool change = false;        // input chaged (H-L or L-H)
bool idle = true;           // idle state (no transmission)
bool save = false;          // save state (transmission)
int last_bit = 0;           // last bit detected (0: "0", 1: "1", 2: "bit start")
bool data[8];               // saved data
int bit_index;              // index of current transmission bit

void setup() {
  // Initialization

  Serial.begin(9600);
  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), handlePinChange, CHANGE);
  while(!Serial) {;}
  Serial.println("RX Ready");
  dt = 0;
  last_change = micros();
}

void loop() {
  // Logic Block

  if (change) {
    if (idle) {
      if (dt < idle_duration) {
        // Detects the first pulse of the bit start

        idle = false;
        last_bit = 2;
      }
    }
    else {
      if (save) {
        // Saves the bits into data

        if (last_bit == 0) {
          if (dt < same_bit_duration) { 
            // SAVE "0"
            data[bit_index] = last_bit;
            last_bit = 0;
          }
          else {  
            // SAVE "1"
            data[bit_index] = last_bit;
            last_bit = 1;
          }
          
          bit_index++;
        }
        else if (last_bit == 1) {
          if (dt < same_bit_duration) { 
            // SAVE "1"
            data[bit_index] = last_bit;
            last_bit = 1;
          }
          else {  
            // SAVE "0"
            data[bit_index] = last_bit;
            last_bit = 0;
          }
          
          bit_index++;
        }
      }
      else {
        if (dt < idle_duration){
          // Detects the second pulse of the bit start and starts the save state

          save = true;
          last_bit = 0; // first bit is always 0 for alphabetic characters in ASCII
          bit_index = 0;
        }
        else {
          // Return to idle state

          idle = true;
        }
      }

      if (bit_index == 8) {
        // Transmission has finished
        
        idle = true;
        save = false;
        Serial.print("Character: ");
        byte ascii = 0;
        for (int i = 0; i < 8; i++) {
          if (data[i]) {
            ascii |= (1 << (7 - i));
            Serial.print(1);
          }
          else {
            Serial.print(0);
          }
        }
        Serial.print(" ");
        Serial.println((char)ascii);
      }
    }
    change = false;
  }
}


void handlePinChange() {
  // Detects rising and falling edges and triggers the change flag to detect if its a "0" or a "1"

  int pin_state = digitalRead(2);

  if ((last_bit != 1 && pin_state == LOW) || (last_bit == 1 && pin_state == HIGH)) {
    // Triggers change flag to start the logic block in loop

    dt = micros() - last_change;
    change = true;
  }

  last_change = micros();
}