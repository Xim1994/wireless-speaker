#include "UIManager.h"

UIManager::UIManager(TFT_eSPI &display)
  : tft(display) {
  tft.begin();
  tft.setRotation(1);  // Set screen rotation

  // Obtain and store the screen dimensions from the display
  screenWidth = tft.width();
  screenHeight = tft.height();
}

// Draw the playback UI with song information and control buttons
void UIManager::drawPlaybackUI() {
  tft.fillScreen(TFT_WHITE);  // Clear the screen with a white background

  // Set text color and size then draw the placeholders for song information and control buttons
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString("", screenWidth * 0.05, screenHeight * 0.1);
  tft.setTextSize(1);
  tft.drawString("", screenWidth * 0.05, screenHeight * 0.30);
  tft.drawString("", screenWidth * 0.05, screenHeight * 0.40);

  // Draw Icons
  drawPlayIcon();
  drawPrevIcon();
  drawNextIcon();
  drawPlusIcon();
  drawMinusIcon();
}

void UIManager::drawSongInfo(String song, String artist, String album) {
  //Empty
  tft.fillRect(screenWidth * 0.05, screenHeight * 0.1, screenWidth, screenHeight * 0.1, TFT_WHITE);
  tft.fillRect(screenWidth * 0.05, screenHeight * 0.30, screenWidth, screenHeight * 0.05, TFT_WHITE);
  tft.fillRect(screenWidth * 0.05, screenHeight * 0.40, screenWidth, screenHeight * 0.05, TFT_WHITE);

  // Drawing song information using relative positioning
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTextSize(2);
  tft.drawString(song, screenWidth * 0.05, screenHeight * 0.1);
  tft.setTextSize(1);
  tft.drawString(artist, screenWidth * 0.05, screenHeight * 0.30);
  tft.drawString(album, screenWidth * 0.05, screenHeight * 0.40);
}

void UIManager::drawPlayIcon() {
  tft.fillRect(screenWidth * 0.20, screenHeight * 0.7, screenWidth * 0.2, screenHeight * 0.11, TFT_WHITE);
  int x = screenWidth * 0.25;
  int y = screenHeight * 0.7; 
  int w = screenWidth * 0.1;
  int h = screenHeight * 0.1;
  tft.fillTriangle(x, y, x, y + h, x + w, y + h / 2, TFT_BLACK);
  // Add any additional stylings if needed
}

void UIManager::drawPauseIcon() {
  int x = screenWidth * 0.25;
  int y = screenHeight * 0.7;
  int w = screenWidth * 0.08;
  int h = screenHeight * 0.1;
  tft.fillRect(screenWidth * 0.2, screenHeight * 0.7, screenWidth * 0.2, screenHeight * 0.11, TFT_WHITE);
  int barWidth = w / 3.5;
  tft.fillRect(x, y, barWidth, h, TFT_BLACK);
  tft.fillRect(x + w - barWidth, y, barWidth, h, TFT_BLACK);
}

void UIManager::drawPrevIcon() {
  // Draw two left-pointing arrows for 'Previous'
  int x = screenWidth * 0.05;
  int y = screenHeight * 0.7;
  int w = screenWidth * 0.1;
  int h = screenHeight * 0.1;
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

void UIManager::drawNextIcon() {
  // Draw two right-pointing arrows for 'Next'
  int x = screenWidth * 0.45;
  int y = screenHeight * 0.7;
  int w = screenWidth * 0.1;
  int h = screenHeight * 0.1;
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

void UIManager::drawPlusIcon() {
  int x = screenWidth * 0.65;
  int y = screenHeight * 0.7;
  int w =  screenWidth * 0.1;
  int h = screenHeight * 0.1;
  int centerX = x + w / 2;
  int centerY = y + h / 2;
  int lineSize = min(w, h) / 3;
  tft.fillRect(x, centerY - lineSize / 2, w, lineSize, TFT_BLACK);  // Horizontal line
  tft.fillRect(centerX - lineSize / 2, y, lineSize, h, TFT_BLACK);  // Vertical line
}

void UIManager::drawMinusIcon() {
  int x = screenWidth * 0.85;
  int y = screenHeight * 0.7;
  int w = screenWidth * 0.1;
  int h = screenHeight * 0.1;
  int centerY = y + h / 2;
  int lineSize = min(w, h) / 3;
  tft.fillRect(x, centerY - lineSize / 2, w, lineSize, TFT_BLACK);  // Horizontal line
}
