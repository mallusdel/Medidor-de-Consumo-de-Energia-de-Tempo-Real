#include <Wire.h>          // Necessário para o CLOCK (RTC) - Módulo RTC usa comunicação I2C
#include <LiquidCrystal.h> // Biblioteca para LCD sem módulo I2C
#include <EmonLib.h>       // Biblioteca para medição de energia

// Definições dos pinos do LCD para o Arduino (modo 4-bit)
// LiquidCrystal(rs, en, d4, d5, d6, d7)
// Suas ligações: RS(Pino 4 LCD) -> 9 Arduino, EN(Pino 6 LCD) -> 8 Arduino,
// D4(Pino 11 LCD) -> 7 Arduino, D5(Pino 12 LCD) -> 6 Arduino,
// D6(Pino 13 LCD) -> 5 Arduino, D7(Pino 14 LCD) -> 4 Arduino
LiquidCrystal lcd(9, 8, 7, 6, 5, 4); 

// Definições para os pinos analógicos
#define SENSOR_CORRENTE_PIN A3 // Saída do sensor de corrente ligada no A3
#define SENSOR_TENSAO_PIN   A0 // Saída do sensor de tensão ligada no A2

// Instância da biblioteca EmonLib
EnergyMonitor emon;

// Variáveis para as taxas de energia em R$/kWh
const float TARIFA_BAIXA = 0.273866; // R$/kWh para corrente <= 30A
const float TARIFA_ALTA = 0.469484;  // R$/kWh para corrente > 30A (assumindo a próxima faixa)

// Variáveis para calibração 
const float CALIBRACAO_TENSAO = 234.2; // Exemplo: ajustado para 127V AC (pode variar)
const float CALIBRACAO_CORRENTE = 60.6; // Exemplo: ajustado para SCT-013-000 100A/1V

unsigned long tempoAnterior = 0;
const long intervaloAtualizacao = 3000; // Intervalo de atualização do LCD (3 segundos)

float consumoKWH_total = 0.0;
float preco_total = 0.0;

void setup() {
  Serial.begin(9600);
  
  // Inicializa a comunicação I2C, necessária para o módulo RTC (CLOCK)
  Wire.begin(); 

  // Configura o número de colunas e linhas do LCD
  lcd.begin(16, 2); 
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  delay(2000);
  lcd.clear();

  // Define os pinos e calibrações para a EmonLib
  // emon.voltage(pino_tensão, calib_tensão, fator_correção_fase);
  emon.voltage(SENSOR_TENSAO_PIN, CALIBRACAO_TENSAO, 1.7); 
  // emon.current(pino_corrente, calib_corrente);
  emon.current(SENSOR_CORRENTE_PIN, CALIBRACAO_CORRENTE); 
}

void loop() {
  unsigned long tempoAtual = millis();

  if (tempoAtual - tempoAnterior >= intervaloAtualizacao) {
    tempoAnterior = tempoAtual;

    // Calcula todas as leituras de energia com a EmonLib
    emon.calcVI(20, 2000); 

    float tensaoRMS = emon.Vrms;
    float correnteRMS = emon.Irms;
    float potenciaReal = emon.realPower; // Potência real em Watts

    // Cálculo do consumo em kWh
    float tempoHoras = (float)intervaloAtualizacao / 3600000.0; // Converte ms para horas
    float consumoKW_inst = potenciaReal / 1000.0; // Potência instantânea em kW
    
    // Acumula o consumo total em kWh
    consumoKWH_total += (consumoKW_inst * tempoHoras);

    // Cálculo do preço baseado na corrente RMS para selecionar a tarifa
    float preco_inst = 0.0;
    if (correnteRMS <= 30.0) {
      preco_inst = consumoKW_inst * TARIFA_BAIXA;
    } else { 
      preco_inst = consumoKW_inst * TARIFA_ALTA;
    }
    preco_total += preco_inst * tempoHoras; // Acumula o preço total

    // --- Exibição no LCD ---
    lcd.clear(); // Limpa a tela antes de escrever novos valores
    
    // Linha 1: Consumo em kWh
    lcd.setCursor(0, 0); // Coluna 0, Linha 0
    lcd.print("Consumo: ");
    lcd.print(consumoKWH_total, 2); // Exibe com 2 casas decimais
    lcd.print(" kWh");

    // Linha 2: Preço total
    lcd.setCursor(0, 1); // Coluna 0, Linha 1
    lcd.print("Preco: R$ ");
    lcd.print(preco_total, 2); // Exibe com 2 casas decimais (moeda)
    
    // --- Debug no Serial Monitor ---
    Serial.print("Tensao: "); Serial.print(tensaoRMS); Serial.println("V");
    Serial.print("Corrente: "); Serial.print(correnteRMS); Serial.println("A");
    Serial.print("Potencia: "); Serial.print(potenciaReal); Serial.println("W");
    Serial.print("Consumo Instantaneo: "); Serial.print(consumoKW_inst, 2); Serial.println(" kW");
    Serial.print("Consumo Total: "); Serial.print(consumoKWH_total, 2); Serial.println(" kWh");
    Serial.print("Preco Total: R$ "); Serial.println(preco_total, 2);
    Serial.println("---");
  }
}
