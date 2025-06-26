#include <Wire.h>          // Necessário para o CLOCK (RTC) - Módulo RTC usa comunicação I2C
#include <LiquidCrystal.h> // Biblioteca para LCD sem módulo I2C
#include <EmonLib.h>       // Biblioteca para medição de energia

// Definições dos pinos do LCD para o Arduino (modo 4-bit)
// LiquidCrystal(rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(9, 8, 7, 6, 5, 4); 

// Definições para os pinos analógicos
#define SENSOR_CORRENTE_PIN A3 // Saída do sensor de corrente ligada no A3
#define SENSOR_TENSAO_PIN   A2 // Saída do sensor de tensão ligada no A2

// Instância da biblioteca EmonLib
EnergyMonitor emon;

// Variáveis para as taxas de energia em R$/kWh
const float TARIFA_BAIXA = 0.273866; 
const float TARIFA_ALTA = 0.469484;  

// Variáveis para calibração 
const float CALIBRACAO_TENSAO = 234.2; 
const float CALIBRACAO_CORRENTE = 60.6; 

unsigned long tempoAnterior = 0;
const long intervaloAtualizacao = 2000; // Intervalo de atualização do LCD (2 segundos)

float consumoKWH_total = 0.0;
float preco_total = 0.0;

// Variáveis para medir WCET de cada tarefa
unsigned long wcet_t1_max = 0;
unsigned long wcet_t2_max = 0;
unsigned long wcet_t3_max = 0;
unsigned long wcet_t4_max = 0;

void setup() {
  Serial.begin(115200); 
  Wire.begin(); 
  lcd.begin(16, 2); 
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

    // --- TAREFA 1: Leitura de sinais analógicos ---
    start_time = micros();
    // O emon.calcVI já faz as leituras de tensão e corrente
    // e calcula internamente alguns parâmetros brutos.
    // Consideramos esta linha como a parte principal da T1.
    emon.calcVI(20, 2000); 
    end_time = micros();
    exec_time = end_time - start_time;
    if (exec_time > wcet_t1_max) wcet_t1_max = exec_time;
    float tensaoRMS = emon.Vrms;
    float correnteRMS = emon.Irms;
    float potenciaReal = emon.realPower; 
    
    // --- TAREFA 2: Cálculo de potência ativa ---
    // A potenciaReal já foi calculada dentro de emon.calcVI()
    start_time = micros();
    float tempoHoras = (float)intervaloAtualizacao / 3600000.0; // Converte ms para horas
    float consumoKW_inst = potenciaReal / 1000.0; // Potência instantânea em kW
    consumoKWH_total += (consumoKW_inst * tempoHoras); // Acumula o consumo total em kWh
    end_time = micros();
    exec_time = end_time - start_time;
    if (exec_time > wcet_t2_max) wcet_t2_max = exec_time;

    // --- TAREFA 3: Conversão em valor monetário ---
    start_time = micros();
    float preco_inst = 0.0;
    if (correnteRMS <= 30.0) {
      preco_inst = consumoKW_inst * TARIFA_BAIXA;
    } else { 
      preco_inst = consumoKW_inst * TARIFA_ALTA;
    }
    preco_total += preco_inst * tempoHoras; // Acumula o preço total
    end_time = micros();
    exec_time = end_time - start_time;
    if (exec_time > wcet_t3_max) wcet_t3_max = exec_time;

    // --- TAREFA 4: Exibição no display ---
    start_time = micros();
    lcd.clear(); 
    lcd.setCursor(0, 0);
    lcd.print("Consumo: ");
    lcd.print(consumoKWH_total, 2);
    lcd.print(" kWh");

    lcd.setCursor(0, 1);
    lcd.print("Preco: R$ ");
    lcd.print(preco_total, 2);
    end_time = micros();
    exec_time = end_time - start_time;
    if (exec_time > wcet_t4_max) wcet_t4_max = exec_time;
    
    // --- Debug no Serial Monitor (sem medição de tempo, para não interferir) ---
    Serial.print("Tensao: "); Serial.print(tensaoRMS); Serial.println("V");
    Serial.print("Corrente: "); Serial.print(correnteRMS); Serial.println("A");
    Serial.print("Potencia: "); Serial.print(potenciaReal); Serial.println("W");
    Serial.print("Consumo Instantaneo: "); Serial.print(consumoKW_inst, 2); Serial.println(" kW");
    Serial.print("Consumo Total: "); Serial.print(consumoKWH_total, 2); Serial.println(" kWh");
    Serial.print("Preco Total: R$ "); Serial.println(preco_total, 2);
    Serial.println("---");

    // --- Exibindo os WCETs para cada tarefa ---
    Serial.print("WCET T1 (Leitura): "); Serial.print(wcet_t1_max); Serial.println(" us");
    Serial.print("WCET T2 (Calculo): "); Serial.print(wcet_t2_max); Serial.println(" us");
    Serial.print("WCET T3 (Conversao): "); Serial.print(wcet_t3_max); Serial.println(" us");
    Serial.print("WCET T4 (Exibicao): "); Serial.print(wcet_t4_max); Serial.println(" us");
    Serial.println(" "); // Linha em branco para separar as medições
  }
}

