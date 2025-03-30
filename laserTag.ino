#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_IR_RECEIVER 4  // Pino do receptor IR no ESP32
#define MAX_AMMO 6         // Máximo de disparos antes de recarregar
#define COOLDOWN_SHOT 1000  // Cooldown de 1s entre tiros
#define COOLDOWN_RELOAD 5000 // Cooldown de 5s para recarga

#define LED_GREEN 5   // Vida cheia
#define LED_YELLOW 18 // Vida média
#define LED_RED 19    // Vida baixa

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço do LCD I2C
IRrecv receiver(PIN_IR_RECEIVER);

int ammo = MAX_AMMO;        // Quantidade de balas iniciais do jogador
int health = 10;            // Vida inicial do jogador
unsigned long lastShotTime = 0; // Para cooldown de tiros do jogador

// "Pratinho" - simulação multiplayer
int otherPlayerAmmo = MAX_AMMO;
int otherPlayerHealth = 10;
unsigned long lastEnemyShotTime = 0; // Para cooldown de tiros do inimigo

// **DECLARAÇÃO DAS FUNÇÕES**
void updateLCD();
void updateLEDs();
void reload(bool isPlayerReloading);
void shoot(bool isPlayerShooting);
void translateIR();
void printOtherPlayerStats();
void checkGameOver();  // Nova função que verifica se o jogo acabou
void resetGame();      // Nova função para reiniciar o jogo

void setup() {
  Serial.begin(9600);  // Inicializa a comunicação serial
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Laser Tag Ready");

  receiver.enableIRIn();  // Inicializa o receptor IR

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  updateLCD();
  updateLEDs();
  printOtherPlayerStats();
}

void loop() {
  if (receiver.decode()) {
    translateIR();
    receiver.resume();
  }

  // Recarga automática
  if (otherPlayerAmmo == 0) {
    reload(false);
  }
  if (ammo == 0) {
    reload(true);
  }

  checkGameOver();  // Verifica se alguém venceu ou perdeu
}

void translateIR() {
  switch (receiver.decodedIRData.command) {
    case 2:   // Botão "+" -> Pires atira
      shoot(true);
      break;
    case 152:  // Botão "-" -> Pratinho atira
      shoot(false);
      break;
    default:
      break;
  }
}

void shoot(bool isPlayerShooting) {
  unsigned long now = millis();

  if (isPlayerShooting) {
    if (ammo > 0 && health > 0 && (now - lastShotTime >= COOLDOWN_SHOT)) {
      ammo--;
      otherPlayerHealth--;
      lastShotTime = now;

      updateLCD();
      updateLEDs();
      printOtherPlayerStats();
    }
  } else {
    if (otherPlayerAmmo > 0 && otherPlayerHealth > 0 && (now - lastEnemyShotTime >= COOLDOWN_SHOT)) {
      otherPlayerAmmo--;
      health--;
      lastEnemyShotTime = now;

      updateLCD();
      updateLEDs();
      printOtherPlayerStats();
    }
  }

  checkGameOver();  // Após cada tiro, verifica se o jogo acabou
}

void reload(bool isPlayerReloading) {
  if (isPlayerReloading && ammo == 0 && health > 0) {
    Serial.println("Pires Recarregando...");
    delay(COOLDOWN_RELOAD);
    ammo = MAX_AMMO;
    updateLCD();
  }
  if (!isPlayerReloading && otherPlayerAmmo == 0 && otherPlayerHealth > 0) {
    Serial.println("Pratinho Recarregando...");
    delay(COOLDOWN_RELOAD);
    otherPlayerAmmo = MAX_AMMO;
    printOtherPlayerStats();
  }
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);

  if (health == 0) {
    lcd.print("Derrota...");
    lcd.setCursor(0, 1);
    lcd.print("Fim de jogo!");
  } else {
    lcd.print("Vida: ");
    lcd.print(health);
    lcd.setCursor(0, 1);
    lcd.print("Balas: ");
    lcd.print(ammo);
  }
}

void updateLEDs() {
  if (health > 6) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  } else if (health > 3) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
  } else if (health > 0) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }
}

void printOtherPlayerStats() {
  Serial.print("Pratinho -> Vida: ");
  Serial.print(otherPlayerHealth);
  Serial.print(" | Balas: ");
  Serial.println(otherPlayerAmmo);
}

void checkGameOver() {
  if (health <= 0) {  // Jogador perdeu
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Derrota...");
    lcd.setCursor(0, 1);
    lcd.print("Reiniciando...");
    Serial.println("Pratinho venceu!");
    delay(5000);
    resetGame();
  } else if (otherPlayerHealth <= 0) {  // Jogador venceu
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Vitoria!");
    lcd.setCursor(0, 1);
    lcd.print("Reiniciando...");
    Serial.println("Pires venceu!");
    delay(5000);
    resetGame();
  }
}

void resetGame() {
  // Reinicializa as variáveis do jogo
  health = 10;
  ammo = MAX_AMMO;
  otherPlayerHealth = 10;
  otherPlayerAmmo = MAX_AMMO;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Laser Tag Ready");
  updateLCD();
  updateLEDs();
  printOtherPlayerStats();
}
