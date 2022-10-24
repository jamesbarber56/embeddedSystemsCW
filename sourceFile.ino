#include <EEPROM.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define STARTSCREEN 5
#define MAINMENU 0
#define DEVSTAGEEGG 1
#define DEVSTAGEONE 2
#define DEVSTAGETWO 3
#define GAMEOVER 4

int state = STARTSCREEN;
bool growing=0, decreasing=0;

struct creature{
  int devStage;
  int happiness;
  int fullness;
  unsigned long age;
};

creature Animal; 


unsigned long currentTime = 0;
unsigned long prevHappiness = 0;
unsigned long prevFullness = 0;
unsigned long prevTime = 0;
unsigned long enterMenu = 0;
unsigned long menuTime = 0;
unsigned long prevClick = 0;

//to make the interface better.
byte downArrow[8] = { 0b00100,0b00100,0b00100,0b00100,0b10001,0b01010,0b00100,0b00000};
byte upArrow[8] = { 0b00100,0b01010,0b10001,0b00100,0b00100,0b00100,0b00100,0b00000};

//makes the egg pixel art when on egg stage
byte egg1[8] = {0b00000,0b00000,0b00001,0b00011,0b00011,0b00111,0b00111,0b01000};
byte egg2[8] = {0b00000,0b11111,0b10000,0b11000,0b11000,0b10000,0b00000,0b00000};
byte egg3[8] = {0b00000,0b10000,0b10000,0b01000,0b01000,0b00100,0b00100,0b10010};
byte egg4[8] = {0b01000,0b01000,0b01011,0b01111,0b00111,0b00011,0b00000,0b00000};
byte egg5[8] = {0b00001,0b00001,0b10000,0b11000,0b11100,0b11100,0b11111,0b00000};
byte egg6[8] = {0b11010,0b11010,0b10010,0b00010,0b00100,0b11000,0b00000,0b00000};



void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.createChar(1,downArrow);
  lcd.createChar(2,upArrow);
  lcd.createChar(3,egg1);
  lcd.createChar(4,egg2);
  lcd.createChar(5,egg3);
  lcd.createChar(6,egg4);
  lcd.createChar(7,egg5);
  lcd.createChar(8,egg6);
  enterMenu = 0;
}


void loop(){
  uint8_t buttons = lcd.readButtons();
  switch(state){
    case STARTSCREEN:
      lcd.setCursor(2,0);
      lcd.print("START NEW ->");
      lcd.setCursor(2,1);
      lcd.write(1);
      lcd.print(" CONTINUE ");
      lcd.write(1);  
      if(buttons){
        if(buttons & BUTTON_RIGHT){
          resetGame();
          loadGame();
          state = MAINMENU;
          lcd.clear();
        }
        if(buttons & BUTTON_DOWN){
          loadGame();
          state = MAINMENU;
          lcd.clear();
        }
      }
      break;
    case MAINMENU:
      growing = 0, decreasing = 0;
      lcd.setCursor(0,0);
      lcd.print("   MAIN  MENU   ");
      lcd.setCursor(0,1);
      lcd.print("<-EXIT == CONT ");
      lcd.write(2);
      if(buttons){
        if(buttons & BUTTON_UP){
          if(Animal.devStage == 0){
            state = DEVSTAGEEGG;
          }
          else if(Animal.devStage == 1){
            prevFullness = prevFullness + (currentTime - enterMenu);
            prevHappiness = prevHappiness + (currentTime - enterMenu);
            state = DEVSTAGEONE;
          }
          else if(Animal.devStage == 2){
            prevFullness = prevFullness + (currentTime - enterMenu);
            prevHappiness = prevHappiness + (currentTime - enterMenu);
            state = DEVSTAGETWO;
          }
          prevTime = prevTime + (currentTime - enterMenu);
          
          lcd.clear();
        }
        if(buttons & BUTTON_LEFT){
          state = STARTSCREEN;
          saveGame();
          lcd.clear();
        }
      }
      break;

    case DEVSTAGEEGG:
      growing = 1, decreasing = 0;
      Animal.devStage = 0;
      lcd.setCursor(1,0);
      lcd.print("Egg Stage");
      lcd.setCursor(1,1);
      lcd.print("Age: ");
      lcd.print(Animal.age);
      lcd.setCursor(12,0);
      lcd.write(3);
      lcd.write(4);
      lcd.write(5);
      lcd.setCursor(12,1);
      lcd.write(6);
      lcd.write(7);
      lcd.write(8);
      if(Animal.age >= 5){
        state = DEVSTAGEONE;
        prevHappiness = currentTime;
        prevFullness = currentTime;
        lcd.clear();
      }
      if(buttons){
        if(buttons & BUTTON_DOWN){
          state = MAINMENU;
          enterMenu = currentTime;
          lcd.clear();
        }
      }
      break;
      
    case DEVSTAGEONE:
      growing = 1; decreasing = 1;
      Animal.devStage = 1;
      lcd.setCursor(0,0);
      lcd.print("Food: ");
      lcd.print(Animal.fullness);
      lcd.setCursor(8,0);
      lcd.print("Happy: ");
      lcd.print(Animal.happiness);
      lcd.setCursor(0,1);
      lcd.print("Dev: ");
      lcd.print(Animal.devStage);
      lcd.setCursor(7,1);
      lcd.print("Age: ");
      lcd.print(Animal.age);
      if(Animal.age >= 600){
        state = GAMEOVER;
        lcd.clear();
      }
      if(buttons){
        if(buttons & BUTTON_DOWN){
          state = MAINMENU;
          enterMenu = currentTime;
          lcd.clear();
        }
        if(buttons & BUTTON_LEFT){
          if((currentTime - prevClick >= 500)){
            feedAnimal();
          }
        }
        if(buttons & BUTTON_RIGHT){
          if((currentTime - prevClick >= 500)){
            playWithAnimal();
          }
        }
        if(buttons & BUTTON_UP){
          boolean grow = growPet();
          if(grow){
            state = DEVSTAGETWO;
            lcd.clear();
          }
        }
      }
      break;
      
    case DEVSTAGETWO:
      growing = 1; decreasing = 1;
      Animal.devStage = 2;
      lcd.setCursor(0,0);
      lcd.print("Food: ");
      lcd.print(Animal.fullness);
      lcd.setCursor(8,0);
      lcd.print("Happy: ");
      lcd.print(Animal.happiness);
      lcd.setCursor(0,1);
      lcd.setCursor(0,1);
      lcd.print("Dev: ");
      lcd.print(Animal.devStage);
      lcd.setCursor(7,1);
      lcd.print("Age: ");
      lcd.print(Animal.age);
      if(Animal.age >= 600){
        state = GAMEOVER;
        lcd.clear();
      }
      if(buttons){
        if(buttons & BUTTON_DOWN){
          state = MAINMENU;
          enterMenu = currentTime;
          lcd.clear();
        }
        if(buttons & BUTTON_LEFT){
          if((currentTime - prevClick >= 250)){
            feedAnimal();
          }
        }
        if(buttons & BUTTON_RIGHT){
          if((currentTime - prevClick >= 500)){
            playWithAnimal();
          }
        }
      }
      break;
      
    case GAMEOVER:
      growing = 0; decreasing = 0;
      lcd.setCursor(0,0);
      lcd.print("   GAME  OVER   ");
      lcd.setCursor(0,1);
      lcd.write(2);
      lcd.print(" START SCREEN ");
      lcd.write(2);
      if(buttons){
        if(buttons & BUTTON_UP){
          state = STARTSCREEN;
          resetGame();
          lcd.clear();
        }
      }
      break;
  }
  
  currentTime = millis();
  
  if(growing){
    if(currentTime-prevTime >= 1000){
      Animal.age = Animal.age + 1;
      Serial.print("Age: ");
      Serial.println(Animal.age);
      Serial.print("Fullness: ");
      Serial.println(Animal.fullness);
      Serial.print("Happiness: ");
      Serial.println(Animal.happiness);
      Serial.println();
      prevTime = currentTime;
    }
  }
  if(decreasing){
    if(currentTime-prevFullness >= 11000 & Animal.fullness > 0){
      decreaseFullness();
      prevFullness = currentTime;
    }
    if(currentTime-prevHappiness >= 7000 & Animal.happiness > 0){
      decreaseHappiness();
      prevHappiness = currentTime;
    }
  }
}

void decreaseHappiness(){
    Animal.happiness--;
    Serial.println("Happiness decreased by 1"); 
}

void decreaseFullness(){
    Animal.fullness--;
    Serial.println("Fullness decreased by 1"); 
}

void playWithAnimal(){
  prevClick = currentTime;
  if(Animal.happiness < 3){
    if(Animal.fullness >= 2){
      prevHappiness = currentTime;
      Animal.happiness++;
      Serial.println("You have played with your animal :D");
    } else {
      Serial.println("Animal is too hungry to have fun!");
    }
  } else {
    Serial.println("Your animal has already had too much fun!");
  }
}

boolean growPet(){
  if(Animal.age >= 35 & Animal.fullness >= 3 & Animal.happiness >= 1){
    Animal.devStage = 2;
    return true;
  } else{
    return false;
  }
}

void feedAnimal(){
  prevClick = currentTime;
  if(Animal.fullness < 4){
    prevFullness = currentTime;
    Animal.fullness++;
    Serial.println("You have fed your animal!");
    if(Animal.fullness == 4){
      Serial.println("You overfed them, now they are sad!");
      Animal.happiness = 0;
    }
  } else {
    Serial.println("Your animal is already overfull!");
  }
}

void resetGame(){
  int stats[4] = {0, 3, 2};
  for(int i=0;i < 3; i++){
    EEPROM.write(i, stats[i]);
  }
  EEPROM.write(3, 0);
}

void loadGame(){
  Animal.devStage = EEPROM.read(0);
  Animal.happiness = EEPROM.read(1);
  Animal.fullness = EEPROM.read(2);
  Animal.age = EEPROM.read(3);
}

void saveGame(){
  int stats[3] = {Animal.devStage, Animal.happiness, Animal.fullness};
  for(int i=0;i < 3; i++){
    EEPROM.write(i, stats[i]);
  }
  EEPROM.write(3, Animal.age);
}
