

#define TOLERANCE_AMOUNT  5
#define SWITCH_ANALOG_IN  0
#define MIDI_NOTE_OFFSET  35
#define MAX_COUNT_TRIGGER 200

int read_values[14] = {
    464,  // A# - 11
    486,  // B  - 12
    512,  // G# - 9
    537,  // C1 - 13
    568,  // F# - 7
    603,  // A  - 10
    640,  // D  - 3
    683,  // G  - 8
    730,  // C# - 2
    787,  // F  - 6
    850,  // E  - 5
    930,  // D# - 4
    1024, // C0 - 1
    0
};

int note_indexes[14] = {
    11,
    12,
    9,
    13,
    7,
    10,
    3,
    8,
    2,
    6,
    5,
    4,
    1,
    0
 };
 
// *************************************************************
void SendMidiNote(int inNote, bool inOn) 
{
  if (inNote > 0) {
    if (inOn) {
//      Serial.print(inNote);
//      Serial.print(" On\n");
      Serial3.write(0x90);
    }
    else {
//      Serial.print(inNote);
//      Serial.print(" Off\n");
      Serial3.write(0x80);
    }
    
    Serial3.write(inNote);
    Serial3.write(127);
  }
}

// *************************************************************
int GetArrayIndex(int inValue) {
  int last_value = 0;
  int index = 0;
  while (read_values[index] != 0) {
    int cur_value = read_values[index];
    int lower_mid = (last_value + cur_value)/2;
    if (inValue < lower_mid) {
      return index-1;
    }
    last_value = cur_value;
    index++;
  }
  return index-1;
}

int count_array[14];

// *************************************************************
void AddArrayCount(int inIndex) {
  count_array[inIndex]++;
}

// *************************************************************
int GetArrayCount(int inIndex) {
  return count_array[inIndex];
}

// *************************************************************
void PrintCountArray() {
  for (int index = 0; index < 14; ++index) {
    int count = count_array[index];
    if (count > 0) {
      Serial.print("count[");
      Serial.print(index);
      Serial.print("] = ");
      Serial.print(count);
      Serial.print("\n");
    }
  }
}

// *************************************************************
void ClearCountArray() {
  for (int index = 0; index < 14; ++index) {
    count_array[index] = 0;
  }
}


// *************************************************************
void setup() {
  pinMode(45, OUTPUT);
  pinMode(53, OUTPUT);
  digitalWrite(45, LOW);
  digitalWrite(53, HIGH);

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(19200);
  Serial3.begin(31250);
  ClearCountArray();
}

int last_array_index = -1;
int max_array_index = 0;
int last_note = 0;
int last_note_count = 0;

// *************************************************************
void loop() {
  int analog_value = analogRead(SWITCH_ANALOG_IN);
//  Serial.print(analog_value);
//  Serial.print("\n"); 
  
  int cur_array_index = GetArrayIndex(analog_value);
  if (cur_array_index > max_array_index) {
    max_array_index = cur_array_index;
  }

  if (cur_array_index < 0) {
    if (cur_array_index != last_array_index) {
      int max_note = note_indexes[max_array_index];
//      Serial.print("max_array_index: ");
//      Serial.print(max_array_index);
//      Serial.print(" note: ");
//      Serial.print(max_note);
//      Serial.print("\n"); 
//      PrintCountArray();
      max_array_index = -1;
      ClearCountArray();
      SendMidiNote(last_note, false);
      last_note = 0;
    }
  }
  else {
    if (last_note == 0) {
      AddArrayCount(cur_array_index);
      int max_count = GetArrayCount(max_array_index);
      if (max_count > MAX_COUNT_TRIGGER) {
        last_note = note_indexes[max_array_index] + 35;
        SendMidiNote(last_note, true);
      }
    }
  }
  last_array_index = cur_array_index;
}
