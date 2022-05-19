
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LedControl.h>

#define JOY_X 1
#define JOY_Y 2
#define JOY_SW 2

int start_button  = 6;
int quit_button  = 7;

int fullrow = 0;
bool isfull = false;

#define BLOCK_COUNT (sizeof(blocks) / sizeof(blocks[0]))

LedControl lc = LedControl(12, 11, 10, 1);
LiquidCrystal_I2C lcd(0x27, 16, 2);


struct point {
  int x;
  int y;
};


static int blocks[][4][8] = {
  {
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 2, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0}
  },
  {
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 2, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0}
  },
  {
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 2, 1, 0, 0, 0},
  },
  {
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
  },
  {
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 2, 0, 0, 0},
  },
  {
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 2, 1, 0, 0, 0},
  },
  {
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 2, 1, 0, 0},
  },
};

void(* resetFunc) (void) = 0; // pentru resetarea jocului

static int moving[12][8]; // pentru mutarea piesei
static int line[8][8]; // pentru retinerea randurilor cu piese

static unsigned long lastUpdate;
static const unsigned long updateInterval = 1000;

static unsigned long lastInput;
static const unsigned long inputDelay = 120;

static int score;
static int level;
int started = 0;

void setup() {
  pinMode(start_button, INPUT_PULLUP);
  pinMode(quit_button, INPUT_PULLUP);
  
  lc.shutdown(0, false);
  lc.setIntensity(0, 5);
  lc.clearDisplay(0);

  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(JOY_SW, INPUT);

  randomSeed(analogRead(0));
  queueNewBlock(); // randomizarea pieselor

  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

  lcd.setCursor(3, 0);
  lcd.print("--Tetris--");
  lcd.setCursor(1, 1);
  lcd.print("S  R");
  
}

void loop() {
  //Serial.println("Intrare in loop");
  if(digitalRead(quit_button) == LOW){ // butonul de reset joc
      started = 0;
      lc.clearDisplay(0);
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("--Tetris--");
      lcd.setCursor(1, 1);
      lcd.print("S  R");
      resetFunc(); // functie ce reseteaza matricea si scorul
  }

  if(started == 0){
    if(digitalRead(start_button) == LOW){ // butonul de start joc
             started = 1;
             lcd.clear();
             lcd.setCursor(1, 0);
             lcd.print("--Start Game--");
             delay(2000);
    }
  }
   
   if(started == 1){ // desfasurarea jocului

      for(int i = 0; i < 8; i++){ // in cazul in care o linie este plina
        for(int j = 0; j < 8; j++){ // matricea se reseteaza, fiecare led devenind inactiv
          if(line[i][j] != 0)
            fullrow++;
        }
        if(fullrow == 8){
          isfull = true;
          Serial.print("rand plin1");
        }
        fullrow = 0;
      }

      if (isfull){
         Serial.print("rand plin2");
         for(int i = 0; i < 8; i++)
          for(int j = 0; j < 8; j++)
            line[i][j] = 0;
            
         isfull = false;
      }
    
      if (isGameOver()) { // in cazul in care nu se mai pot adauga piese
        //Serial.println("Aici"); // matreicea se reseteaza
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("--Game Over!--");
          lcd.setCursor(0, 1);
          lcd.print("Score: ");
          lcd.print(score);
          for (int t = 0; t < 3; t++) {
            for (int i = 0; i < 8; i++) {
              for (int j = 0; j < 8; j++)
                lc.setLed(0, i, j, true);
        
            }
           delay(500);
           render();
           delay(500);
        }
        delay(3000);
        resetFunc();
    }
    
    unsigned long currentMillis = millis();
    
    if (currentMillis - lastInput > inputDelay) {
      handleInput();
      lastInput = currentMillis;
    }
    
    if (currentMillis - lastUpdate > updateInterval) {
      updateState();
      printLcd();
      lastUpdate = currentMillis;
    }
    render();
 }
}


void handleInput() { // folosirea joystickului
  //Serial.println("Metoda handleInput");
  if (analogRead(JOY_X) < 150) { // deplasarea la dreapta
      transformMoving(1, 0);
  } else if (analogRead(JOY_X) > 850) { // deplasarea la stanga
      transformMoving(-1, 0);
  } else if (analogRead(JOY_Y) > 850) { // deplasarea in jos cu viteza
      lastUpdate -= updateInterval;
      score++;
  } else if (analogRead(JOY_Y) < 150) { // rotirea piesei
      rotate90();
  }
}

void updateState() { // update joc
  //Serial.println("Metoda updateState");
  if (isMovingAtBottom()) {
    handleAtBottom();
    queueNewBlock();
  }
  transformMoving(0, -1);
  updateRows();
}

void render() { // 
  //Serial.println("Metoda render");
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) 
      lc.setLed(0, i, j, moving[i + 4][j] | line[i][j]);
  }
}

void printLcd() { //afisare lcd
  //Serial.println("Metoda printLcd");
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Level: ");
  lcd.print(level);

  lcd.setCursor(1, 1);
  lcd.print("Score: ");
  lcd.print(score);
 }

void updateRows() { // calcularea scorului 
  //Serial.println("Metoda updateRows");
  int rowsUpdated = 0;

  for (int i = 0; i < 8; i++) {
    int validRow = 1;
    for (int j = 0; j < 8; j++) {
      if (!line[i][j])
        validRow = 0;
    }
    validRow = 0;

    if (validRow)
      rowsUpdated++;
      
  }

  switch (rowsUpdated) { // actualizarea scorului
    case 1:
      level = 1;
      score += 10 * level;
      //Serial.print("aici1");
      break;
    case 2:
      level = 2;
      score += 100 * level);
      break;
    case 3:
      level = 3;
      score += 150 * level;
      break;
    case 4:
      level = 4;
      score += 200 * level;
      break;
  }
}

void queueNewBlock() { // randomizarea pieselor
  //Serial.println("Metoda queueNewBlock");
  int randomBlock = random(BLOCK_COUNT - 1);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 8; j++)
      moving[i][j] = blocks[randomBlock][i][j];
  }
}

int isMovingAtBottom() { // mutarea piesei pana la ultima linie disponibila
 // Serial.println("Metoda isMovingAtBotton");
  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 8; j++) {
      if (moving[i][j] && i == 11 || (moving[i][j] && i >= 4 && i + 1 < 12 && line[i - 4 + 1][j]))
        return 1;
    }
  }
  return 0;
}

int isGameOver() { // sfarsitul jocului: daca nu se mai pot adauga piese
  //Serial.println("Metoda isGameOver");
  for (int j = 0; j < 8; j++) {
    if (line[0][j])
      return 1;

  }
  
  return 0;
}

void handleAtBottom() {
  Serial.println("Metoda handleAtBottom");
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      line[i][j] = line[i][j] || moving[i + 4][j];
      moving[i + 4][j] = 0;
    }
  }
}

void rotate90() { // rotirea piesei
  Serial.println("Metoda rotate90");
  struct point pivot = { -1, -1};

  int updatedArr[12][8];

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 8; j++) {
      updatedArr[i][j] = 0;
      if (moving[i][j] == 2) {
        pivot.y = i;
        pivot.x = j;
      }
    }
  }

  if (pivot.y == -1 || pivot.x == -1)
    return;

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 8; j++) {
      if (moving[i][j] == 1) {
        struct point toCoordinate = {j - pivot.x, i - pivot.y};
        struct point transformedCoordinate = { -toCoordinate.y, toCoordinate.x};
        struct point transformedGrid = {transformedCoordinate.x + pivot.x, transformedCoordinate.y + pivot.y};

        if (transformedGrid.x >= 8 || transformedGrid.x < 0 || transformedGrid.y >= 12 || transformedGrid.y < 0 || (transformedGrid.y >= 4 && line[transformedGrid.y - 4][transformedGrid.x]))
          return;

        updatedArr[transformedGrid.y][transformedGrid.x] = 1;
      } else if (moving[i][j] == 2)
          updatedArr[i][j] = 2;

    }
  }

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 8; j++)
      moving[i][j] = updatedArr[i][j];
  }
}

void transformMoving(int x, int y) {// deplasarea piesei
  Serial.println("Metoda transformMoving");
  y = -y;
  int updatedArr[12][8];
  int xValid = 1;

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 8; j++)
      updatedArr[i][j] = 0;
  }

  for (int i = 0; i < 12 && xValid; i++) {
    for (int j = 0; j < 8 && xValid; j++) {
      if (moving[i][j]) {
        if (j + x >= 8 || j + x < 0)
          xValid = 0;
      }
    }
  }

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 8; j++) {
      if (moving[i][j]) {
        if (i + y < 12 && i + y >= 0) {
          if (xValid) {
            if (i >= 4 && line[i - 4 + y][j + x])
              return;
            updatedArr[i + y][j + x] = moving[i][j];
          } else
              updatedArr[i + y][j] = moving[i][j];
        }
      }
    }
  }

  for (int i = 0; i < 12; i++) {
    for (int j = 0; j < 8; j++)
      moving[i][j] = updatedArr[i][j];
  }
}
