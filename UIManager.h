#ifndef UIManager_h
#define UIManager_h

#include "TFT_eSPI.h" // Include the TFT library

class UIManager {
  public:
    UIManager(TFT_eSPI &tft); // Constructor that takes the display as an input
    void drawPlaybackUI();
    void drawPlayIcon();
    void drawPauseIcon();
    void drawPrevIcon();
    void drawNextIcon();
    void drawPlusIcon();
    void drawMinusIcon();
    void drawSongInfo(String song, String artist, String album);

  private:
    TFT_eSPI &tft; // Reference to the TFT display
    int screenWidth, screenHeight; // Could be parameters or managed separately
};

#endif
