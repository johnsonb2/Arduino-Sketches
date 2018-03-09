#include "pitches.h"

#define AUDIO_PIN 3
#define BAUD_RATE 4800

String sequence = "";
const int defTempo = 150;
const int defOctave = 4;
int tempo = defTempo;
int octave = defOctave;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  pinMode(AUDIO_PIN, OUTPUT);
  Serial.println("Setup complete. Awaiting sequences.");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available() > 0) {
    // read next sequence
    sequence = Serial.readString();
    Serial.println(sequence);
    sequence.toLowerCase();
    sequence.trim();
    for(int i = 0; i < sequence.length(); i++) {
      if(sequence[i] == ' ' || sequence[i] == '\t') {
        sequence.remove(i, 1);
      }
    }

    // Keep track of commands
    String commands[sequence.length()];
    // Keep track of parameters related to commands
    String inputs[sequence.length()];
    // set reasonable length for commands and inputs

    // keep track of command & input pointer
    int count = 0;
    // keep track of beginning of input string
    int startInputPointer = 0;
    for(int i = 0; i < sequence.length();) {
      // check for command
      if(isCommand(sequence[i])) {
        // check if command is two characters
        if(isNote(sequence[i]) && canHaveSharp(sequence[i])) {
          if(sequence[i+1] == '+') {
            commands[count] = sequence.substring(i, i+2);
            i++;
          }
          // does not have two character command
          else {
            commands[count] = sequence[i];
          }
        }
        else {
          // not a note
          commands[count] = sequence[i];
        }
        i++;
        // figure out corresponding input for command
        startInputPointer = i;
        while(!isCommand(sequence[i]) && i < sequence.length()) {
          i++; 
        }
        inputs[count] = sequence.substring(startInputPointer, i);
        count++;
      }
      // invalid input, skip
      else i++;
    }

    // execute commands
    for(int i = 0; i < sizeof(commands)/sizeof(String); i++) {
      if(isNote(commands[i][0])) {
        note(commands[i], inputs[i]);
      }
      else if(commands[i][0] == 'r') {
        rest(inputs[i]);
      }
      else if(commands[i][0] == 't') {
        setTempo(convertToInt(inputs[i]));
      }
      else if(commands[i][0] == 'o') {
        absOctaveChange(convertToInt(inputs[i]));
      }
      else if(commands[i][0] == '>' || commands[i][0] == '<') {
        relOctaveChange(commands[i]);
      }
    }
    Serial.println("Pass complete");
    tempo = defTempo;
    octave = defOctave;
  }
}

// Checks to see if the input is a note
bool isNote(char input) {
  if(input == 'c' || input == 'd' || input == 'e' || input == 'f' || 
    input == 'g' || input == 'a' || input == 'b') {
          return true;
  }
  else return false;
  
}

// Checks to see if the note can be sharp
bool canHaveSharp(char input) {
  return input == 'c' || input == 'd' || input == 'f' || input == 'g' || input == 'a';
}

bool isCommand(char input) {
  return input == 'c' || input == 'd' || input == 'e' || input == 'f' ||
         input == 'g' || input == 'a' || input == 'b' || input == 'r' || 
         input == 't' || input == 'o' || input == '<' || input == '>';
}

// find a function that can do this for you
int convertToInt(String input) {
  int retVal = 0;
  for(int i = 0; i < input.length(); i++) {
    int place = input.length();
    int temp = (input[i] - 48);
    for(int j = 0; j < place - i - 1; j++) {
      temp *= 10;
    }
    retVal += temp;
  }
  return retVal;
}

// plays a note at the specified length
void note(String note, String seq) {
  String lengths[seq.length()]; 
  float totalTime = 0; 
  for(int i, start, count = 0; i < seq.length(); i++) {
    if(i + 1 >= seq.length()) {
      lengths[count++] = seq.substring(start, seq.length());
    }
    else if(seq[i] == '&') {
      lengths[count++] = seq.substring(start, i);
      start = i + 1;
    }
  }
  for(int i = 0; i < sizeof(lengths) / sizeof(String); i++) {
    Serial.println(lengths[i]);
  }
  for(int i = 0; i < sizeof(lengths) / sizeof(String); i++) {
    int temp = convertToInt(lengths[i]);
    if(temp == 1) {
      totalTime += 1.0f;
    }
    else if (temp == 2) {
      totalTime += 0.5f;
    }
    else if (temp == 4) {
      totalTime += 0.25f;
    }
    else if (temp == 8) {
      totalTime += 0.125f;
    }
    else if (temp == 16) {
      totalTime += 0.0625f;
    }
    else if (temp == 32) {
      totalTime += 0.03125f;
    }
  }
  float duration = (totalTime / (tempo / 60)) * 10000;
  Serial.println(duration);
  if(note == "c") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_C1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_C2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_C3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_C4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_C5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_C6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_C7, (unsigned long)duration);
    else if (octave == 8) tone(AUDIO_PIN, NOTE_C8, (unsigned long)duration);
    delay(duration);
  } 
  else if (note == "c+") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_CS1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_CS2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_CS3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_CS4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_CS5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_CS6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_CS7, (unsigned long)duration);
    else if (octave == 8) tone(AUDIO_PIN, NOTE_CS8, (unsigned long)duration);
    delay(duration);
  }
  else if (note == "d") {    
    if(octave == 1) tone(AUDIO_PIN, NOTE_D1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_D2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_D3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_D4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_D5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_D6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_D7, (unsigned long)duration);
    else if (octave == 8) tone(AUDIO_PIN, NOTE_D8, (unsigned long)duration);
    delay(duration);
  }
  else if (note == "d+") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_DS1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_DS2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_DS3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_DS4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_DS5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_DS6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_DS7, (unsigned long)duration);
    else if (octave == 8) tone(AUDIO_PIN, NOTE_DS8, (unsigned long)duration);
    delay(duration);
  }
  else if (note == "e") {    
    if(octave == 1) tone(AUDIO_PIN, NOTE_E1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_E2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_E3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_E4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_E5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_E6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_E7, (unsigned long)duration);
    delay(duration);
  } 
  else if (note == "f") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_F1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_F2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_F3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_F4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_F5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_F6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_F7, (unsigned long)duration);
    delay(duration);
  } 
  else if (note == "f+") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_FS1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_FS2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_FS3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_FS4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_FS5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_FS6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_FS7, (unsigned long)duration);
    delay(duration);
  } 
  else if (note == "g") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_G1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_G2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_G3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_G4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_G5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_G6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_G7, (unsigned long)duration);
    delay(duration);
  }
  else if (note == "g+") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_GS1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_GS2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_GS3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_GS4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_GS5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_GS6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_GS7, (unsigned long)duration);
    delay(duration);
  } 
  else if(note == "a") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_A1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_A2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_A3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_A4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_A5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_A6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_A7, (unsigned long)duration);
    delay(duration);
  } 
  else if(note == "a+") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_AS1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_AS2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_AS3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_AS4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_AS5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_AS6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_AS7, (unsigned long)duration);
    delay(duration);
  } 
  else if (note == "b") {
    if(octave == 1) tone(AUDIO_PIN, NOTE_B1, (unsigned long)duration);
    else if (octave == 2) tone(AUDIO_PIN, NOTE_B2, (unsigned long)duration);
    else if (octave == 3) tone(AUDIO_PIN, NOTE_B3, (unsigned long)duration);
    else if (octave == 4) tone(AUDIO_PIN, NOTE_B4, (unsigned long)duration);
    else if (octave == 5) tone(AUDIO_PIN, NOTE_B5, (unsigned long)duration);
    else if (octave == 6) tone(AUDIO_PIN, NOTE_B6, (unsigned long)duration);
    else if (octave == 7) tone(AUDIO_PIN, NOTE_B7, (unsigned long)duration);
    delay(duration);
  }
}

void rest(String seq) {
  noTone(AUDIO_PIN);
  String lengths[seq.length()]; 
  float totalTime = 0;
  for(int i, start, count = 0; i < seq.length(); i++) {
    if(i + 1 >= seq.length()) {
      lengths[count++] = seq.substring(start, seq.length());
    }
    else if(seq[i] == '&') {
      lengths[count++] = seq.substring(start, i);
      start = i + 1;
    }
  }
  for(int i = 0; i < sizeof(lengths) / sizeof(String); i++) {
    int temp = convertToInt(lengths[i]);
    if(temp == 1) {
      totalTime += 1.0f;
    }
    else if (temp == 2) {
      totalTime += 0.5f;
    }
    else if (temp == 4) {
      totalTime += 0.25f;
    }
    else if (temp == 8) {
      totalTime += 0.125f;
    }
    else if (temp == 16) {
      totalTime += 0.0625f;
    }
    else if (temp == 32) {
      totalTime += 0.03125f;
    }
  }
  float duration = (totalTime / (tempo / 60)) * 1000;
  delay(duration);
}

void setTempo(int seq) {
  if(seq > 250 || seq < 50) {
     Serial.println("Invalid tempo command recieved.");
     return;
  } 
  else tempo = seq;
}

void absOctaveChange(int seq) {
  if(seq > 8 || seq < 1) {
     Serial.println("Invalid absolute octave command recieved.");
     return;
  } 
  else octave = seq;
}

void relOctaveChange(String seq) {
  if(seq == ">") {
    if(octave - 1 < 1) { 
      Serial.println("Invalid relative octave command recieved.");
      return; 
      }
    else octave--;
  }
  else if (seq == "<") {
    if(octave + 1 > 8) { 
      Serial.println("Invalid relative octave command recieved.");
      return; 
      }
    else octave++;
  }
}
