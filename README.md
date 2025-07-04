# Sistema de Monitoramento de Consumo de Energia e Custo em Tempo Real Utilizando Arduino
#### Autores: Maria Lima | Rosilene Monteiro | Thiago Pinheiro

### APRESENTAÇÃO
#### Este projeto contém os algorítimos utilizados para o desenvolvimento do medidor digital que exibe potência ativa e precificação de consumo de energia elétrica, bem como, o código para determinar o WCET de cada atividade realizada pelo processador do microcontrolador - Arduino Uno. 

### INFORMAÇÕES ÚTEIS
#### Na pasta <Medidor_Codigo.ino> pode ser acessado o código fonte utilizado para o desenvolvimento do medidor de consumo. Sua finalidade é a captação dos sinais de tensão e corrente enviados pelos sensores conectados na unidade consumidora (residencial) e realizar o processamento, com definição de tarefas rígidas e cíclicas, o processador armazena os dados, faz o primeiro processamento - Cálculo de Potência Ativa Instantânea, armazena novamente os dados internamente, realiza a próxima tarefa - Calculo da tarifa (de acordo com  a tarifa vigente da região - Manaus|Am), armazena os dados e imprime a informação em um display de LCD I2C - 16x2. 

#### Na pasta <WCET_Codigo.ino> se encontra uma implementação no código fonte, cuja finalidade é a de exibir os WCETs (EWorst-Case Execution Time). Para tal, todo o sistema deve estar funcionando e o código após ser implementado no arduino, gera, no monitor serial da IDE do Arduino, os WCETs de cada atividade. 

### CIRCUITO DE LIGAÇÃO DO SISTEMA
![CKT1](https://github.com/user-attachments/assets/ed439e29-7d33-419c-85cf-571b5dbfeaeb)

### WCET E ESCALONABILIDADE DO SISTEMA
![CKT2](https://github.com/user-attachments/assets/b0d6c4b9-32cd-47ac-8625-c80b51d00444)

### AVALIAÇÃO DO CONSUMO DE MEMÓRIA
![CJT3](https://github.com/user-attachments/assets/dc049d60-f8b8-4b4b-bbaf-351f91fb41d8)

### ANÁLISE COMPARATIVA DA PROPOSTA COM O SISTEMA ELETROMECÂNICO
![CKT4](https://github.com/user-attachments/assets/c04c876c-1e8b-43d7-bfbf-85d03e5a3723)

## Simulações
### INICIALIZAÇÃO DO SISTEMA: 
https://github.com/user-attachments/assets/4c6b2501-8836-400a-bf2c-c23229b87a86

### AVALIAÇÃO DO CONSUMO - LÂMPADA: 
https://github.com/user-attachments/assets/fd44e620-92ad-48e8-9ae9-14d0c559167a

### AVALIAÇÃO DO CONSUMO - PRANCHA CERÂMICA: 
https://github.com/user-attachments/assets/ffad75a0-2afd-4cf4-ba9f-bf324820389d

### AVALIAÇÃO DO CONSUMO - FERRO DE PASSAR: 
https://github.com/user-attachments/assets/abbafbc7-e3c4-4033-8a3d-931177f6d78a

### SIMULAÇÃO - SECCIONAMENTO DE ENERGIA PARA RESIDÊNCIA:
https://github.com/user-attachments/assets/8729bc90-e9a3-4c49-a2b4-2514b3a4cb0f

### SIMULAÇÃO - PARADA FORÇADA DO SISTEMA:
https://github.com/user-attachments/assets/f60ea7c7-76ee-4263-a5e5-d96b72f882b0










