// ============================================================
//  Semáforo Inteligente com LDR
//  Componentes: LEDs (pinos 2, 3, 4), LDR (A0),
//               Potenciômetro (A1)
//
//  Modo Normal : Verde → Amarelo → Vermelho (temporizações reais)
//  Modo Noturno: Pisca amarelo (quando LDR < limiar)
//  O potenciômetro ajusta o limiar de sensibilidade do LDR
// ============================================================

// --- Pinos ---
const int PIN_VERMELHO = 2;
const int PIN_AMARELO  = 3;
const int PIN_VERDE    = 4;
const int PIN_LDR      = A0;
const int PIN_POT      = A1;

// --- Temporizações do modo normal (ms) ---
const unsigned long T_VERDE    = 5000;  // Verde aceso por 5s
const unsigned long T_AMARELO  = 2000;  // Amarelo aceso por 2s
const unsigned long T_VERMELHO = 5000;  // Vermelho aceso por 5s

// --- Temporizações do modo noturno (ms) ---
const unsigned long T_PISCA_ON  = 500;  // Amarelo aceso por 0,5s
const unsigned long T_PISCA_OFF = 500;  // Amarelo apagado por 0,5s

// --- Máquina de estados ---
enum Estado {
  VERDE,
  AMARELO,
  VERMELHO,
  NOTURNO_ON,
  NOTURNO_OFF
};

Estado estadoAtual = VERDE;
unsigned long tempoUltimaMudanca = 0;

// ============================================================
void setup() {
  pinMode(PIN_VERMELHO, OUTPUT);
  pinMode(PIN_AMARELO,  OUTPUT);
  pinMode(PIN_VERDE,    OUTPUT);

  Serial.begin(9600);

  // Inicia no verde
  setLEDs(LOW, LOW, HIGH);
  tempoUltimaMudanca = millis();
}

// ============================================================
void loop() {
  unsigned long agora = millis();

  // Lê o LDR e o limiar do potenciômetro
  int valorLDR   = analogRead(PIN_LDR);
  int limiar     = analogRead(PIN_POT);  // 0–1023 mapeado direto
  bool modoNoturno = (valorLDR < limiar);

  Serial.print("LDR: "); Serial.print(valorLDR);
  Serial.print(" | Limiar: "); Serial.print(limiar);
  Serial.print(" | Noturno: "); Serial.println(modoNoturno ? "SIM" : "NAO");

  // --- MODO NOTURNO ---
  if (modoNoturno) {
    // Se estava no modo normal, reinicia estado noturno
    if (estadoAtual == VERDE || estadoAtual == AMARELO || estadoAtual == VERMELHO) {
      setLEDs(LOW, LOW, LOW);
      estadoAtual = NOTURNO_OFF;
      tempoUltimaMudanca = agora;
    }

    if (estadoAtual == NOTURNO_OFF && (agora - tempoUltimaMudanca >= T_PISCA_OFF)) {
      setLEDs(LOW, HIGH, LOW);       // Amarelo ON
      estadoAtual = NOTURNO_ON;
      tempoUltimaMudanca = agora;
    }
    else if (estadoAtual == NOTURNO_ON && (agora - tempoUltimaMudanca >= T_PISCA_ON)) {
      setLEDs(LOW, LOW, LOW);        // Amarelo OFF
      estadoAtual = NOTURNO_OFF;
      tempoUltimaMudanca = agora;
    }
    return;
  }

  // --- MODO NORMAL ---
  // Se estava no modo noturno, volta para o Verde
  if (estadoAtual == NOTURNO_ON || estadoAtual == NOTURNO_OFF) {
    setLEDs(LOW, LOW, HIGH);  // Verde
    estadoAtual = VERDE;
    tempoUltimaMudanca = agora;
    return;
  }

  switch (estadoAtual) {
    case VERDE:
      if (agora - tempoUltimaMudanca >= T_VERDE) {
        setLEDs(LOW, HIGH, LOW);  // Amarelo
        estadoAtual = AMARELO;
        tempoUltimaMudanca = agora;
      }
      break;

    case AMARELO:
      if (agora - tempoUltimaMudanca >= T_AMARELO) {
        setLEDs(HIGH, LOW, LOW);  // Vermelho
        estadoAtual = VERMELHO;
        tempoUltimaMudanca = agora;
      }
      break;

    case VERMELHO:
      if (agora - tempoUltimaMudanca >= T_VERMELHO) {
        setLEDs(LOW, LOW, HIGH);  // Verde
        estadoAtual = VERDE;
        tempoUltimaMudanca = agora;
      }
      break;

    default:
      break;
  }
}

// ============================================================
// Liga/desliga os três LEDs de uma vez
// ============================================================
void setLEDs(int vermelho, int amarelo, int verde) {
  digitalWrite(PIN_VERMELHO, vermelho);
  digitalWrite(PIN_AMARELO,  amarelo);
  digitalWrite(PIN_VERDE,    verde);
}
