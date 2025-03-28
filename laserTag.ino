#include <IRremote.hpp>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define PIN_RECEIVER 4   // Pino do receptor IR no Wokwi
#define LED_GREEN 5      // LED para vida alta
#define LED_YELLOW 18    // LED para vida média
#define LED_RED 19       // LED para vida baixa

#define COOLDOWN_SHOT 1000    // Cooldown entre tiros (1 segundo)
#define COOLDOWN_RELOAD 5000  // Tempo de recarga (5 segundos)
#define MAX_AMMO 6            // Máximo de balas por recarga
#define MAX_HEALTH 10         // Vida máxima do jogador

int playerHealth = MAX_HEALTH;
int enemyHealth = MAX_HEALTH;
int ammo = MAX_AMMO;
unsigned long lastShotTime = 0;
bool reloading = false;
bool gameOver = false;

LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço I2C do LCD

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(PIN_RECEIVER, ENABLE_LED_FEEDBACK); // Inicializa receptor IR

  lcd.init();
  lcd.backlight();

  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  updateLCD();
  updateLEDs();
}

void loop() {
  if (gameOver) return; // Se o jogo acabou, para a execução

  if (IrReceiver.decode()) {
    unsigned long command = IrReceiver.decodedIRData.command; // Obtém código do controle
    translateIR(command);
    IrReceiver.resume();
  }
}

void translateIR(unsigned long command) {
  if (gameOver) return; // Bloqueia ações após o fim do jogo

  switch (command) {
    case 0x18: // Botão "+"
      shoot(2); // Atira no inimigo (log serial)
      break;
    case 0x4A: // Botão "-"
      shoot(152); // Atira no próprio jogador (LCD e LEDs)
      break;
  }
}

void shoot(int command) {
  unsigned long now = millis();

  if (gameOver || reloading) {
    Serial.println("Recarregando... espere.");
    return;
  }

  if (ammo > 0) {
    if (now - lastShotTime >= COOLDOWN_SHOT) {
      lastShotTime = now;
      ammo--;

      if (command == 2) { // Disparo no inimigo
        if (enemyHealth > 0) {
          enemyHealth--;
        }

        Serial.print("Vida do inimigo: ");
        Serial.print(enemyHealth);
        Serial.print(" | Balas restantes: ");
        Serial.println(ammo);

        updateLCD(); // ✅ Agora também atualiza o LCD

        if (enemyHealth == 0) {
          Serial.println("Inimigo derrotado! Você venceu!");
          lcd.clear();
          lcd.setCursor(4, 0);
          lcd.print("Vitória!");
          gameOver = true;
          return;
        }
      } 
      else if (command == 152) { // Disparo no próprio jogador
        if (playerHealth > 0) {
          playerHealth--;
        }

        updateLCD();
        updateLEDs();

        if (playerHealth == 0) {
          Serial.println("Você foi derrotado!");
          lcd.clear();
          lcd.setCursor(3, 0);
          lcd.print("Derrota...");
          gameOver = true;
          return;
        }
      }

      if (ammo == 0) {
        Serial.println("Recarregando...");
        reloading = true;
        lcd.clear();
        lcd.setCursor(3, 0);
        lcd.print("Recarregando...");
        delay(COOLDOWN_RELOAD);
        reload();
      }
    }
  }
}


void reload() {
  if (gameOver) return; // Evita recarga após fim de jogo
  ammo = MAX_AMMO;
  reloading = false;
  updateLCD();
  Serial.println("Munição recarregada.");
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Inimigo: ");
  lcd.print(enemyHealth);
  lcd.setCursor(0, 1);
  lcd.print("Balas: ");
  lcd.print(ammo);
}

void updateLEDs() {
  if (playerHealth >= 7) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  } else if (playerHealth >= 4) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
  } else if (playerHealth >= 1) {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, HIGH);
  } else {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
  }
}