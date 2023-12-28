#include "SPI.h"
#include "TFT_eSPI.h"
#include "BluetoothA2DPSink.h"

BluetoothA2DPSink a2dp_sink;

// Use hardware SPI
TFT_eSPI tft = TFT_eSPI();

// Variables for song and artist info
String currentSong = "";
String currentArtist = "";
String currentAlbum = "";
bool isPlaying = false;

unsigned long songStartTime = 0;  // Variable to store the start time of the song
const unsigned long songDuration = 300000; // 5 minutes in milliseconds

bool is_active = false;

int screenWidth, screenHeight;

void avrc_metadata_callback(uint8_t id, const uint8_t* text) {
  Serial.printf("==> AVRC metadata rsp: attribute id 0x%x, %s\n", id, text);

  const char* charText = reinterpret_cast<const char*>(text);
  if (id == 1) {
    if (currentSong != charText) {
      resetProgressBar();
    }
    currentSong = charText;
    tft.fillRect(screenWidth * 0.05, screenHeight * 0.05, screenWidth, screenHeight * 0.1, TFT_WHITE);
  } else if (id == 2) {
    currentArtist = charText;
    tft.fillRect(screenWidth * 0.05, screenHeight * 0.20, screenWidth, screenHeight * 0.05, TFT_WHITE);
  } else if (id == 4) {
    currentAlbum = charText;
    tft.fillRect(screenWidth * 0.05, screenHeight * 0.30, screenWidth, screenHeight * 0.05, TFT_WHITE);
  }

  // Drawing song information using relative positioning
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString(currentSong, screenWidth * 0.05, screenHeight * 0.05);
  tft.setTextSize(1);
  tft.drawString(currentArtist, screenWidth * 0.05, screenHeight * 0.20);
  tft.drawString(currentAlbum, screenWidth * 0.05, screenHeight * 0.30);
}

// Touch coordinates
int16_t x, y;

void setup() {
  tft.begin();
  tft.setRotation(1);  // Set screen rotation

  screenWidth = tft.width();
  screenHeight = tft.height();

  // Initially draw the playback UI
  drawPlaybackUI();
  Serial.begin(115200);

  i2s_pin_config_t my_pin_config = {
    .bck_io_num = 27,
    .ws_io_num = 26,
    .data_out_num = 25,
    .data_in_num = I2S_PIN_NO_CHANGE
  };
  a2dp_sink.set_pin_config(my_pin_config);

  a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
  a2dp_sink.start("MyMusic");
}

void drawPlaybackUI() {
  tft.fillScreen(TFT_WHITE);

  // Drawing song information using relative positioning
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("Song Name", screenWidth * 0.05, screenHeight * 0.05);
  tft.setTextSize(1);
  tft.drawString("Song Artist", screenWidth * 0.05, screenHeight * 0.20);
  tft.drawString("Song Album", screenWidth * 0.05, screenHeight * 0.30);

  // Draw Icons
  drawPlayIcon(screenWidth * 0.25, screenHeight * 0.7, screenWidth * 0.1, screenHeight * 0.1);
  drawPrevIcon(screenWidth * 0.05, screenHeight * 0.7, screenWidth * 0.1, screenHeight * 0.1);
  drawNextIcon(screenWidth * 0.45, screenHeight * 0.7, screenWidth * 0.1, screenHeight * 0.1);
  drawPlusIcon(screenWidth * 0.65, screenHeight * 0.7, screenWidth * 0.1, screenHeight * 0.1);
  drawMinusIcon(screenWidth * 0.85, screenHeight * 0.7, screenWidth * 0.1, screenHeight * 0.1);
}

void drawPlayIcon(int x, int y, int w, int h) {
  tft.fillTriangle(
    x, y, 
    x, y + h, 
    x + w, y + h / 2, 
    TFT_BLACK);
}

void drawPauseIcon(int x, int y, int w, int h) {
  int barWidth = w / 3.5;
  tft.fillRect(x, y, barWidth, h, TFT_BLACK);
  tft.fillRect(x + w - barWidth, y, barWidth, h, TFT_BLACK);
}

void drawPrevIcon(int x, int y, int w, int h) {
  // Draw two left-pointing arrows for 'Previous'
  int arrowWidth = w / 2.5;
  tft.fillTriangle(
    x + arrowWidth * 2, y, 
    x + arrowWidth * 2, y + h, 
    x + arrowWidth, y + h / 2, 
    TFT_BLACK);
  tft.fillTriangle(
    x + arrowWidth, y, 
    x + arrowWidth, y + h, 
    x, y + h / 2, 
    TFT_BLACK);
}

void drawNextIcon(int x, int y, int w, int h) {
  // Draw two right-pointing arrows for 'Next'
  int arrowWidth = w / 2.5;
  tft.fillTriangle(
    x, y, 
    x, y + h, 
    x + arrowWidth, y + h / 2, 
    TFT_BLACK);
  tft.fillTriangle(
    x + arrowWidth, y, 
    x + arrowWidth, y + h, 
    x + arrowWidth * 2, y + h / 2, 
    TFT_BLACK);
}

void drawPlusIcon(int x, int y, int w, int h) {
  int centerX = x + w / 2;
  int centerY = y + h / 2;
  int lineSize = min(w, h) / 3;
  tft.fillRect(x, centerY - lineSize / 2, w, lineSize, TFT_BLACK);  // Horizontal line
  tft.fillRect(centerX - lineSize / 2, y, lineSize, h, TFT_BLACK);  // Vertical line
}

void drawMinusIcon(int x, int y, int w, int h) {
  int centerY = y + h / 2;
  int lineSize = min(w, h) / 3;
  tft.fillRect(x, centerY - lineSize / 2, w, lineSize, TFT_BLACK);  // Horizontal line
}

void loop() {
  uint16_t x, y;

  if (tft.getTouch(&x, &y)) {
    handleTouch(x, y);
    delay(200);  // Simple debouncing
  }
}

void handleTouch(int x, int y) {
  // Play/Pause Button Area
  if (x >= 75 && x <= 100 && y >= 40 && y <= 65) {
    Serial.println("Play/Pause Button Pressed");
    is_active = !is_active;
    if (!is_active) {
      Serial.println("pause");
      a2dp_sink.pause();
      tft.fillRect(screenWidth * 0.20, screenHeight * 0.7, screenWidth * 0.2, screenHeight * 0.11, TFT_WHITE);
      drawPlayIcon(screenWidth * 0.25, screenHeight * 0.7, screenWidth * 0.1, screenHeight * 0.1);
    } else {
      Serial.println("play");
      a2dp_sink.play();
      startSongTimer();
      tft.fillRect(screenWidth * 0.2, screenHeight * 0.7, screenWidth * 0.2, screenHeight * 0.11, TFT_WHITE);
      drawPauseIcon(screenWidth * 0.25, screenHeight * 0.7, screenWidth * 0.08, screenHeight * 0.1);
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
    resetProgressBar();
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
  Serial.print("x: ");
  Serial.println(x);
  Serial.print("y: ");
  Serial.println(y);
}

void adjustVolume(int adjustment) {
  int current_volume = a2dp_sink.get_volume();
  current_volume += adjustment;

  if (current_volume > 100) current_volume = 100;   // Ensure volume doesn't go above 100
  else if (current_volume < 0) current_volume = 0;  // Ensure volume doesn't go below 0

  a2dp_sink.set_volume(current_volume);  // Set new volume
}
