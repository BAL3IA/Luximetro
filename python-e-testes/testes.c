// testes para algoritimo de aplicação do filtro

#include <stdio.h>
#include <stdint-gcc.h>

#define MAX 10
#define ORDEM 10

uint16_t arr[MAX];
double filter[ORDEM];

uint16_t contador = 0;

void main(void) {

    uint8_t tmp;

    for (uint8_t i = 0; i < MAX; i++) arr[i] = 0;
    for (uint8_t i = 0; i < ORDEM; i++) filter[i] = 0.0;

    while (1) {

        arr[contador % MAX] = contador;

        printf("Contador: %d\n\n", contador);

        for (uint8_t i = 0; i < MAX; i++ ) {

            if ( contador < i) {


                //  (int)((MAX + contador - (i % MAX)))
                //  (int)(MAX + contador - (i % MAX)))
                //  (int)(MAX + contador - i)
                printf("arr[%d] = %d\t * filter[%d]\t IF\n", i, arr[i], (int)(MAX + contador - i));
            }

            else {
                
                printf("arr[%d] = %d\t * filter[%d]\t ELSE\n", i, arr[i], (int)((contador - i) % MAX));
            } 
        } 

        contador++; 

        printf("\n");
        
        scanf("%hhd", &tmp);
    }

    return;
}