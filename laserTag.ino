#include <IRremote.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_IR_RECEIVER 4 // Pino do receptor IR no ESP32
#define MAX_AMMO 6        // Máximo de disparos antes de recarregar
#define COOLDOWN_SHOT 1000   // Cooldown de 1s entre tiros
#define COOLDOWN_RELOAD 5000 // Cooldown de 5s para recarga

#define LED_GREEN 5 // Vida cheia
#define LED_YELLOW 18 // Vida média
#define LED_RED 19   // Vida baixa

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço do LCD (ajuste conforme necessário)
IRrecv receiver(PIN_IR_RECEIVER);

int ammo = MAX_AMMO; // Quantidade de balas iniciais
int health = 10;     // Vida do jogador
unsigned long lastShotTime = 0;

// DECLARAÇÃO DAS FUNÇÕES
void updateLCD();
void updateLEDs();
void reload();
void shoot();
void translateIR();

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Laser Tag Ready");

  receiver.enableIRIn(); // Inicializa o receptor IR

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  updateLCD();  // Mostra os valores iniciais corretamente
  updateLEDs(); // Atualiza LEDs no início
}

void loop() {
  if (receiver.decode()) { // Se receber um sinal do controle
    translateIR();
    receiver.resume();
  }
}

void translateIR() {
  switch (receiver.decodedIRData.command) {
    case 2: // Botão "+"
      shoot();
      break;
    case 152: // Botão "-"
      reload();
      break;
    default:
      break;
  }
}

void shoot() {
  if (ammo > 0 && health > 0) {  // Impede atirar se a vida for 0
    unsigned long now = millis();
    if (now - lastShotTime >= COOLDOWN_SHOT) {
      ammo--;
      lastShotTime = now;
      health--; // Simula dano ao jogador ao atirar
      updateLCD(); // Atualiza o LCD apenas quando atirar
      updateLEDs(); // Atualiza LEDs conforme a vida
      if (ammo == 0) {
        lcd.setCursor(0, 1);
        lcd.print("Recarregando...");
        delay(COOLDOWN_RELOAD);
        reload();
      }
    }
  }
}

void reload() {
    if (health > 0) {  // Impede recarregar se a vida for 0
      ammo = MAX_AMMO;
      updateLCD(); // Atualiza o LCD após recarga
    }
  }
  
  void updateLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
  
    if (health == 0) {  // Se a vida chegou a 0, exibe "Derrota..."
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
    } else {  // Apaga todos os LEDs se a vida for 0
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_YELLOW, LOW);
      digitalWrite(LED_RED, LOW);
    }
  }