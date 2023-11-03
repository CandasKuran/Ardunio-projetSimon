// Candas KURAN 
// 23.10.2023
// Projet Simon

// Définition des boutons et LEDs
const int buttons[] = {A2, A3, A4, A5}; // Pins des boutons
const int leds[] = {2, 3, 4, 5};        // Pins des LEDs
const int tones[] = {330, 262, 220, 660}; // Fréquences pour chaque couleur
const int ERROR_TONE = 262;              // Fréquence en cas d'erreur
const int NB_COLORS = 4;                 // Nombre de couleurs
const int TONE_PIN = 6;                  // Pin du buzzer
const int MAX_SEQUENCE = 31;             // Longueur maximale de la séquence

int sequence[MAX_SEQUENCE];              // Séquence générée aléatoirement
int sequenceLength = 1;                  // Longueur actuelle de la séquence
int playerStep = 0;                      // Étape actuelle du joueur
bool gameStarted = false;                // Indique si le jeu a commencé
int currentDelay = 600;                  // Variable pour la vitesse du jeu



// Définition des notes
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
#define NOTE_SUSTAIN 300

void setup() {
  Serial.begin(9600); // Initialisation de la communication série

  // Configuration des boutons et LEDs
  for (int i = 0; i < NB_COLORS; i++) {
    pinMode(buttons[i], INPUT_PULLUP); // Mettre le bouton en entrée avec une résistance pull-up
    pinMode(leds[i], OUTPUT);   // Initialiser le LED comme sortie
    digitalWrite(leds[i], LOW); // Éteindre le LED
  }
  pinMode(TONE_PIN, OUTPUT); // Configuration du buzzer
  waitForStart(); // Attendre que le joueur commence le jeu
}

void loop() {
  if (gameStarted) {  // Si le jeu a démarré
    playSequence();  // Jouer la séquence
    playerTurn();   // Tour du joueur
  }
}

// Fonction pour attendre le démarrage du jeu
void waitForStart() {
  // Indiquez au joueur comment démarrer le jeu et on va afficher sur serial monitor
  Serial.println("Press the A2 button (green button) to start the game.");

  // Attendre que le joueur appuie sur le bouton A2 pour démarrer
  while (!gameStarted) {
    if (digitalRead(buttons[3]) == LOW) { // Correction here: buttons[0] for A2
      playIntroWithLeds();  // Jouer une séquence d'introduction avec LEDs
      startupMusicAndLeds(); // Jouer une mélodie pour indiquer le démarrage
      Serial.println("The game is starting...");
      delay(1000);
      startNewGame(); // Commencez un nouveau jeu
      gameStarted = true; // Marquer que le jeu a commencé
    }
    delay(50);
  }
}

// Fonction pour jouer la séquence de démarrage
void startupMusicAndLeds() {
  // Jouer une musique et allumer les LEDs pour indiquer le début du jeu
  for (int i = 0; i < NB_COLORS; i++) {
    tone(TONE_PIN, tones[i], 500);
    digitalWrite(leds[i], HIGH);
    delay(600);
    digitalWrite(leds[i], LOW);
  }
}

// Fonction pour démarrer un nouveau jeu
void startNewGame() {
  // Générer une nouvelle séquence pour le joueur

  randomSeed(analogRead(A0));

  for (int i = 0; i < MAX_SEQUENCE; i++) {
    sequence[i] = random(NB_COLORS);
  }
  sequenceLength = 1;
  playerStep = 0;
  currentDelay = 600; // recommencer avec la vitesse du jeu à sa valeur de départ
  Serial.println("New game started!");
}

void playSequence() {
  for (int i = 0; i < sequenceLength; i++) {
    int color = sequence[i];
    digitalWrite(leds[color], HIGH);
    tone(TONE_PIN, tones[color]);
    delay(currentDelay);
    digitalWrite(leds[color], LOW);
    noTone(TONE_PIN);
    delay(currentDelay / 3);
  }
  
  // Augmenter la vitesse du jeu tous les 3 tour
  if (sequenceLength % 3 == 0) {
    currentDelay *= 0.75; // Augmenter le vitesse de 25%.
    if (currentDelay < 100) { // Ajouter une limite inférieure pour éviter que le jeu ne devienne trop rapide
      currentDelay = 100;
    }
  }
}

// Fonction pour le tour du joueur
void playerTurn() {
  // Laisser le joueur essayer de suivre la séquence
  playerStep = 0;

  while (playerStep < sequenceLength) {
    for (int i = 0; i < NB_COLORS; i++) {
      if (digitalRead(buttons[i]) == LOW) {
        if (i == sequence[playerStep]) {
          playerStep++;
          digitalWrite(leds[i], HIGH);
          tone(TONE_PIN, tones[i]);
          delay(300);
          digitalWrite(leds[i], LOW);
          noTone(TONE_PIN);
        } else {
          playError(); // Jouer un ton d'erreur si le joueur fait une erreur
          Serial.println("Error! The game restarts.");
          startupMusicAndLeds();
          delay(1000);
          startNewGame(); // Recommencez le jeu après une erreur
          return;
        }
      }
    }
  }
  // Si le joueur suit correctement toute la séquence
  if (playerStep == sequenceLength) {
    if (sequenceLength < MAX_SEQUENCE) {
      sequenceLength++;
      delay(1000);
    } else {
      Serial.println("Congratulations! You've won!");
      playVictory(); // Jouer une mélodie de victoire
      delay(2000);
      startNewGame(); // Recommencer après la victoire
    }
  }
}


// Fonction pour jouer la séquence d'introduction
void playIntroWithLeds() {
    for (int i = 0; i < 3; i++) {
  
    for (int j = 0; j < NB_COLORS; j++) {
      digitalWrite(leds[j], HIGH);
    }

    if (i < 2) {
      tone(TONE_PIN, NOTE_C5, 200);
      delay(250);
      tone(TONE_PIN, NOTE_E5, 200);
      delay(250);
      tone(TONE_PIN, NOTE_G5, 200);
      delay(250);
      tone(TONE_PIN, NOTE_E5, 200);
      delay(250);
    } else {
      delay(1000);  
    }

  
    for (int j = 0; j < NB_COLORS; j++) {
      digitalWrite(leds[j], LOW);
    }

    delay(500);
  }

  noTone(TONE_PIN);
}


// Fonction pour clignoter tous les LEDs
void blinkAllLeds() {
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < NB_COLORS; j++) {
      digitalWrite(leds[j], HIGH);
    }
    delay(500);
    for (int j = 0; j < NB_COLORS; j++) {
      digitalWrite(leds[j], LOW);
    }
    delay(500);
  }
}


// Fonction pour jouer la séquence de victoire
void playVictory() {
  for (uint8_t nLoop = 0; nLoop < 2; nLoop++) {
    tone(TONE_PIN, NOTE_A5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_B5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_C5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_B5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_C5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_D5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_C5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_D5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_E5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_D5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_E5);
    delay(NOTE_SUSTAIN);
    tone(TONE_PIN, NOTE_E5);
    delay(NOTE_SUSTAIN);
  }
  noTone(TONE_PIN);
}


// Fonction pour jouer la séquence d'erreur
void playError() {
  tone(TONE_PIN, NOTE_E5, 200);
  delay(250);
  tone(TONE_PIN, NOTE_D5, 200);
  delay(250);
  tone(TONE_PIN, NOTE_C5, 200);
  delay(250);
  noTone(TONE_PIN);
  blinkAllLeds();
}


