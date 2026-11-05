/* REFERENCIA, no es código del firmware: senoide por DAC + DMA con una LLI circular.
 * Copia sin cambios del ejemplo main_dac_sine_dma, solo con este comentario agregado.
 * Ojo: en confDMA(), dstMemAddr = (uint32_t)LPC_DAC->DACR lee el valor del registro
 * y no su dirección. En M2P el driver lo ignora, pero no hay que copiarlo. */
#include "lpc17xx_gpdma.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_clkpwr.h"

#define NUM_SINE_SAMPLE ((uint32_t)60)
#define SINE_FREQ_IN_HZ ((uint32_t)50)
#define DMA_SIZE        NUM_SINE_SAMPLE
#define DMA_CHANNEL     GPDMA_CH_0

#define DAC_MID         ((uint32_t)512)
#define DAC_MAX         ((uint32_t)1023)
#define SINE_SCALE      ((uint32_t)10000)

static uint32_t dacSineLut[NUM_SINE_SAMPLE];

static GPDMA_LLI_T dmaLLI;

static void buildSineLut(void);
static void confDac(void);
static void confDMA(void);

int main(void) {

	buildSineLut();
	confDac();
	confDMA();
	GPDMA_ChannelStart(GPDMA_CH_0);

    while(1) {
        __WFI();
    }
    return 0 ;
}

static void buildSineLut(void) {
    static const uint32_t sin0to90[16] = {
        0,    1045, 2079, 3090, 4067, 5000, 5877, 6691,
        7431, 8090, 8660, 9135, 9510, 9781, 9945, 10000
    };

    for (uint32_t i = 0; i < NUM_SINE_SAMPLE; i++) {
        uint32_t value;

        if (i <= 15) {
            value = DAC_MID + DAC_MID * sin0to90[i] / SINE_SCALE;
        } else if (i <= 30) {
            value = DAC_MID + DAC_MID * sin0to90[30 - i] / SINE_SCALE;
        } else if (i <= 45) {
            value = DAC_MID - DAC_MID * sin0to90[i - 30] / SINE_SCALE;
        } else {
            value = DAC_MID - DAC_MID * sin0to90[60 - i] / SINE_SCALE;
        }

        if (value > DAC_MAX) {
            value = DAC_MAX;
        }

        dacSineLut[i] = DAC_VALUE(value);
    }
}

void confDac(void) {
	DAC_Init();
	const uint32_t pclkDac 		= CLKPWR_GetPCLK(CLKPWR_PCLKSEL_DAC);
	DAC_SetDMATimeOut((uint16_t)(pclkDac / (SINE_FREQ_IN_HZ * NUM_SINE_SAMPLE)));
	DAC_CONVERTER_CFG_T dacCfg 	= {ENABLE,ENABLE,ENABLE};
	DAC_ConfigDAConverterControl(&dacCfg);
}

void confDMA(void) {
	dmaLLI.srcAddr = (uint32_t)dacSineLut;
	dmaLLI.dstAddr = (uint32_t)&LPC_DAC->DACR;
	dmaLLI.control = DMA_SIZE
					| GPDMA_BSIZE_1 << 12
					| GPDMA_BSIZE_1 << 15
					| GPDMA_WORD 	<< 18
					| GPDMA_WORD 	<< 21
					| 1 			<< 26;

	dmaLLI.nextLLI = (uint32_t)&dmaLLI;

	GPDMA_Init();

	GPDMA_Channel_CFG_T dmaCfg 	= {0};
	dmaCfg.channelNum 			= GPDMA_CH_0;
	dmaCfg.transferSize 		= DMA_SIZE;
	dmaCfg.type 				= GPDMA_M2P;
	dmaCfg.srcMemAddr 			= (uint32_t)dacSineLut;
	dmaCfg.dstMemAddr 			= (uint32_t)LPC_DAC->DACR;
	dmaCfg.dstConn 				= GPDMA_DAC;
	dmaCfg.linkedList 			= (uint32_t)&dmaLLI;


	GPDMA_SetupChannel(&dmaCfg);

}
