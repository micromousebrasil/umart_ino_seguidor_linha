/**
 *****************************************************************
 * Arquivo  umart_ino_seguidor_linha.ino
 * Autor    Kleber Lima da Silva (micromousebrasil@gmail.com)
 * Versão   V1.0.0
 * Data     12-Maio-2015
 * Resumo   Programa de demonstração do Robô uMaRT-INO no modo
 *         robotracer (seguidor de linha)
 *****************************************************************
 */
 
// Definição dos pinos no modo ROBOTRACER ------------------------
const int LINHA6 = A0;     // Sensor de linha 6 (esquerda)
const int LINHA5 = A1;     // Sensor de linha 5
const int LINHA4 = A2;     // Sensor de linha 4
const int LINHA3 = A3;     // Sensor de linha 3
const int LINHA2 = A4;     // Sensor de linha 2
const int LINHA1 = A5;     // Sensor de linha 1 (direita)
const int EMISSORES = 13;  // LEDs dos sensores de linha
const int LED1 = 0;        // LED1
const int LED2 = 1;        // LED2
const int B_ENC_E = 2;     // Encoder do motor esquerdo (sinal B)
const int A_ENC_D = 3;     // Encoder do motor direito (sinal A)
const int A_ENC_E = 4;     // Encoder do motor esquerdo (sinal A)
const int B_ENC_D = 5;     // Encoder do motor direito (sinal B)
const int PWM_E = 6;       // PWM do motor esquerdo
const int IN2_E = 7;       // IN2 da ponte H do motor esquerdo
const int IN1_E = 8;       // IN1 da ponte H do motor esquerdo
const int IN1_D = 9;       // IN1 da ponte H do motor direito
const int IN2_D = 10;      // IN2 da ponte H do motor direito
const int PWM_D = 11;      // PWM do motor direito
const int SW1 = 12;        // Botão SW1

// Constantes do controlador -------------------------------------
const float KP = 1;
const int PWM_MEDIO = 70;

// Definição das constantes do programa --------------------------
#define LINHA HIGH  // LOW: linha branca | HIGH: linha preta
#define INFINITO 255  // Para indicar que nenhum sensor leu linha

// Protótipos das funções ----------------------------------------
int lerSensoresLinha(void);
void setMotores(int pwm_esquerda, int pwm_direita);

// Variáveis -----------------------------------------------------
int erro = 0, erro_anterior = 0;
int integral = 0, derivativo = 0, MV = 0;


// Inicialização dos pinos ---------------------------------------
void setup()
{ 
  pinMode(LINHA6, INPUT);
  pinMode(LINHA5, INPUT);
  pinMode(LINHA4, INPUT);
  pinMode(LINHA3, INPUT);
  pinMode(LINHA2, INPUT);
  pinMode(LINHA1, INPUT);
  pinMode(EMISSORES, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(PWM_E, OUTPUT);
  pinMode(IN1_E, OUTPUT);
  pinMode(IN2_E, OUTPUT);
  pinMode(PWM_D, OUTPUT);
  pinMode(IN1_D, OUTPUT);
  pinMode(IN2_D, OUTPUT);
  pinMode(SW1, INPUT);
    
  // Aguarda o botão SW1 ser pressionado para ligar o robô
  while(digitalRead(SW1) == LOW);
  delay(1000);
}

// LOOP principal do programa ------------------------------------
void loop()
{
  unsigned long t0;
  
  // Registra o tempo inicial para garantir uma base de tempo fixa
  t0 = micros();
  
  // Verifica a leitura dos sensores
  erro = lerSensoresLinha();
  if (erro == INFINITO)
  {
    erro = erro_anterior;
  }
  
  // Indica o estado dos sensores através dos LEDs
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  if (erro > 0)
  {
    digitalWrite(LED1, HIGH);
  }
  else if (erro < 0)
  {
    digitalWrite(LED2, HIGH);
  }
  else if (erro == 0)
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
  }
  
  // Controlador Proporcional
  MV = KP * erro;
  
  // Acionamento dos motores
  setMotores(PWM_MEDIO + MV, PWM_MEDIO - MV);
  
  // Aguarda 1 milisegundo (base de tempo do controlador)
  while((micros() - t0) < 1000);
}


/* Função para leitura dos sensores de linha ---------------------
 * Retorna valores entre -30 e 30 (valores negativos indicam que
 * o robô precisa se deslocar para a esquerda)
 */
int lerSensoresLinha(void)
{
  int erro = 0, soma = 0, n = 0;
  unsigned long t0 = micros();
  
  // Habilita os emissores por 100 us
  digitalWrite(EMISSORES, HIGH);
  while((micros() - t0) < 100);
  
  // Realiza a leitura de todos os sensores de linha. Os sensores
  // das extremidades possuem peso maior. No final é realizada
  // a média ponderada
  if(digitalRead(LINHA1) == LINHA)
  {
    soma += -30;
    n++;
  }
  if(digitalRead(LINHA2) == LINHA)
  {
    soma += -20;
    n++;
  }
  if(digitalRead(LINHA3) == LINHA)
  {
    soma += -10;
    n++;
  }
  if(digitalRead(LINHA4) == LINHA)
  {
    soma += 10;
    n++;
  }
  if(digitalRead(LINHA5) == LINHA)
  {
    soma += 20;
    n++;
  }
  if(digitalRead(LINHA6) == LINHA)
  {
    soma += 30;
    n++;
  }
  
  // Desabilita os emissores
  digitalWrite(EMISSORES, LOW);
  
  // Retorna a média ou retorna a constante INFINITO indicando
  // que nenhum sensor leu linha
  if(n != 0) erro = soma / n;
  else erro = INFINITO;
  
  return erro;
}


/* Função para acionamento dos motores ---------------------------
 * pwm_esquerda e pwm_direita recebem valores entre -255 e 255
 * (valores negativos giram o respectivo motor para trás)
 */
void setMotores(int pwm_esquerda, int pwm_direita)
{
  if(pwm_esquerda < 0)
  {
    pwm_esquerda = -pwm_esquerda;
  
    digitalWrite(IN1_E, LOW);
    digitalWrite(IN2_E, HIGH);
  }
  else
  {
    digitalWrite(IN1_E, HIGH);
    digitalWrite(IN2_E, LOW);
  }
  
  if(pwm_direita < 0)
  {
    pwm_direita = -pwm_direita;
  
    digitalWrite(IN1_D, LOW);
    digitalWrite(IN2_D, HIGH);
  }
  else
  {
    digitalWrite(IN1_D, HIGH);
    digitalWrite(IN2_D, LOW);
  }
  
  if(pwm_esquerda > 255) pwm_esquerda = 255;
  if(pwm_direita > 255) pwm_direita = 255;
  
  analogWrite(PWM_E, pwm_esquerda);
  analogWrite(PWM_D, pwm_direita);
}

