#include <SPI.h>
#include <Gamebuino.h>

Gamebuino gb;

#define SIZE_X (200)
#define SIZE_Y (100)
#define LIMIT_X (195)
#define LIMIT_Y (93)

#define TIMESUP 100
#define BONESNO 10

extern const byte font3x5[];

// dog
int d_posx, d_posy;
boolean have_bone, game_over;
int start_time;
int bones_taken;
int b_posx, b_posy;
int k_posx[BONESNO];
int k_posy[BONESNO];

#define GNAME F("Dogs&Bones")

void setup() {
  gb.begin();
  restartGame();
}

// the loop routine runs over and over again forever
void loop() {
  if (gb.update()) {
    keyboardPressed();
    displayView();
  }
}

void doLogic() {
  // check if we have no bone
  if (!have_bone) {
    // check all bones
    for (int i = 0; i < BONESNO; i++) {
      if (gb.collideRectRect(d_posx, d_posy, 3, 5, k_posx[i], k_posy[i], 3, 5)) {
        have_bone = true;
        k_posx[i] = -10;
        k_posy[i] = -10;
        break;
      }
    }
  } else {
    if (gb.collideRectRect(d_posx, d_posy, 3, 5, b_posx, b_posy, 3, 5)) {
      have_bone = false;
      bones_taken++;
    }
  }

}

void displayView() {
  if (game_over) {
    gb.display.fillScreen(BLACK);
    gb.display.setColor(WHITE, BLACK);
    gb.display.cursorX = 7;
    gb.display.cursorY = 10;
    gb.display.fontSize = 2;
    gb.display.print("Game Over");
  } else {

    // display time and points, etc...
    gb.display.fillRect(63, 0, 20, 48);
    gb.display.setColor(WHITE, BLACK);

    for (int i = 0; i < 48; i += 2) gb.display.drawPixel(63, i);

    int time = TIMESUP - ((millis() - start_time) / 1000);
    if (time < 0) game_over = true;

    gb.display.cursorX = 67;
    gb.display.cursorY = 2;
    if (time < 10) {
      gb.display.cursorX += 8;
    }
    gb.display.fontSize = 2;
    gb.display.print(time);

    // bones taken
    gb.display.cursorX = 75;
    gb.display.cursorY = 14;
    gb.display.print(bones_taken);
    if ( !have_bone || (millis() % 1000) < 500) gb.display.drawChar(67, 14, 18, 2);

    // line
    gb.display.drawLine(66, 26, 81, 26);

    gb.display.setColor(BLACK, WHITE);
    gb.display.fontSize = 1;

    // calculate current display view
    int lx = d_posx - 31;
    if (lx < 0) lx = 0;
    int ly = d_posy - 24;
    if (ly < 0) ly = 0;
    if ((d_posx + 30) > LIMIT_X + 4) {
      lx = LIMIT_X - 57;
    }
    if ((d_posy + 23) > LIMIT_Y + 6) {
      ly = LIMIT_Y - 41;
    }

    if (lx == 0) gb.display.drawLine(0, 0, 0, 47);
    if (ly == 0) gb.display.drawLine(0, 0, 61, 0);
    if (lx == LIMIT_X - 57) gb.display.drawLine(61, 0, 61, 47);
    if (ly == LIMIT_Y - 41) gb.display.drawLine(0, 47, 61, 47);

    // grass
    for (int x = lx; x < lx + 62; x++) {
      boolean dox = (x % 6 == 3);
      int shift = (x % 12 == 3) ? 0 : 2;
      for (int y = ly; y < ly + 48; y++) {
        if ( (dox) && (( (shift + y) % 6) == 3))
          gb.display.drawPixel(x - lx, y - ly);
      }
    }

    // dog house
    if (inside(lx, ly, b_posx, b_posy)) {
      gb.display.drawChar(b_posx - lx, b_posy - ly, '#', 1);
    }

    // bones
    for (int i = 0; i < BONESNO; i++) {
      if (inside(lx, ly, k_posx[i], k_posy[i])) {
        gb.display.drawChar(k_posx[i] - lx, k_posy[i] - ly, 18, 1);
      }
    }

    // dog
    gb.display.drawChar(d_posx - lx, d_posy - ly, '@', 1);
  }
}

boolean inside(int lx, int ly, int posx, int posy) {
  int t = posx - lx;
  if (t < 0 || t > 58) return false;
  t = posy - ly;
  if (t < 0 || t > 47) return false;
  return true;
}

void gameInitialize() {
  b_posx = random(10, LIMIT_X - 10);
  b_posy = random(10, LIMIT_Y - 10);
  d_posx = b_posx - 6;
  d_posy = b_posy;

  int y = 5;
  for (int i = 0; i < BONESNO; i++) {
    k_posx[i] = random(10, LIMIT_X - 10);
    k_posy[i] = y + 5;
    y += 10;
  }

  bones_taken = 0;
  have_bone = false;
  game_over = false;

  start_time = millis();
}

#define FR 1

void restartGame() {
  gb.titleScreen(GNAME);
  gb.battery.show = false;
  gb.display.persistence = false;
  gb.display.setFont(font3x5);
  gb.pickRandomSeed();
  gameInitialize();
}

void keyboardPressed() {
  if (gb.buttons.pressed(BTN_C) || (game_over && gb.buttons.pressed(BTN_A))) {
    restartGame();
  } else {
    if (gb.buttons.repeat(BTN_UP, FR)) {
      d_posy--;
      if (d_posy < 1) d_posy = 1;
    }

    if (gb.buttons.repeat(BTN_DOWN, FR)) {
      d_posy++;
      if (d_posy > LIMIT_Y) d_posy = LIMIT_Y;
    }

    if (gb.buttons.repeat(BTN_LEFT, FR)) {
      d_posx--;
      if (d_posx < 1) d_posx = 1;
    }

    if (gb.buttons.repeat(BTN_RIGHT, FR)) {
      d_posx++;
      if (d_posx > LIMIT_X) d_posx = LIMIT_X;
    }

    if (gb.buttons.pressed(BTN_A)) {
      doLogic();
    }
  }
}

