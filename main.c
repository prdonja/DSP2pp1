//////////////////////////////////////////////////////////////////////////////
// *
// * Predmetni projekat iz predmeta OAiS DSP 2
// * Godina: 2017
// *
// * Zadatak: Ekvalizacija audio signala
// * Autor:
// *                                                                          
// *                                                                          
/////////////////////////////////////////////////////////////////////////////

#include "stdio.h"
#include "ezdsp5535.h"
#include "ezdsp5535_i2c.h"
#include "aic3204.h"
#include "ezdsp5535_aic3204_dma.h"
#include "ezdsp5535_i2s.h"
#include "ezdsp5535_sar.h"
#include "print_number.h"
#include "math.h"

#include "iir.h"
#include "processing.h"

/* Frekvencija odabiranja */
#define SAMPLE_RATE 8000L

#define PI 3.14159265

/* Niz za smestanje ulaznih i izlaznih odbiraka */
#pragma DATA_ALIGN(sampleBufferL,4)
Int16 sampleBufferL[AUDIO_IO_SIZE];
#pragma DATA_ALIGN(sampleBufferR,4)
Int16 sampleBufferR[AUDIO_IO_SIZE];

Int16 dirak[AUDIO_IO_SIZE];

Int16 outputShelvLp[4];
Int16 outputShelvHp[4];
Int16 outputPeek1[6];
Int16 outputPeek2[6];

Int16 x_historyLPLeft[1] = {0};
Int16 y_historyLPLeft[1] = {0};
Int16 x_historyHPLeft[1] = {0};
Int16 y_historyHPLeft[1] = {0};
Int16 x_historyPeek1Left[2] = {0,0};
Int16 y_historyPeek1Left[2] = {0,0};
Int16 x_historyPeek2Left[2] = {0,0};
Int16 y_historyPeek2Left[2] = {0,0};

Int16 x_historyLPRight[1] = {0};
Int16 y_historyLPRight[1] = {0};
Int16 x_historyHPRight[1] = {0};
Int16 y_historyHPRight[1] = {0};
Int16 x_historyPeek1Right[2] = {0,0};
Int16 y_historyPeek1Right[2] = {0,0};
Int16 x_historyPeek2Right[2] = {0,0};
Int16 y_historyPeek2Right[2] = {0,0};



Int16 bufferLeft[AUDIO_IO_SIZE];
Int16 bufferRight[AUDIO_IO_SIZE];


float alp[6] = {0.939062505885046, 0.854080685618676, 0.783571847537077, 0.0392901078615617};
float beta[3] = {0.963513756825621, 0.534167930122268};

float f[6] = {160, 690, 400, 2565, 615, 3800};

Int16 k[4] = { 16000, 16000, 16000, 16000 };
int k_i = 0;


Uint16 getKey() {
    static Uint16 old = NoKey;
    Uint16 key = EZDSP5535_SAR_getKey();
    if (key == old) {
        return NoKey;
    } else {
        old = key;
        return key;
    }
}

void printNewValue() {
	setWritePointerToFirstChar();
	/* Indeks broja k */
	printChar('0' + k_i);
	printChar(' ');
	/* Vrednost broja k */
	if (k[k_i] == 32767) {
		printChar('1');
		printChar('.');
		printChar('0');
	} else {
		printChar('0');
		printChar('.');
		printChar('1' + k[k_i] / 3277);
	}
}

void main( void )
{   
	int i = 0;

    /* Inicijalizaija razvojne ploce */
    EZDSP5535_init( );

    /* Inicijalizacija kontrolera za ocitavanje vrednosti pritisnutog dugmeta*/
    EZDSP5535_SAR_init();

    /* Inicijalizacija LCD kontrolera */
    initPrintNumber();

	printf("\n Ekvalizacija audio signala \n");
		
    /* Inicijalizacija veze sa AIC3204 kodekom (AD/DA) */
    aic3204_hardware_init();
	
    /* Inicijalizacija AIC3204 kodeka */
	aic3204_init();

    aic3204_dma_init();
    

    /* Postavljanje vrednosti frekvencije odabiranja i pojacanja na kodeku */
    set_sampling_frequency_and_gain(SAMPLE_RATE, 0);

    dirak[0] = 16000;
    for(i = 1; i < AUDIO_IO_SIZE; i++)
    {
    	dirak[i] = 0;
    }

    calculateShelvingCoeff(alp[3], outputShelvHp);
    calculateShelvingCoeff(alp[0], outputShelvLp);
    calculatePeekCoeff(alp[1], beta[0], outputPeek1);
    calculatePeekCoeff(alp[2], beta[1], outputPeek2);

    clearLCD();

    while(1)
    {
    	aic3204_read_block(sampleBufferL, sampleBufferR);

    	Uint16 key = getKey();

		switch (key)
		{
			case SW1:
				(k_i + 1 == 4) ? k_i = 0 : ++k_i;
				printNewValue();
				break;
			case SW2:
				k[k_i] -= 3277;
				if (k[k_i] < 0) {
					k[k_i] = 32767;
				}
				printNewValue();
				break;
		}

    			setWritePointerToFirstChar();

    	/* Your code here */
    	for(i = 0; i < AUDIO_IO_SIZE; i++)
    	{

    		bufferLeft[i] = shelvingLP(dirak[i], outputShelvLp, x_historyLPLeft, y_historyLPLeft, k[0]);
			bufferLeft[i] = shelvingPeek(bufferLeft[i], outputPeek1, x_historyPeek1Left, y_historyPeek1Left, k[1]);
			bufferLeft[i] = shelvingPeek(bufferLeft[i], outputPeek2, x_historyPeek2Left, y_historyPeek2Left, k[2]);
			bufferLeft[i] = shelvingHP(bufferLeft[i], outputShelvHp, x_historyHPLeft, y_historyHPLeft, k[3]);

			bufferRight[i] = shelvingLP(dirak[i], outputShelvLp, x_historyLPRight, y_historyLPRight, k[0]);
			bufferRight[i] = shelvingPeek(bufferRight[i], outputPeek1, x_historyPeek1Right, y_historyPeek1Right, k[1]);
			bufferRight[i] = shelvingPeek(bufferRight[i], outputPeek2, x_historyPeek2Right, y_historyPeek2Right, k[2]);
			bufferRight[i] = shelvingHP(bufferRight[i], outputShelvHp, x_historyHPRight, y_historyHPRight, k[3]);
		}



		aic3204_write_block(sampleBufferR, sampleBufferR);
	}

    	
	/* Prekid veze sa AIC3204 kodekom */
    aic3204_disable();

    printf( "\n***Kraj programa***\n" );
	SW_BREAKPOINT;
}


