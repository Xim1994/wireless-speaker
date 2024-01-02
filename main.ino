// Include necessary libraries for SPI communication, TFT display, and Bluetooth audio
#include "SPI.h"
#include "TFT_eSPI.h"
#include "BluetoothA2DPSink.h"
#include "UIManager.h"

// Initialize a Bluetooth audio sink for streaming audio data
BluetoothA2DPSink a2dp_sink;

// Initialize the TFT display using hardware SPI
TFT_eSPI tft = TFT_eSPI();
UIManager uiManager(tft);  // Create an instance of UIManager

// Variables to store the current song, artist, and album information
String currentSong = "";
String currentArtist = "";
String currentAlbum = "";

// Variable to indicate if playback is active or paused
bool is_active = false;

// Function to handle updates to the song, artist, or album via AVRCP (Audio/Video Remote Control Profile)
void avrc_metadata_callback(uint8_t id, const uint8_t* text) {
  Serial.printf("==> AVRC metadata rsp: attribute id 0x%x, %s\n", id, text);

  // Update the display with new song, artist, or album information
  // Clear the previous information and draw the new text
  // The id parameter indicates the type of metadata being received
  const char* charText = reinterpret_cast<const char*>(text);
  if (id == 1) {
    currentSong = charText;
  } else if (id == 2) {
    currentArtist = charText;
  } else if (id == 4) {
    currentAlbum = charText;
  }

  is_active = true;
  auto state = a2dp_sink.get_audio_state();
  Serial.println(state);

  // Draw song information
  uiManager.drawSongInfo(currentSong, currentArtist, currentAlbum);
}

// Touch coordinates
int16_t x, y;

void setup() {
  tft.begin();
  tft.setRotation(1);  // Set screen rotation

  // Draw the initial user interface on the display
  uiManager.drawPlaybackUI();

  // Start serial communication for debugging purposes
  Serial.begin(115200);

  // Configure the I2S pins for Bluetooth audio streaming
  i2s_pin_config_t my_pin_config = {
    .bck_io_num = 27, // Bit Clock (BCK) pin
    .ws_io_num = 26,  // Word Select (WS) pin
    .data_out_num = 25, // Data Output pin
    .data_in_num = I2S_PIN_NO_CHANGE  // Not using input
  };
  a2dp_sink.set_pin_config(my_pin_config);

  // Set up the AVRCP metadata callback function to handle changes in track information
  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);

  // Start the Bluetooth sink to begin listening for audio connections
  a2dp_sink.start("MyMusic");
  Serial.println("1");
}

void loop() {
  // Check for touch input on the TFT screen to control playback
  // Handle the touch input and update the display or playback state accordingly
  uint16_t x, y;

  if (tft.getTouch(&x, &y)) {
    handleTouch(x, y);
    delay(200);  // Simple debouncing
  }
}

// Handle touch events on the display and update playback status or volume
void handleTouch(int x, int y) {
  // Determine which UI element was touched and perform the appropriate action
  // such as play/pause, next/previous track, or adjust volume
  if (x >= 75 && x <= 100 && y >= 40 && y <= 65) {
    Serial.println("Play/Pause Button Pressed");
    is_active = !is_active;
    if (!is_active) {
      Serial.println("pause");
      a2dp_sink.pause();
      uiManager.drawPlayIcon();
    } else {
      Serial.println("play");
      a2dp_sink.play();
      uiManager.drawPauseIcon();
    }
  }
  // Previous Button Area
  else if (x >= 10 && x <= 35 && y >= 40 && y <= 65) {
    Serial.println("Previous Button Pressed");
    a2dp_sink.previous();  // Add your code to play the previous song
  }
  // Next Button Area
  else if (x >= 140 && x <= 155 && y >= 40 && y <= 65) {
    Serial.println("Next Button Pressed");
    a2dp_sink.next();  // Add your code to play the next song
  }
  // Volume+ Button Area
  else if (x >= 205 && x <= 225 && y >= 40 && y <= 65) {
    Serial.println("Volume+ Button Pressed");
    adjustVolume(10);  // Increase the volume by 10 units
  }
  // Volume- Button Area
  else if (x >= 265 && x <= 290 && y >= 40 && y <= 65) {
    Serial.println("Volume- Button Pressed");
    adjustVolume(-10);  // Decrease the volume by 10 units
  }
}

// Function to adjust the volume of the playback
void adjustVolume(int adjustment) {
  // Increase or decrease the volume and ensure it remains within valid limits
  int current_volume = a2dp_sink.get_volume();
  current_volume += adjustment;

  if (current_volume > 100) current_volume = 100;   // Ensure volume doesn't go above 100
  else if (current_volume < 0) current_volume = 0;  // Ensure volume doesn't go below 0

  a2dp_sink.set_volume(current_volume);  // Set new volume
}