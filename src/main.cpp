#include <Arduino.h>
#include <math.h>
#include "fir_coef.h" 

#define PIN_LED 2
#define PIN_LEITURA A0
#define BUFFER_LEN 50
#define FILTER_ORDER 50 
#define ADC_MAX 1023 
#define RESISTENCIA 10000
#define COEF_A (float) -1.138
#define COEF_B (float) exp(16.178)

volatile bool fflag = false;
volatile uint8_t contador = 0;
volatile uint16_t leituras[BUFFER_LEN];
extern float filtro[FILTER_ORDER]; 
volatile float filtrado = (float) 0.0; 
  
void setRegistros(); 
float getFiltrado();
uint16_t getLux();
 
void setup() {

    pinMode(PIN_LED, OUTPUT); 

    // inicializa todos os membros de leituras[] para zero
    for (uint8_t i = 0; i < BUFFER_LEN; i ++) leituras[i] = 0;

    setRegistros();  
}

void loop() {

    if (fflag) {

        Serial.println(getFiltrado());

        fflag = false;
    }
}  

// rotina de interrupção do Timer 1 com amostragem
ISR(TIMER1_COMPA_vect) {

    // linhas de teste
    static bool ON = true; 
    digitalWrite(PIN_LED, ON); 
    ON = !ON; 

    // amostragem
    if (contador < BUFFER_LEN) {

        leituras[contador] = analogRead(PIN_LEITURA); 
    }

    else {

        contador = 0; 
        
        leituras[contador] = analogRead(PIN_LEITURA);
    }

    filtrado = getFiltrado();

    fflag = true;

    contador++; 
}

// configura os registros para interrupção no Timer1
void setRegistros() { 

    cli();
 
    TCCR1A = 0x00;       // CTC mode, prescaler = 256
    TCCR1B = 0x0C;
    // OCR1A = 0xF423;     // 0.5 Hz 
    // OCR1A = 0x0C34;     // 1 Hz
    // OCR1A = 0x7A11;  // 10 Hz
    OCR1A = 0x0619;     // 20 Hz 
    TIMSK1 |= 1 << 1;

    // desabilita a entrada digital nos pinos de  A0 à A15
    DIDR0 = 0x00;
    DIDR2 = 0x00;
     
    sei();
}

// retorna um valor de leitura filtrado
float getFiltrado() {

    float resultado = (float) 0.0;
    uint8_t aux = BUFFER_LEN + contador;

    for (uint8_t i = 0; i < FILTER_ORDER; i++) {

        if ( i <= contador) resultado += leituras[i]*filtro[((contador - i) % BUFFER_LEN)];

        else resultado += leituras[i]*filtro[aux - i];
    }

    return resultado;
}

// retorna o valor da intensidade luminosa
uint16_t getLux() {
 
    float base = (float)(ADC_MAX - filtrado) * RESISTENCIA / (float) filtrado;

    return (uint16_t) floor(COEF_B * pow(base,COEF_A)); 
}



