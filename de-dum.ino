#include "pitches.h"

int currentTrack = 0;
const int numOfTracks = 4;
const int numOfSlots = 8;
int currentSlot = 0;
int currentPlaySlot = 0;
int compositionState[numOfTracks][numOfSlots] = {
  { 1, 0, 0, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 1, 1, 0, 0, 1 },
  { 0, 0, 1, 0, 0, 1, 0, 0 },
  { 1, 0, 1, 0, 1, 0, 1, 0 }
};

// pins
int trackScrollPin = 8;
int instrumentPin = 7;
int modeSelectPin = 2;
int slotLightGreenPin = 6;
int slotLightBluePin = 5;
int slotTrackRedPin = 10;
int slotTrackGreenPin = 9;
int slotTrackBluePin = 3;
int passiveBuzzerOne = 13;
int passiveBuzzerTwo = 11;
int activeBuzzerOne = 12;
int tempoLedPin = 4;
char changeTempoPin = A0; 

// time
unsigned long time;
unsigned long pastTime;
long oneSecond = 1000L;
long twoSeconds = oneSecond * 2;
long halfSecond = oneSecond / 2L;
long quarterSecond = oneSecond / 4L;
long eigthSecond = oneSecond / 8L;
long sixteenthSecond = oneSecond / 16L;
long tempo;
long timeCount = 0L;
long tempos[6] = { sixteenthSecond, eigthSecond, quarterSecond, halfSecond, oneSecond, twoSeconds };
int noteDuration = 100;
int activeBuzzerCount = noteDuration;
int tempoIndex = 3;
long tempoLedCount = 0L;

void setup() {
  Serial.begin(9600);
  pinMode(instrumentPin, INPUT_PULLUP);
  pinMode(trackScrollPin, INPUT_PULLUP);
  pinMode(modeSelectPin, INPUT_PULLUP);
  pinMode(changeTempoPin, INPUT_PULLUP);
  pinMode(slotLightGreenPin, OUTPUT);
  pinMode(slotLightBluePin, OUTPUT);
  pinMode(slotTrackRedPin, OUTPUT);
  pinMode(slotTrackGreenPin, OUTPUT);
  pinMode(slotTrackBluePin, OUTPUT);
  pinMode(passiveBuzzerOne, OUTPUT);
  pinMode(passiveBuzzerTwo, OUTPUT);
  pinMode(activeBuzzerOne, OUTPUT);
  pinMode(tempoLedPin, OUTPUT);
}

void loop() {
  time = millis();
  boolean isNewTime = time != pastTime;

  if (isNewTime) {
    boolean isListenForUpdates = (time % oneSecond) == 0;
  
    if (isListenForUpdates) {
      boolean isInstrumentLow = digitalRead(instrumentPin) == LOW;
      currentSlot = (currentSlot + 1) % numOfSlots;
      
      if (digitalRead(trackScrollPin) == LOW) {
        currentTrack = (currentTrack + 1) % numOfTracks;
      }
  
      if (isInstrumentLow) {
        compositionState[currentTrack][currentSlot] = 1;
      }

      if (digitalRead(changeTempoPin) == LOW) {
        if (isInstrumentLow) {
          resetState();
        } else {
          printState();
          tempoIndex = (tempoIndex + 1) % sizeof(tempos);
        }
      }
    }

    showTrackLights();
    manageSlots();
    pastTime = time;
    playSong();
  }
}

void playSong() {
  tempo = tempos[tempoIndex];
  
  if (timeCount <= 0) {
    digitalWrite(tempoLedPin, HIGH);

    if (digitalRead(modeSelectPin) == LOW) {
      if (compositionState[0][currentPlaySlot] == 1) {
          tone(passiveBuzzerOne, NOTE_B5, noteDuration);
      }
      if (compositionState[1][currentPlaySlot] == 1) {
          tone(passiveBuzzerTwo, NOTE_AS6, noteDuration);
      }
      if (compositionState[2][currentPlaySlot] == 1) {
        digitalWrite(activeBuzzerOne, HIGH);
      } 
    }

    timeCount = tempo;
    currentPlaySlot = (currentPlaySlot + 1) % numOfSlots;
  }

  if (activeBuzzerCount <= 0) {
    digitalWrite(activeBuzzerOne, LOW);
    activeBuzzerCount = noteDuration;
  }

  if (tempoLedCount <= 0) {
    digitalWrite(tempoLedPin, LOW);
    tempoLedCount = noteDuration / 4;
  }
  
  timeCount = timeCount - 1;
  activeBuzzerCount = activeBuzzerCount - 1;
  tempoLedCount = tempoLedCount - 1;
}

void showTrackLights() {
  if (currentTrack == 0) {
    updateTrackLights(LOW, LOW, HIGH);
  } else if (currentTrack == 1) {
    updateTrackLights(HIGH, LOW, LOW);
  } else if (currentTrack == 2) {
    updateTrackLights(LOW, HIGH, LOW);
  } else {
    updateTrackLights(HIGH, HIGH, HIGH);
  }
}

void manageSlots() {
  boolean isShowLight = (time % oneSecond) < quarterSecond;

  if (isShowLight && (currentSlot + 1) == numOfSlots) {
    updateSlotLights(HIGH, LOW);
  } else if (isShowLight) {
    updateSlotLights(LOW, HIGH);
  } else {
    updateSlotLights(LOW, LOW);
  }
}

void updateTrackLights(int red, int green, int blue) {
  digitalWrite(slotTrackRedPin, red);
  digitalWrite(slotTrackGreenPin, green);
  digitalWrite(slotTrackBluePin, blue);
}

void updateSlotLights(int green, int blue) {
  digitalWrite(slotLightGreenPin, green);
  digitalWrite(slotLightBluePin, blue);
}

void printState() {
  for (int i = 0; i < numOfTracks; i++) {
    Serial.print(i);
    Serial.print(": ");
    for (int j = 0; j < numOfSlots; j++) {
      Serial.print(compositionState[i][j]);
      Serial.print(", ");
    }
    Serial.println("");
  }
}

void resetState() {
  for (int i = 0; i < numOfTracks; i++) {
    for (int j = 0; j < numOfSlots; j++) {
      compositionState[i][j] = 0;
    }
  }
}


