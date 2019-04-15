unsigned long testPixel() {
  tft.fillWindow();
  unsigned long start = micros();
  tft.drawPixel(0, 0, COLOR_WHITE);
  return micros() - start;
}

unsigned long testPixels() {
  int green = 0;
  tft.fillWindow();
  unsigned long start = micros();
  for (uint16_t i = 0; i < tft.width(); i++) {
    if (green > 255) green = 0;
    tft.drawPixel(i, 0, tft.Color565(0, green, 0));
    green++;
  }
  return micros() - start;
}

unsigned long testFillScreen() {
  tft.fillWindow();
  unsigned long start = micros();
  tft.fillWindow(COLOR_RED);
  return micros() - start;
}

unsigned long testText() {
  tft.clearScreen();
  tft.setTextColor(COLOR_WHITE);
  tft.setFontScale(0);
  unsigned long start = micros();
  tft.setTextColor(COLOR_YELLOW);
  tft.setFontScale(1);
  tft.println(1234.56);
  tft.setTextColor(COLOR_RED);
  tft.println(0xFFFF, HEX);
  tft.println();
  tft.setTextColor(COLOR_GREEN);
  tft.setFontScale(3);
  tft.println("Hello");
  tft.setFontScale(0);
  tft.println("I implore thee, my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles, Or I will rend thee");
  tft.println("in the gobberwartswith my blurglecruncheon,");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  unsigned long result = micros() - start;
  return result;
}

unsigned long testLines(uint16_t color) {
  unsigned long start = 0;
  unsigned long t = 0;
  uint16_t i;

  tft.fillWindow();

  start = micros();
  for (i = 0; i < tft.width(); i += 6) tft.drawLine(0, 0, i, tft.height() - 1, color);
  for (i = 0; i < tft.height(); i += 6) tft.drawLine(0, 0, tft.width() - 1, i, color);
  t = micros() - start;//exclude clear screen from count
  tft.fillWindow();
  start = micros();
  for (i = 0; i < tft.width(); i += 6) tft.drawLine(tft.width() - 1, 0, i, tft.height() - 1, color);
  for (i = 0; i < tft.height(); i += 6) tft.drawLine(tft.width() - 1, 0, 0, i, color);
  t += micros() - start;
  tft.fillWindow();
  start = micros();
  for (i = 0; i < tft.width(); i += 6) tft.drawLine(0, tft.height() - 1, i, 0, color);
  for (i = 0; i < tft.height(); i += 6) tft.drawLine(0, tft.height() - 1, tft.width() - 1, i, color);
  t += micros() - start;
  tft.fillWindow();
  start = micros();
  for (i = 0; i < tft.width(); i += 6) tft.drawLine(tft.width() - 1, tft.height() - 1, i, 0, color);
  for (i = 0; i < tft.height(); i += 6) tft.drawLine(tft.width() - 1, tft.height() - 1, 0, i, color);
  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  uint16_t i;
  tft.clearScreen();
  start = micros();
  for (i = 0; i < tft.height(); i += 5) tft.drawFastHLine(0, i, tft.width() - 1, color1);
  for (i = 0; i < tft.width(); i += 5) tft.drawFastVLine(i, 0, tft.height() - 1, color2);
  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  uint16_t i;
  uint16_t i2;
  uint16_t cx = tft.width() / 2;
  uint16_t cy = tft.height() / 2;
  uint16_t n = min(tft.width(), tft.height());
  tft.fillWindow();
  start = micros();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color);
  }
  return micros() - start;
}


unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start;
  unsigned long t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;
  tft.fillWindow();
  n     = min(tft.width(), tft.height());
  start = micros();
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.fillRect(cx - i2, cy - i2, i, i, color1);
  }
  t = micros() - start;
  for (i = 2; i < n; i += 6) {
    i2 = i / 2;
    tft.drawRect(cx - i2, cy - i2, i, i, color2);
  }
  return t;
}


unsigned long testFilledCircles(uint8_t radius, uint16_t color1, uint16_t color2) {
  unsigned long start;
  //unsigned long t = 0;
  uint16_t x;
  uint16_t y;
  uint16_t r2 = radius * 2;
  tft.fillWindow();
  start = micros();
  for (x = radius; x < tft.width(); x += r2) {
    for (y = radius; y < tft.height(); y += r2) {
      tft.fillCircle(x, y, radius, color1);
      //t    += micros() - start;
      //tft.drawCircle(x, y, radius, color2);
    }
  }
  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  uint16_t x;
  uint16_t y;
  uint16_t r2 = radius * 2;
  start = micros();
  for (x = 0; x < (tft.width() - radius); x += r2) {
    for (y = 0; y < (tft.height() - radius); y += r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
  return micros() - start;
}

unsigned long testTriangles(uint16_t color) {
  unsigned long start;
  uint16_t i;
  uint16_t cx = (tft.width() / 2) - 1;
  uint16_t cy = (tft.height() / 2) - 1;
  uint16_t n = min(cx, cy);
  tft.fillWindow();
  start = micros();
  for (i = 0; i < n; i += 5) {
    // peak,bottom left,bottom right
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color);
  }
  return micros() - start;
}

unsigned long testFilledTriangles(uint16_t color1, uint16_t color2) {
  unsigned long start;
  unsigned long t = 0;
  uint16_t i;
  uint16_t cx = (tft.width() / 2) - 1;
  uint16_t cy = (tft.height() / 2) - 1;
  uint16_t n = min(cx, cy);
  tft.fillWindow();
  start = micros();
  for (i = n; i > 10; i -= 5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color1);
    //tft.Color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color2);
    //tft.Color565(i, i, 0));
  }
  return t;
}



unsigned long testFilledRoundRects() {
  unsigned long start;
  uint16_t i, d;
  tft.fillWindow();
  d = tft.height();
  if (d > tft.width()) d = tft.width();
  d--;
  start = micros();
  for (i = d; i >= 10; i--) {
    tft.fillRoundRect(tft.width() / 2 - (i / 2), tft.height() / 2 - (i / 2), i, i, i / 4, tft.htmlTo565(0xFF0000 + i));
  }
  return micros() - start;
}

unsigned long testRoundRects() {
  unsigned long start;
  uint16_t i, d;
  tft.fillWindow();
  d = tft.height();
  if (d > tft.width()) d = tft.width();
  d--;
  start = micros();
  for (i = d; i >= 10; i--) {
    tft.drawRoundRect(tft.width() / 2 - (i / 2), tft.height() / 2 - (i / 2), i, i, i / 4, random(0xFFFF));
  }
  return micros() - start;
}

void test(uint8_t rot) {
  unsigned long i;
  
  tft.clearScreen();
  tft.setRotation(rot);
  tft.setCursor(CENTER,CENTER);
  tft.setTextColor(COLOR_WHITE);
  tft.setFontScale(1);
  tft.print("Rotation:");
  tft.print(rot);
  tft.setFontScale(2);
  tft.setCursor(0,0);
  delay(1000);
  Sprint(F("screen:"));
  Sprint(tft.width());
  Sprint("x");
  Sprint(tft.height());
  Sprint(" - rotation:");
  Sprint(rot);
  Sprintln(F("\nBenchmark                Time (microseconds)"));
  Sprint(F("Screen fill              "));
  i = testFillScreen();
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Test Pixel               "));
  i = testPixel();
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Test Pixels              "));
  i = testPixels();
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Text                     "));
  i = testText();
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Lines                    "));
  i = testLines(COLOR_CYAN);
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Horiz/Vert Lines         "));
  i = testFastLines(COLOR_RED, COLOR_BLUE);
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Rectangles (outline)     "));
  i = testRects(COLOR_GREEN);
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Rectangles (filled)      "));
  i = testFilledRects(COLOR_YELLOW, COLOR_MAGENTA);
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Circles (filled)         "));
  i = testFilledCircles(10, COLOR_MAGENTA, COLOR_YELLOW);
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Circles (outline)        "));
  i = testCircles(10, COLOR_WHITE);
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Triangles (outline)      "));
  i = testTriangles(COLOR_CYAN);
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Triangles (filled)       "));
  i = testFilledTriangles(COLOR_RED, COLOR_CYAN);
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Rounded rects (outline)  "));
  i = testRoundRects();
  Sprintln(i);
  delay(DELAY_BETWEEN);

  Sprint(F("Rounded rects (filled)   "));
  i = testFilledRoundRects();
  Sprintln(i);
  delay(DELAY_BETWEEN);


  Sprintln(F("--------------------------------\n"));
}
