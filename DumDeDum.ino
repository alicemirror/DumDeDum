#include <MIDI.h>
#include <HX711.h>

#undef _DEBUG  // Undef for normal working with MIDI output

// Number of weight sensor pads
#define NUM_PADS 5

// Fixed MIDI Channel (the default percussions channel 10)
#define MIDI_CHANNEL 10

// Number of readings every sample
#define NUM_READINGS 1

// Min/Max mapping parameters from sensor data to MIDI velocity
#define MIN_SENSOR_RANGE -4096
#define MAX_SENSOR_RANGE 500000
#define MIN_MIDI_VELOCITY 0
#define MAX_MIDI_VELOCITY 127

/* 
 * Note duration (ms)
 * This value express the fixed duration of a note before the noteOff is sent.
 */
#define NOTE_DURATION 5

#define MIDI_CHANNEL 1   // MIDI predefined channel where notes are sent

// Connection of pads pins
#define PAD_CLK 2     // Clock PIN
#define PAD1 3         // Pad 1 PIN
#define PAD2 4         // Pad 2 PIN
#define PAD3 5         // Pad 3 PIN
#define PAD4 6         // Pad 4 PIN
#define PAD5 7         // Pad 5 PIN

MIDI_CREATE_DEFAULT_INSTANCE();

static const unsigned ledPin = 13;      // LED pin on Arduino Uno

// Define the weight sensor instance for every pad
HX711 pad0(PAD1, PAD_CLK);
HX711 pad1(PAD2, PAD_CLK);
HX711 pad2(PAD3, PAD_CLK);
HX711 pad3(PAD4, PAD_CLK);
HX711 pad4(PAD5, PAD_CLK);

// Create an array of pointers to the the pad instances
HX711 *pads[NUM_PADS];
// Predefined notes mapped to the pads
int padsMap[NUM_PADS];

// Last value read from pad weight sensor
float padRead;

// Initialization
void setup() {
  int j;
  
  // Create the pads vector to manage the weight sensors in loops
  pads[0] = &pad0;
  pads[1] = &pad1;
  pads[2] = &pad2;
  pads[3] = &pad3;
  pads[4] = &pad4;

  // Setup the pitch notes associated to every pad
  padsMap[0] = 38;
  padsMap[1] = 39;
  padsMap[2] = 40;
  padsMap[3] = 41;
  padsMap[4] = 42;

#ifdef _DEBUG
  Serial.begin(38400);
  Serial.println("Intializing sensors");
#endif

  // Initialize the sensor readings and set tare
  for(j = 0; j < 3; j++) {
      // As "gain" value is ommited; the default value 128 is used by the library
      pads[j]->set_scale();
      pads[j]->tare();
  }

#ifdef _DEBUG
  Serial.begin(38400);
  Serial.println("Intialization complete");
#endif

  pinMode(ledPin, OUTPUT);
  MIDI.begin(); 

}

/*
 * Main program loop
 */
void loop() {
  int j;
  int mappedPad;

  // Loop on every pad to see if one has been pressed
  for(j = 0; j < NUM_PADS; j++) {
    padRead = pads[j]->get_units(NUM_READINGS) * -1;
    mappedPad = calcMIDIVelocity(padRead);

    if(mappedPad > 3) {
//      digitalWrite(ledPin, HIGH);
      MIDI.sendNoteOn(padsMap[j], 64, MIDI_CHANNEL); // Send a Note (pitch, velo, channel)
//      delay(NOTE_DURATION);
      MIDI.sendNoteOff(padsMap[j], 0, MIDI_CHANNEL);     // Stop the note
//      digitalWrite(ledPin, LOW);
    }

#ifdef _DEBUG
    Serial.print("\t\t"); 
    Serial.print(mappedPad, 1);
#endif
  }
#ifdef _DEBUG
Serial.println();
#endif
}

/*
 * Map the read sensor value and convert it to MIDI velocity.
 * As sometimes some out of range value occurs, if the mapped
 * value is out of range the function return zero.
 */
int calcMIDIVelocity(float sensor) {
  int velocity;
  
//  velocity = map(padRead, -MIN_SENSOR_RANGE, MAX_SENSOR_RANGE, MIN_MIDI_VELOCITY, MAX_MIDI_VELOCITY);
  return map(padRead, -MIN_SENSOR_RANGE, MAX_SENSOR_RANGE, MIN_MIDI_VELOCITY, MAX_MIDI_VELOCITY);

//  if( (velocity > MAX_MIDI_VELOCITY) || (velocity < MIN_MIDI_VELOCITY) ) {
//    return 0;
//  } else {
//    return velocity;
//  }
}

