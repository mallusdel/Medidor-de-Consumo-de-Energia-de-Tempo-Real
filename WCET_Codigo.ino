#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EmonLib.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define SENSOR_CORRENTE_PIN A3
#define SENSOR_TENSAO_PIN   A2

EnergyMonitor emon;

const float TARIFA_BAIXA = 0.273866; 
const float TARIFA_ALTA  = 0.469484;
const float CALIBRACAO_TENSAO  = 230.0;
const float CALIBRACAO_CORRENTE = 60.6;

unsigned long tempoAnterior = 0;
const long intervaloAtualizacao = 3000;

float consumoKWH_total = 0.0;
float preco_total = 0.0;

// Variáveis para guardar o WCET máximo de cada tarefa
unsigned long wcet_t1_max = 0;
unsigned long wcet_t2_max = 0;
unsigned long wcet_t3_max = 0;
unsigned long wcet_t4_max = 0;

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

    unsigned long start_time, end_time, exec_time;

    // --- Tarefa 1: Leitura e cálculo de sinais 
    start_time = micros();
    emon.calcVI(20, 2000); 
    float tensaoRMS = emon.Vrms;
    float correnteRMS = emon.Irms;
    float potenciaReal = emon.realPower;
    end_time = micros();
    exec_time = end_time - start_time;
    if (exec_time > wcet_t1_max) wcet_t1_max = exec_time;

    // --- Tarefa 2: Cálculo de consumo e preço 
    start_time = micros();
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
    end_time = micros();
    exec_time = end_time - start_time;
    if (exec_time > wcet_t2_max) wcet_t2_max = exec_time;

    // --- Tarefa 3: Exibição no LCD 
    start_time = micros();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Consumo:");
    lcd.print(consumoKWH_total, 3);
    lcd.print(" kWh");

    lcd.setCursor(0, 1);
    lcd.print("Preco:");
    lcd.print(preco_total, 3);
    lcd.print(" R$");
    end_time = micros();
    exec_time = end_time - start_time;
    if (exec_time > wcet_t3_max) wcet_t3_max = exec_time;

    // --- Tarefa 4: Debug Serial 
    start_time = micros();
    Serial.print("Tensao: "); Serial.print(tensaoRMS); Serial.println("V");
    Serial.print("Corrente: "); Serial.print(correnteRMS); Serial.println("A");
    Serial.print("Potencia: "); Serial.print(potenciaReal); Serial.println("W");
    Serial.print("Consumo Total: "); Serial.print(consumoKWH_total, 2); Serial.println(" kWh");
    Serial.print("Preco Total: "); Serial.println(preco_total, 2);
    Serial.println("-------------------");

    Serial.print("WCET T1 (Leitura): "); Serial.print(wcet_t1_max); Serial.println(" us");
    Serial.print("WCET T2 (Calculo): "); Serial.print(wcet_t2_max); Serial.println(" us");
    Serial.print("WCET T3 (LCD): "); Serial.print(wcet_t3_max); Serial.println(" us");
    Serial.print("WCET T4 (Serial): "); Serial.print(wcet_t4_max); Serial.println(" us");
    Serial.println();
    end_time = micros();
    exec_time = end_time - start_time;
    if (exec_time > wcet_t4_max) wcet_t4_max = exec_time;
  }
}
