#include "pitches.h"

#define AUDIO_PIN 13

String sequence = "";
const int defTempo = 150;
const int defOctave = 4;
int tempo = defTempo;
int octave = defOctave;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(4800);
  Serial.println("Setup complete. Awaiting sequences.");
}

void loop() {
  // put your main code here, to run repeatedly:

    // TODO: convert to vector instead of array
    // Keep track of commands
    String commands[0];
    // Keep track of parameters related to commands
    String inputs[0];
    
  if(Serial.available() > 0) {
    // read next sequence
    sequence = Serial.readString();
    Serial.println(sequence);
    sequence.trim();

    // set reasonable length for commands and inputs
    commands[sequence.length()];
    inputs[sequence.length()];

    // keep track of command & input pointer
    int count = 0;
    // keep track of beginning of input string
    int startInputPointer = 0;
    for(int i = 0; i < sequence.length();) {
      // check for command
      if(isCommand(sequence[i])) {
        // check if command is two characters
        if(hasSharp(sequence[i])) {
          if(sequence[i+1] == '+') {
            commands[count] = sequence[i] + '+';
            i++;
          }
          // does not have two character command
          else commands[count] = sequence[i];
        }
        else {
          // not a note
          commands[count] = sequence[i];
        }
        i++;
        // figure out corresponding input for command
        startInputPointer = i;
        while(!isCommand(sequence[i]) || i < sequence.length()) {
          i++; 
        }
        inputs[count++] = sequence.substring(startInputPointer, i);
      }
      // invalid input, skip
      else i++;
    }

    // execute commands
    for(int i = 0; i < sizeof(commands)/sizeof(String); i++) {
      if(isNote(commands[i])) {
        note(commands[i], inputs[i]);
      }
      else if(commands[i] == "r") {
        rest(inputs[i]);
      }
      else if(commands[i] == "t") {
        setTempo(inputs[i].toInt());
      }
      else if(commands[i] == "o") {
        absOctaveChange(inputs[i].toInt());
      }
      else if(commands[i] == ">" || commands[i] == "<") {
        relOctaveChange(commands[i]);
      }
    }
  }
}

// Checks to see if the input is a note
bool isNote(String input) {
  if(input == "c" || input == "c+" || input == "d" || input == "d+" || 
      input == "e" || input == "f" || input == "f+" || input == "g" ||
      input == "g+" || input == "a" || input == "a+" || input == "b") {
          return true;
  }
  else return false;
  
}

// Checks to see if the note has a sharp note
bool hasSharp(char input) {
  if(input == 'c' || input == 'd' || input == 'f' || input == 'g' ||
      input == 'a') {
          return true;
  }
  else return false;
  
}

bool isCommand(char input) {
  if(input == 'c' || input == 'd' || input == 'e' || input == 'f' ||
      input == 'g' || input == 'a' || input == 'b' || input == 'r' || 
      input == 't' || input == 'o' || input == '<' || input == '>') {
          return true;
  }
  else return false;
}

void note(String note, String seq) {
  char ctemp[seq.length()]; 
  seq.toCharArray(ctemp, seq.length());
  char * lengths = strtok(ctemp, "&");
  float totalTime = 0; 
  int total = sizeof lengths / sizeof lengths[0];
  for(int i = 0; i < total; i++) {
    int temp = convertToInt(lengths);
    lengths = strtok(ctemp, "&");
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
  if(note == "c") {
    tone(AUDIO_PIN, NOTE_C1, duration);
  } 
  else if (note == "c+") {
    tone(AUDIO_PIN, NOTE_CS1, duration);
  }
  else if (note == "d") {
    tone(AUDIO_PIN, NOTE_D1, duration);
  }
  else if (note == "d+") {
    tone(AUDIO_PIN, NOTE_DS1, duration);
  }
  else if (note == "e") {    
    tone(AUDIO_PIN, NOTE_E1, duration);
  } 
  else if (note == "f") {
    tone(AUDIO_PIN, NOTE_F1, duration);
  } 
  else if (note == "f+") {
    tone(AUDIO_PIN, NOTE_FS1, duration);
  } 
  else if (note == "g") {
    tone(AUDIO_PIN, NOTE_G1, duration);
  }
  else if (note == "g+") {
    tone(AUDIO_PIN, NOTE_GS1, duration);
  } 
  else if(note == "a") {
    tone(AUDIO_PIN, NOTE_A1, duration);
  } 
  else if(note == "a+") {
    tone(AUDIO_PIN, NOTE_AS1, duration);
  } 
  else if (note == "b") {
    tone(AUDIO_PIN, NOTE_B1, duration);
  }
}

int convertToInt(char input[]) {
  int retVal = 0;
  int inputSize = sizeof input / sizeof input[0];
  for(int i = 0; i < inputSize; i++) {
    int temp = ((int)input[i] - 30);
    for(int j = inputSize - 1; j > i; j--) {
      temp *= 10;
    }
    retVal += temp;
  }
  return retVal;
}

void rest(String seq) {
  noTone(AUDIO_PIN):
  char ctemp[seq.length()]; 
  seq.toCharArray(ctemp, seq.length());
  char * lengths = strtok(ctemp, "&");
  float totalTime = 0; 
  int total = sizeof lengths / sizeof lengths[0];
  for(int i = 0; i < total; i++) {
    int temp = convertToInt(lengths);
    lengths = strtok(ctemp, "&");
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
  delay((totalTime / (tempo / 60)) * 1000 );
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
  else return;
}

