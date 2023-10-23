// BN: Arduino Micro
#include <SD.h>
#include <TMRpcm.h>
#include <Wire.h>
#include <RTClib.h>

#define SD_ChipSelectPin 10  // SD card CS pin
TMRpcm audio;
RTC_DS1307 rtc;             // Create an RTC object

int lastPlayed = -1;    // Variable to store the last time the sound was played

struct HourSound {
  int hourToBePlayed;
  const char* soundFileName; // name length limit is like 8
};

// Sounds to play
HourSound hourSounds[] = {
  {0, "midnight.wav"},
  {1, "oneam.wav"},
  {2, "twoam.wav"},
};

void setup(){
  Serial.begin(9600);
  // debugging 
//  while (!Serial) {
//    ;
//  }

  Wire.begin();
  rtc.begin();

  // Initialize the SD card
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD card initialization failed!");
    return;
  }

  // Print all files on the SD card
  File root = SD.open("/");
  printDirectory(root, 0);
  root.close();

  Serial.println("we good");
  
  audio.speakerPin = 9;     // Set the speaker pin to 9
  audio.setVolume(5);       // volume (0 to 7) leave at 5


//  audio.play("ngale.wav");
}

void loop(){
  DateTime now = rtc.now(); // Get the current time

  // Print the current time to the Serial Monitor
  Serial.print("Current Time: ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  // Check if an hour has passed since the last time the sound was played
  if (now.hour() != lastPlayed) {
    lastPlayed = now.hour();
    Serial.println(lastPlayed);

    // Play sound for this hour
    for (int i = 0; i < sizeof(hourSounds)-1; i++) {
      if (hourSounds[i].hourToBePlayed == now.hour()) {
        audio.play(hourSounds[i].soundFileName);
        break;
      }
    }
    
//    audio.play("bluetit.wav"); 
  }

  delay(1000);
}

// Function to print files in a directory
void printDirectory(File dir, int numTabs) {
  while (true) {
    File entry = dir.openNextFile();
    if (!entry) {
      // No more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // Print file size
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}
