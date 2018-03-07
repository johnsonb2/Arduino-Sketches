#include "pitches.h"

#define AUDIO_PIN 13
#define BAUD_RATE 4800

String sequence = "";
const int defTempo = 150;
const int defOctave = 4;
int tempo = defTempo;
int octave = defOctave;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(BAUD_RATE);
  Serial.println("Setup complete. Awaiting sequences.");
}

void loop() {
  // put your main code here, to run repeatedly:

    // TODO: convert to vector instead of array
    
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
    Serial.println("Starting check...");
    for(int i = 0; i < sequence.length();) {
      // check for command
      Serial.println("Checking if command...");
      if(isCommand(sequence[i])) {
        Serial.println("is command...");
        // check if command is two characters
        if(isNote(sequence[i]) && hasSharp(sequence[i])) {
          Serial.println("can have a sharp version of this note...");
          if(sequence[i+1] == '+') {
            commands[count] = sequence.substring(i, i+2);
            i++;
          }
          // does not have two character command
          else {
            commands[count] = sequence[i];
            Serial.println("there is no sharp version of this command...");
          }
        }
        else {
          // not a note
          commands[count] = sequence[i];
          Serial.println("this is not a note...");
        }
        i++;
        // figure out corresponding input for command
        Serial.println("checking for command...");
        startInputPointer = i;
        while(!isCommand(sequence[i]) && i < sequence.length()) {
          i++; 
        }
        inputs[count] = sequence.substring(startInputPointer, i);
        Serial.println(commands[count] + inputs[count]);
        count++;
      }
      // invalid input, skip
      else i++;
    }

    // execute commands
    for(int i = 0; i < sizeof(commands)/sizeof(String); i++) {
      if(isNote(commands[i][0])) {
        Serial.println("Note: " + commands[i] + inputs[i]);
      }
      else if(commands[i][0] == 'r') {
        Serial.println("rest: " + commands[i] + inputs[i]);
      }
      else if(commands[i][0] == 't') {
        Serial.println("tempo: " + commands[i] + inputs[i]);
      }
      else if(commands[i][0] == 'o') {
        Serial.println("absOctaveChange: " + commands[i] + inputs[i]);
      }
      else if(commands[i][0] == '>' || commands[i][0] == '<') {
        Serial.println("relOctaveChange: " + commands[i] + inputs[i]);
      }
      else Serial.println("invalid input: " + commands[i] + inputs[i]);
    }
    Serial.println("Pass complete");
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

