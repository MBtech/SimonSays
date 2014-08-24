#include <Tone.h>

// circuit stuff
#define BUTTON1_PIN 2
#define BUTTON2_PIN 3
#define BUTTON3_PIN 4
#define BUTTON4_PIN 5
#define DIFFICULTY_PIN 6
#define LED1_PIN 8
#define LED2_PIN 9
#define LED3_PIN 10
#define LED4_PIN 11
#define SPEAKER_PIN 12

// logic stuff
#define NUM_ROUNDS 100
#define DEFAULT_DIFFICULTY 1

Tone speakerpin;

// start tune
int starttune[] = {NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_C4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4};
int duration2[] = {150, 300, 150, 300, 150, 600, 150, 150, 150, 150, 300, 150, 750};

// success tune
int note[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_G4, NOTE_C5};
int duration[] = {150, 150, 150, 300, 150, 600};

// buttons and leds organized as an array
boolean button[] = {BUTTON1_PIN, BUTTON2_PIN, BUTTON3_PIN, BUTTON4_PIN}; //The four button input pins
boolean ledpin[] = {LED1_PIN, LED2_PIN, LED3_PIN, LED4_PIN};  // LED pins

int turn = 0;  // turn counter
int buttonstate = 0;  // button state checker
int randomArray[NUM_ROUNDS]; //Intentionally long to store up to 100 inputs (doubtful anyone will get this far)
int inputArray[NUM_ROUNDS];

unsigned long inputTimeout; // milliseconds allowed for user inactivity
unsigned long timeLastInput;
unsigned showDelay;
unsigned showPauseDelay;
unsigned inputDelay;

unsigned difficulty; // difficulty 0-3
unsigned diffInputTimeout[] = {5000, 4000, 2500, 1000};
unsigned diffShowDelay[] = {400, 300, 200, 100};
unsigned diffShowPauseDelay[] = {100, 75, 50, 25};
unsigned diffInputDelay[] = {60, 50, 40, 30};



void setup() {
  Serial.begin(9600);
  speakerpin.begin(SPEAKER_PIN); // speaker is on pin 12

  for(int x=0; x<4; x++) { // LED pins are outputs
    pinMode(ledpin[x], OUTPUT);
  }
  
  for(int x=0; x<4; x++) {
    pinMode(button[x], INPUT);  // button pins are inputs
    digitalWrite(button[x], HIGH);  // enable internal pullup; buttons start in high position; logic reversed
  }
  
  pinMode(DIFFICULTY_PIN, INPUT);  // button pins are inputs
  digitalWrite(DIFFICULTY_PIN, HIGH);  // enable internal pullup; buttons start in high position; logic reversed

  randomSeed(analogRead(0)); //Added to generate "more randomness" with the randomArray for the output function
  
  for (int thisNote = 0; thisNote < 13; thisNote ++) {
     // play the next note:
     speakerpin.play(starttune[thisNote]);
     // hold the note:
     if (thisNote==0 || thisNote==2 || thisNote==4 || thisNote== 6) {
       digitalWrite(ledpin[0], HIGH);
     }
     if (thisNote==1 || thisNote==3 || thisNote==5 || thisNote== 7 || thisNote==9 || thisNote==11) {
       digitalWrite(ledpin[1], HIGH);
     }
     if (thisNote==8 || thisNote==12) {
       digitalWrite(ledpin[2], HIGH);
     }  
     if (thisNote==10) {   
       digitalWrite(ledpin[3], HIGH);
     }
     delay(duration2[thisNote]);
     // stop for the next note:
     speakerpin.stop();
     digitalWrite(ledpin[0], LOW);
     digitalWrite(ledpin[1], LOW);
     digitalWrite(ledpin[2], LOW);
     digitalWrite(ledpin[3], LOW);
     //delay(25);
  }
  
}
 
 
void loop() {   
    
  selectDifficulty();
    
  for (int y=0; y<NUM_ROUNDS; y++) {
    //function for generating the array to be matched by the player
    digitalWrite(ledpin[0], HIGH);
    digitalWrite(ledpin[1], HIGH);
    digitalWrite(ledpin[2], HIGH);
    digitalWrite(ledpin[3], HIGH);
  
    for (int thisNote = 0; thisNote < 6; thisNote ++) {
     // play the next note:
     speakerpin.play(note[thisNote]);
     // hold the note:
     delay(duration[thisNote]);
     // stop for the next note:
     speakerpin.stop();
     //delay(25);
    }
    
    digitalWrite(ledpin[0], LOW);
    digitalWrite(ledpin[1], LOW);
    digitalWrite(ledpin[2], LOW);
    digitalWrite(ledpin[3], LOW);
    delay(1000);
  
    for (int y=turn; y <= turn; y++) { //Limited by the turn variable
      Serial.println(""); //Some serial output to follow along
      Serial.print("Turn: ");
      Serial.print(y);
      Serial.println("");
      randomArray[y] = random(1, 5); //Assigning a random number (1-4) to the randomArray[y], y being the turn count
      for (int x=0; x <= turn; x++) {
        Serial.print(randomArray[x]);
      
        for(int y=0; y<4; y++) {
          //if statements to display the stored values in the array
          if (randomArray[x] == 1 && ledpin[y] == LED1_PIN) {
            digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_G3, 100);
            delay(showDelay);
            digitalWrite(ledpin[y], LOW);
            delay(showPauseDelay);
          }

          if (randomArray[x] == 2 && ledpin[y] == LED2_PIN) {
            digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_A3, 100);
            delay(showDelay);
            digitalWrite(ledpin[y], LOW);
            delay(showPauseDelay);
          }
  
          if (randomArray[x] == 3 && ledpin[y] == LED3_PIN) {
            digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_B3, 100);
            delay(showDelay);
            digitalWrite(ledpin[y], LOW);
            delay(showPauseDelay);
          }

          if (randomArray[x] == 4 && ledpin[y] == LED4_PIN) {
            digitalWrite(ledpin[y], HIGH);
            speakerpin.play(NOTE_C4, 100);
            delay(showDelay);
            digitalWrite(ledpin[y], LOW);
            delay(showPauseDelay);
          }
        }
      }
      Serial.println("");
    }
    input();
  }
}
 
 
 
void input() { //Function for allowing user input and checking input against the generated array

  timeLastInput = millis();

  for (int x=0; x <= turn;) {
    //Statement controlled by turn count
    
    // timeout code (5s timeout)
    long inputTimeDiff = millis() - timeLastInput;
    if(inputTimeDiff > inputTimeout) {
      Serial.println("timeout");
      fail();
    }

    for(int y=0; y<4; y++) {
      
      buttonstate = digitalRead(button[y]);
      
      // reset input timeout if button pressed
      if(buttonstate == LOW) {
        timeLastInput = millis();
      }

      //Checking for button push
      if (buttonstate == LOW && button[y] == BUTTON1_PIN) { 
        digitalWrite(ledpin[0], HIGH);
        speakerpin.play(NOTE_G3, 100);
        delay(showDelay);
        digitalWrite(ledpin[0], LOW);
        inputArray[x] = 1;
        delay(inputDelay); 
        Serial.print(1);
        if (inputArray[x] != randomArray[x]) { //Checks value input by user and checks it against
          fail();                              //the value in the same spot on the generated array
        }                                      //The fail function is called if it does not match
        x++;
      }
       if (buttonstate == LOW && button[y] == BUTTON2_PIN) {
        digitalWrite(ledpin[1], HIGH);
        speakerpin.play(NOTE_A3, 100);
        delay(showDelay);
        digitalWrite(ledpin[1], LOW);
        inputArray[x] = 2;
        delay(inputDelay);
        Serial.print(2);
        if (inputArray[x] != randomArray[x]) {
          fail();
        }
        x++;
      }

      if (buttonstate == LOW && button[y] == BUTTON3_PIN) {
        digitalWrite(ledpin[2], HIGH);
        speakerpin.play(NOTE_B3, 100);
        delay(showDelay);
        digitalWrite(ledpin[2], LOW);
        inputArray[x] = 3;
        delay(inputDelay);
        Serial.print(3);
        if (inputArray[x] != randomArray[x]) {
          fail();
        }
        x++;
      }

      if (buttonstate == LOW && button[y] == BUTTON4_PIN) {
        digitalWrite(ledpin[3], HIGH);
        speakerpin.play(NOTE_C4, 100);
        delay(showDelay);
        digitalWrite(ledpin[3], LOW);
        inputArray[x] = 4;
        delay(inputDelay);
        Serial.print(4);
        if (inputArray[x] != randomArray[x]) 
        {
          fail();
        }
        x++;
      }
    }
    
  }
  
  turn++; //Increments the turn count, also the last action before starting the output function over again
}

void fail() { //Function used if the player fails to match the sequence
 
  for (int y=0; y<3; y++) {
    //Flashes lights for failure
    digitalWrite(ledpin[0], HIGH);
    digitalWrite(ledpin[1], HIGH);
    digitalWrite(ledpin[2], HIGH);
    digitalWrite(ledpin[3], HIGH);
    speakerpin.play(NOTE_G3, 300);
    delay(200);
    digitalWrite(ledpin[0], LOW);
    digitalWrite(ledpin[1], LOW);
    digitalWrite(ledpin[2], LOW);
    digitalWrite(ledpin[3], LOW);
    speakerpin.play(NOTE_C3, 300);
    delay(200);
  }
  delay(500);
  turn = -1; //Resets turn value so the game starts over without need for a reset button
  Serial.println(" failed.");
  selectDifficulty();
}

void selectDifficulty() {
  
  difficulty = DEFAULT_DIFFICULTY;
  showDifficulty();
  
  // read inputs
  while(true) {
    for(int i=0; i<4; i++) {
      if(digitalRead(button[i]) == LOW) {
        difficulty = i;
        showDifficulty();
      }
    }
    if(digitalRead(DIFFICULTY_PIN) == LOW) {
      break;
    }
  }
  
  // free the LEDs!
  digitalWrite(ledpin[0], LOW);
  digitalWrite(ledpin[1], LOW);
  digitalWrite(ledpin[2], LOW);
  digitalWrite(ledpin[3], LOW);
  
  // set difficulty variables
  inputTimeout = diffInputTimeout[difficulty];
  showDelay = diffShowDelay[difficulty];
  showPauseDelay = diffShowPauseDelay[difficulty];
  inputDelay = diffInputDelay[difficulty];
  Serial.print("Selected difficulty: ");
  Serial.println(difficulty);
  
}

void showDifficulty() {
  for(int i=0; i<=difficulty; i++) {
    digitalWrite(ledpin[i], HIGH);
  }
  for(int i=difficulty+1; i<=4; i++) {
    digitalWrite(ledpin[i], LOW);
  }
}
