#include <avr/io.h>
#include <SerialCommand.h>

// No Quartz required version for controlling a AY-3-8910 sound chip with the
// Arduino
// Author: Matio Klingemann http://incubator.quasimondo.com

// Credits:
// This is combination of code found in this tutorial: 
// http://playground.arduino.cc/Main/AY38910
// (which is based on this http://kalshagar.wikispaces.com/Arduino+and+a+YMZ294)
// and code for generating a 2 MHz clock signal found here:
// http://forum.arduino.cc/index.php/topic,62964.0.html

const int BAUD_RATE = 9600;

////Pin connected to Data in (DS) of 74HC595
const int dataPin = 5;
//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = 6;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 7;

const int pinBC1 =  40;     
const int pinBCDIR =  41;

const int resetPin = 48;

// 2mz clock pin
const int freqOutputPin = 11;   // OC2A output pin for ATmega328 boards

const int prescale  = 1;
const int ocr2aval  = 3; 
const float period    = 2.0 * prescale * (ocr2aval+1) / (F_CPU/1.0e6);
const float freq      = 1.0e6 / period;

int tp[] = {//MIDI note number
  15289, 14431, 13621, 12856, 12135, 11454, 10811, 10204,//0-7
  9631, 9091, 8581, 8099, 7645, 7215, 6810, 6428,//8-15
  6067, 5727, 5405, 5102, 4816, 4545, 4290, 4050,//16-23
  3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551,//24-31
  2408, 2273, 2145, 2025, 1911, 1804, 1703, 1607,//32-39
  1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012,//40-47
  956, 902, 851, 804, 758, 716, 676, 638,//48-55
  602, 568, 536, 506, 478, 451, 426, 402,//56-63
  379, 358, 338, 319, 301, 284, 268, 253,//64-71
  239, 225, 213, 201, 190, 179, 169, 159,//72-79
  150, 142, 134, 127, 119, 113, 106, 100,//80-87
  95, 89, 84, 80, 75, 71, 67, 63,//88-95
  60, 56, 53, 50, 47, 45, 42, 40,//96-103
  38, 36, 34, 32, 30, 28, 27, 25,//104-111
  24, 22, 21, 20, 19, 18, 17, 16,//112-119
  15, 14, 13, 13, 12, 11, 11, 10,//120-127
  0//off
};

SerialCommand sc;

void reset()
{
    Serial.print("Resetting chip...");
    digitalWrite(resetPin, LOW);
    delayMicroseconds(5);
    digitalWrite(resetPin, HIGH);
    Serial.println(" Success");
}

void set_amplitude()
{
    Serial.println("Setting amplitude...");
    // Get channel, amplitude and envelope state from Serial
    int channel;
    int amplitude;
    boolean useEnvelope;
    char *arg;
    arg = sc.next();
    if (arg != NULL)
    {
        channel = atoi(arg);
    }
    else
    {
        Serial.println("Error");
        return;
    }
    arg = sc.next();
    if (arg != NULL)
    {
        amplitude = atoi(arg);
    }
    else
    {
        Serial.println("Error");
    }
    // Write data to chip
    if (channel == 1) // Channel A
    {
        set_chA_amplitude(amplitude, false);
    }
    else if (channel == 2) // Channel B
    {
        set_chB_amplitude(amplitude, false);
    }
    else if (channel == 3) // Channel C
    {
        set_chC_amplitude(amplitude, false);
    }
    else
    {
        Serial.println("Error");
        return;
    }
    Serial.println("Success");
}

void set_pitch()
{
    // Get channel and pitch from Serial
    Serial.print("Setting pitch...");
    int channel;
    int pitch;
    char *arg;
    arg = sc.next();
    if (arg != NULL)
    {
        channel = atoi(arg);
    }
    else
    {
        Serial.println("Error");
        return;
    }
    arg = sc.next();
    if (arg != NULL)
    {
        pitch = atoi(arg);
    }
    else
    {
        Serial.println("Error");
        return;
    }
    // Write data to chip
    if (channel == 1) // Channel A
    {
        pitch_chA(pitch);
    }
    else if (channel == 2) // Channel B
    {
        pitch_chB(pitch);
    }
    else if (channel == 3) // Channel C
    {
        pitch_chC(pitch);
    }
    else
    {
        Serial.println("Error");
        return;
    }
    Serial.println(" Success");
}

void volume_slide() {
  for (int i = 15; i >= 0; --i) {
    set_chA_amplitude(i, false);
    set_chB_amplitude(i, false);
    set_chC_amplitude(i, false);
    delay(60);
  }
}

void piano_note()
{
    set_pitch();
    volume_slide();
}

void defaultResponse()
{
    Serial.println("Error: invalid command.");
}

void setup()
{
    // Serial and commands
    Serial.begin(BAUD_RATE);
    sc.addCommand("RESET", reset);
    sc.addCommand("AMP", set_amplitude);
    sc.addCommand("PITCH", set_pitch);
    sc.addCommand("PIANO", piano_note);
    sc.addDefaultHandler(defaultResponse);
    
    //init pins
    //pinMode(latchPin, OUTPUT);
    //pinMode(dataPin, OUTPUT);  
    //pinMode(clockPin, OUTPUT);
    pinMode(pinBC1, OUTPUT);
    pinMode(pinBCDIR, OUTPUT);        
    //pinMode(freqOutputPin, OUTPUT);
    pinMode(resetPin, OUTPUT);
    digitalWrite(resetPin, HIGH);

    // Port A output
    pinMode(22, OUTPUT);
    pinMode(23, OUTPUT);
    pinMode(24, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(26, OUTPUT);
    pinMode(27, OUTPUT);
    pinMode(28, OUTPUT);
    pinMode(29, OUTPUT);
   
    //init2MhzClock();
    set_mix( true, true, true, false, false, false );
    //set_mix( false, false, false, false, false, false );

    Serial.println("Setup complete");
}

void  init2MhzClock()
{
    // Set Timer 2 CTC mode with no prescaling.  OC2A toggles on compare match
    //
    // WGM22:0 = 010: CTC Mode, toggle OC 
    // WGM2 bits 1 and 0 are in TCCR2A,
    // WGM2 bit 2 is in TCCR2B
    // COM2A0 sets OC2A (arduino pin 11 on Uno or Duemilanove) to toggle on compare match
    //
    TCCR2A = ((1 << WGM21) | (1 << COM2A0));

    // Set Timer 2  No prescaling  (i.e. prescale division = 1)
    //
    // CS22:0 = 001: Use CPU clock with no prescaling
    // CS2 bits 2:0 are all in TCCR2B
    TCCR2B = (1 << CS20);

    // Make sure Compare-match register A interrupt for timer2 is disabled
    TIMSK2 = 0;
    // This value determines the output frequency
    OCR2A = ocr2aval;
}

void set_mix( boolean chA_tone,boolean chB_tone,boolean chC_tone,boolean chA_noise,boolean chB_noise,boolean chC_noise )
{
   write_data(7, B11000000 | 
                   (chC_noise == true ? 0 : B00100000)|
                    (chB_noise == true? 0 : B00010000) | 
                    (chA_noise == true ? 0 : B00001000) | 
                    (chC_tone == true ? 0 : B00000100) |
                    (chB_tone == true ? 0 : B00000010) | 
                    (chA_tone == true ? 0 : B00000001) 
   );
}

void set_chA_amplitude(int amplitude, boolean useEnvelope )
{
   write_data(8, (amplitude & 0xf) | (useEnvelope != true ? 0 : B00010000 ) );
}

void set_chB_amplitude(int amplitude, boolean useEnvelope )
{
  write_data(9, (amplitude & 0xf) | (useEnvelope != true ? 0 : B00010000 ) );
}

void set_chC_amplitude(int amplitude, boolean useEnvelope )
{
  write_data(10, (amplitude & 0xf) | (useEnvelope != true ? 0: B00010000 ) );
}

void set_envelope( boolean hold, boolean alternate, boolean attack, boolean cont, unsigned long frequency )
{
    write_data(13, (hold == true ? 0 : 1)|
                    (alternate == true? 0 : 2) | 
                    (attack == true ? 0 : 4) | 
                    (cont == true ? 0 : 8) 
                );
                
    write_data(11,frequency & 0xff );
    write_data(12,(frequency >> 8)& 0xff );
    
}

void piano_chA(int i, int amp, int length_ms, bool slide)
{
  set_chA_amplitude(amp, false);
  write_data(0x00, i & 0xff);
  write_data(0x01, (i >> 8) & 0x0f);
  if (slide) {
    int slide_delay = length_ms / 50;
    for (int i = 0; i < slide_delay; ++i) {
      set_chA_amplitude(--amp, false);
      delay(50);
    }
    delay(length_ms % 50);
  }
  else {
    delay(length_ms);
  }
  set_chA_amplitude(0, false);  
}

void piano_chB(int i, int amp, int length_ms, bool slide)
{
  set_chB_amplitude(amp, false);
  write_data(0x02, i & 0xff);
  write_data(0x03, (i >> 8) & 0x0f);
  if (slide) {
    int slide_delay = length_ms / 50;
    for (int i = 0; i < slide_delay; ++i) {
      set_chB_amplitude(--amp, false);
      delay(50);
    }
    delay(length_ms % 50);
  }
  else {
    delay(length_ms);
  }
  set_chB_amplitude(0, false);
}

void piano_chC(int i, int amp, int length_ms, bool slide)
{
  set_chC_amplitude(amp, false);
  write_data(0x04, i & 0xff);
  write_data(0x05, (i >> 8) & 0x0f);
  if (slide) {
    int slide_delay = length_ms / 50;
    for (int i = 0; i < slide_delay; ++i) {
      set_chC_amplitude(--amp, false);
      delay(50);
    }
    delay(length_ms % 50);
  }
  else {
    delay(length_ms);
  }
  set_chC_amplitude(0, false);
}

void pitch_chA(int i)
{
  write_data(0x00, i & 0xff);
  write_data(0x01, (i >> 8) & 0x0f);    
}

void pitch_chB(int i)
{
  write_data(0x02, i & 0xff);
  write_data(0x03, (i >> 8)&0x0f);
}

void pitch_chC(int i)
{
  write_data(0x04, i & 0xff);
  write_data(0x05, (i >> 8) & 0x0f);
}

void noise(int i)
{
  write_data(0x06, i&0x1f);
}

void mode_latch(){
    digitalWrite(pinBC1, HIGH);
    digitalWrite(pinBCDIR, HIGH);
}

void mode_write(){
    digitalWrite(pinBC1, LOW);
    digitalWrite(pinBCDIR, HIGH);
}

void mode_inactive(){
    digitalWrite(pinBC1, LOW);
    digitalWrite(pinBCDIR, LOW);    
}

void write_data(unsigned char address, unsigned char data)
{  
  mode_inactive();  
  //write address

  digitalWrite(latchPin, LOW);
  // Write the bits on PORTA
  PORTA = address;

  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);

  mode_latch();  
  mode_inactive();

  //write data
  mode_write();  

  digitalWrite(latchPin, LOW);
  // Write the bits on PORTA
  PORTA = data; 

  //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH);

  mode_inactive();  
}

int notePitches[] = {
  4050, // o0
  3822, 3608, 3405, 3214, 3034, 2863, 2703, 2551, 2408, 2273, 2145, 2025, // o1
  1911, 1804, 1703, 1607, 1517, 1432, 1351, 1276, 1204, 1136, 1073, 1012, // o2
  956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506, // o3
  478, 451, 426, 402, 379, 358, 338, 319, 301, 284, 268, 253, // o4
  239, 225, 213, 201, 190, 179, 169, 159, 150, 142, 134, 127, // o5
  119, 113, 106, 100, 95, 89, 84, 80, 75, 71, 67, 63, // o6
  60, 56, 53, 50, 47, 45, 42, 40, 38, 36, 34, 32, // o7
  30 // o8
};

void neat_sequence() {
  set_chA_amplitude(0, false);
  set_chB_amplitude(0, false);
  set_chC_amplitude(0, false);
  int channel = 1;
  int chA_amp, chB_amp, chC_amp = 0;
  int sequence[] = {
    213, 179, 142,
    213, 179, 142,
    159, 134, 106,
    159, 134, 106,
    239, 190, 159,
    239, 190, 159,
    179, 142, 119,
    179, 142, 119,
    268, 213, 179,
    268, 213, 179,
    319, 253, 213,
    319, 253, 213,
    284, 213, 190,
    284, 213, 190,
    284, 225, 190,
    284, 225, 190,

    213, 179, 142,
    213, 179, 142,
    159, 134, 106,
    159, 134, 106,
    239, 190, 159,
    239, 190, 159,
    179, 142, 119,
    179, 142, 119,
    268, 213, 179,
    268, 213, 179,
    319, 253, 213,
    319, 253, 213,
    758, 602, 506,
    379, 301, 253,
    568, 451, 379,
    284, 225, 190,
  };
  for (int i = 0; i < 96 ; ++i) {
    switch (channel) {
      case 1:
        pitch_chA(sequence[i]);
        set_chA_amplitude(15, false);
        chA_amp = 15;
        break;
      case 2:
        pitch_chB(sequence[i]);
        set_chB_amplitude(15, false);
        chB_amp = 15;
        break;
      case 3:
        pitch_chC(sequence[i]);
        set_chC_amplitude(15, false);
        chC_amp = 15;
        break;
    }
    channel++;
    if (channel == 4)
      channel = 1;
    // Decay note volumes
    for (int j = 0; j < 4; j++) {
      if (chA_amp > 0)
        set_chA_amplitude(--chA_amp, false);
      if (chB_amp > 0)
        set_chB_amplitude(--chB_amp, false);
      if (chC_amp > 0)
        set_chC_amplitude(--chC_amp, false);
      delay(30);
    }
  }
}

// Play Jingle Bells on Channel A
void playJingleBells() {
  piano_chA(379, 15, 126, true);
  delay(126);
  piano_chA(379, 15, 126, true);
  delay(126);
  piano_chA(379, 15, 378, true);
  delay(126);
  piano_chA(379, 15, 126, true);
  delay(126);
  piano_chA(379, 15, 126, true);
  delay(126);
  piano_chA(379, 15, 378, true);
  delay(126);
  piano_chA(379, 15, 126, true);
  delay(126);
  piano_chA(319, 15, 126, true);
  delay(126);
  piano_chA(478, 15, 252, true);
  delay(126);
  piano_chA(426, 15, 63, true);
  delay(63);
  piano_chA(379, 15, 504, true);
  delay(504);

  piano_chA(358, 15, 126, true);
  delay(126);
  piano_chA(358, 15, 126, true);
  delay(126);
  piano_chA(358, 15, 252, true);
  delay(126);
  piano_chA(358, 15, 63, true);
  delay(63);
  piano_chA(358, 15, 126, true);
  delay(126);
  piano_chA(379, 15, 126, true);
  delay(126);
  piano_chA(379, 15, 126, true);
  delay(126);
  piano_chA(379, 15, 63, true);
  delay(63);
  piano_chA(379, 15, 63, true);
  delay(63);
  piano_chA(319, 15, 126, true);
  delay(126);
  piano_chA(319, 15, 126, true);
  delay(126);
  piano_chA(358, 15, 126, true);
  delay(126);
  piano_chA(426, 15, 126, true);
  delay(126);
  piano_chA(478, 15, 504, true);
  delay(504);
}

void playChords() {
  pitch_chA(239);
  pitch_chB(190);
  pitch_chC(159);
  volume_slide();
  pitch_chA(239);
  pitch_chB(179);
  pitch_chC(142);
  volume_slide();
  pitch_chA(239);
  pitch_chB(190);
  pitch_chC(159);
  volume_slide();
  pitch_chA(253);
  pitch_chB(179);
  pitch_chC(159);
  volume_slide();
  pitch_chA(239);
  pitch_chB(190);
  pitch_chC(159);
  volume_slide();
  delay(1000);
}

void note_slide() {
  set_chA_amplitude(0, false);
  set_chB_amplitude(0, false);
  set_chC_amplitude(0, false);
  int channel = 1;
  int chA_amp, chB_amp, chC_amp = 0;
  for (int i = 60; i >= 13; --i) {
    switch (channel) {
      case 1:
        pitch_chA(notePitches[i]);
        set_chA_amplitude(15, false);
        chA_amp = 15;
        break;
      case 2:
        pitch_chB(notePitches[i]);
        set_chB_amplitude(15, false);
        chB_amp = 15;
        break;
      case 3:
        pitch_chC(notePitches[i]);
        set_chC_amplitude(15, false);
        chC_amp = 15;
        break;
    }
    channel++;
    if (channel == 4)
      channel = 1;
    // Decay note volumes
    for (int j = 0; j < 4; j++) {
      if (chA_amp > 0)
        set_chA_amplitude(--chA_amp, false);
      if (chB_amp > 0)
        set_chB_amplitude(--chB_amp, false);
      if (chC_amp > 0)
        set_chC_amplitude(--chC_amp, false);
      delay(30);
    }
  }
}

void loop() {
  //sc.readSerial();
  playJingleBells();
  playChords();
  note_slide();
  neat_sequence();
  set_chA_amplitude(0, false);
  set_chB_amplitude(0, false);
  set_chC_amplitude(0, false);
  delay(1000);
  /*
  set_chA_amplitude(15, false);
  for (int i = 25; i < 37; ++i) {
    pitch_chA(notePitches[i]);
    delay(250);
  }
  */
  /*
  set_envelope(false,false,false,false,0);
  if ( random(0,6) == 0 )
  {
    set_chA_amplitude(8,false);
    piano_chA(random(50,66));
  } else if ( random(0,4) == 0 )
  {
     set_chA_amplitude(0,false);
  }
  if ( random(0,6) == 0 )
  {
    set_chB_amplitude(8,false);
    note_chB(random(50,66));
  } else if ( random(0,4) == 0 )
  {
     set_chB_amplitude(0,false);
  if ( random(0,2) == 0 )
  {
    set_chC_amplitude(0,true);
    noise( random(0,0x20) );
  } else  
  {
    set_chC_amplitude(0,false);
  }
  */
}
