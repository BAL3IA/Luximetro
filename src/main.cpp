#include <Arduino.h>
#include <math.h>
#include "fir_coef.h" 

#define PIN_LED 2
#define PIN_PWM 3
#define PIN_LEITURA A0
#define BUFFER_LEN 50
#define FILTER_ORDER 50 
#define ADC_MAX 1023 
#define RESISTENCIA (float) 10000.0
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
    pinMode(PIN_PWM, OUTPUT);

    Serial.begin(115200);

    // inicializa todos os membros de leituras[] para zero
    for (uint8_t i = 0; i < BUFFER_LEN; i ++) leituras[i] = 0;

    setRegistros();  
}

void loop() {

    if (fflag) {

        // uint16_t tmp = getFiltrado(); 

        // // MODO FAST-PWM. Top = 249 -> 1 kHz
        // OCR3C = (uint8_t)((float) tmp * 249.0 / 1023.0);

        // MODO FAST-PWM. Top = 1023 -> 1953,125 Hz
        OCR3C = filtrado; 

        Serial.print("Intensidade Luminosa: ");
        Serial.println(getLux()); 

        fflag = false;
    } 
}  

// rotina de interrupção do Timer 1 com amostragem
ISR(TIMER1_COMPA_vect) {

    // // linhas de teste
    // static bool ON = true; 
    // digitalWrite(PIN_LED, ON); 
    // ON = !ON; 

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

// configura os registros para interrupção no Timer1 e PWM no Timer3
void setRegistros() {   

    cli();

    // habilita interrupção no TIMER1
    TCCR1A = 0x00;      // CTC mode, 
    TCCR1B = 0x0C;      // prescaler = 256
    // OCR1A = 0xF423;    // 0.5 Hz 
    // OCR1A = 0x07A;     // 1 Hz
    // OCR1A = 0x0C34;    // 10 Hz
    // OCR1A = 0x0619;    // 20 Hz 
    OCR1A = 0x0138;    // 100 Hz
    TIMSK1 |= 1 << 1;

    // habilita fast-PWM no Timer3 - modo 15
    TCCR3A = 0x0F;      // saída invertida em OC3C = pino 3 // 0x0B para saída sem inversão
    // TCCR3B = 0x1B;      // presc = 64
    TCCR3B = 0x1A;         // presc = 8
    // OCR3A = 0xF9;       // 1 kHz; -> presc = 64
    OCR3A = 0x03FF;        // 1953,125 Hz 

    // // habilita phase corret-PWM no Timer3 - modo 11
    // TCCR3A = 0x0F;      // saída invertida em OC3C = pino 3 // 0x0B para saída sem inversão
    // TCCR3B = 0x12;      // presc = 8 
    // OCR3A = 0x03FF;       // 977,52 kHz; 

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



