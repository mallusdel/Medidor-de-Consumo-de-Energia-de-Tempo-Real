#include <Wire.h>                // Comunicação I2C
#include <LiquidCrystal_I2C.h>   // LCD I2C
#include <EmonLib.h>             // Biblioteca para medição de energia

// Inicializa o LCD I2C (Endereço 0x27, 16 colunas, 2 linhas)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Definições dos pinos analógicos
#define SENSOR_CORRENTE_PIN A3
#define SENSOR_TENSAO_PIN   A2

EnergyMonitor emon;

// Tarifas de energia
const float TARIFA_BAIXA = 0.273866; 
const float TARIFA_ALTA  = 0.469484;

// Calibração dos sensores
const float CALIBRACAO_TENSAO  = 230.0;
const float CALIBRACAO_CORRENTE = 60.6;

unsigned long tempoAnterior = 0;
const long intervaloAtualizacao = 3000; // 3 segundos

float consumoKWH_total = 0.0;
float preco_total = 0.0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();

  emon.voltage(SENSOR_TENSAO_PIN, CALIBRACAO_TENSAO, 1.7);
  emon.current(SENSOR_CORRENTE_PIN, CALIBRACAO_CORRENTE);
}

void loop() {
  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoAnterior >= intervaloAtualizacao) {
    tempoAnterior = tempoAtual;

    emon.calcVI(20, 2000); 

    float tensaoRMS = emon.Vrms;
    float correnteRMS = emon.Irms;
    float potenciaReal = emon.realPower;

    float tempoHoras = (float)intervaloAtualizacao / 3600000.0;
    float consumoKW_inst = potenciaReal / 1000.0;
    consumoKWH_total += (consumoKW_inst * tempoHoras);

    float preco_inst = 0.0;
    if (correnteRMS <= 30.0) {
      preco_inst = consumoKW_inst * TARIFA_BAIXA;
    } else { 
      preco_inst = consumoKW_inst * TARIFA_ALTA;
    }
    preco_total += preco_inst * tempoHoras;

    // --- Exibição no LCD ---
    lcd.clear();

    // Linha 1: Consumo acumulado (kWh)
    lcd.setCursor(0, 0);
    lcd.print("Consumo:");
    lcd.print(consumoKWH_total, 3);
    lcd.print(" kWh");

    // Linha 2: Preço acumulado (R$)
    lcd.setCursor(0, 1);
    lcd.print("Preco:");
    lcd.print(preco_total, 3);
    lcd.print(" R$");

    // --- Debug Serial ---
    Serial.print("Tensao: "); Serial.print(tensaoRMS); Serial.println("V");
    Serial.print("Corrente: "); Serial.print(correnteRMS); Serial.println("A");
    Serial.print("Potencia: "); Serial.print(potenciaReal); Serial.println("W");
    Serial.print("Consumo Total: "); Serial.print(consumoKWH_total, 2); Serial.println("  kWh");
    Serial.print("Preco Total: "); Serial.println(preco_total, 2); Serial.println(" RS");
    Serial.println("-------------------");
  }
}
