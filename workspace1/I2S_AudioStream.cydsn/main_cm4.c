/* ========================================
 * I2S Pmod AudioStream Project
 *
 * Project: I2S_AudioStream
 * By: Arthur Salvetti
 * Date Modified: 2/12/2019
 *
 * ========================================
*/

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "project.h"
#include "audio.h"
#include "rnnoise.h"

#define FRAME_SIZE 480
#define MAX_INDEX 479
#define NUM_BUFFERS 12
#define INC_BUFFER(index)   { index = ((index + 1) % NUM_BUFFERS); }

static DenoiseState *st;
static float ringBuffer[NUM_BUFFERS][FRAME_SIZE];
static int ringBufferProcess = 0;
static int ringBufferWrite = 0;
static int ringBufferOut = 0;
static int ringBufferReadyOut = -1;
static int starting = 1;
static const int stereo = 0;
static int debugCount = 0;

void I2S_isr_Handler(void)
{   
    static int32_t xL, xR;
    static int fillindex = 0;
    static int drainindex = 0;
    // float32_t xl;
    // float32_t xr;
    // Get L/R audio sample
    xL = (int32_t) I2S_ReadRxData() << 8;  // data in is actually 24-bit, "right-justified"
    xR = (int32_t) I2S_ReadRxData() << 8;  // you have to shift it all the way to the left
    //xL = xL >> 8;    // option 1: you can shift it back to the right here, or at the end 
    //xR = xR >> 8;    // this shift will extend the sign "if" the operand is signed 
    // LEFT = 1, RIGHT = 0
    // xl = (float32_t) xL;
    // xr = (float32_t) xR;
    ringBuffer[ringBufferWrite][fillindex] = (float)xL;
    if (stereo) {
        ringBuffer[ringBufferWrite][fillindex + 1] = (float)xR;
    }
    
    // increment buffer indexes
    fillindex += 1 + stereo;
    if (fillindex > MAX_INDEX) {
        fillindex = 0;
        INC_BUFFER(ringBufferWrite);
        if (ringBufferWrite == 0) {
            Cy_GPIO_Inv(BlueLed_PORT, BlueLed_NUM);
        }
    }
    
    
    // Add your Processing here
    // xl = (float32_t)-2.1 * xl;
    // xr = (float32_t) 0.4 * xr;

    
    
    // Return L/R audio samples
    // just trust that processing will keep up
    if (ringBufferReadyOut >=0) {
          
        xL = (int)ringBuffer[ringBufferOut][drainindex] >> 8;
        xR = (int)ringBuffer[ringBufferOut][drainindex + stereo] >> 8;
    
        // increment buffer indexes
        drainindex += 1 + stereo;
        if (drainindex > MAX_INDEX) {
            drainindex = 0;
            INC_BUFFER(ringBufferOut);
        }
        I2S_WriteTxData(xL);
        I2S_WriteTxData(xR);
    } else {
        //debug
        I2S_WriteTxData(0); // xL >> 8);
        I2S_WriteTxData(0); // xR >> 8);
    }
    // Clear I2S Interrupt
    I2S_ClearInterrupt(I2S_INTR_TX_TRIGGER_Msk);
    
    
  
}


int main(void)
{
    starting = 1;
    st = rnnoise_create();
    ringBufferProcess = ringBufferWrite;
    ringBufferOut = 0;
    ringBufferReadyOut = -1;
    volatile uint counter = 0;
    printf("starting");
    // int a = CY_SYSCLK_BAD_PARAM;
    // volatile cy_en_systick_clock_source_t tickclk = Cy_SysTick_GetClockSource();
    // RTC
    
    // Cy_SysClk_StartClkMeasurementCounters();
     /* Initialize the I2S interrupt */
    Cy_SysInt_Init(&I2S_isr_cfg, I2S_isr_Handler);
    NVIC_EnableIRQ(I2S_isr_cfg.intrSrc);
    
    /* Enable global interrupts. */
    __enable_irq();
        
    /* Wait for the MCLK to clock the Audio Codec */
    CyDelay(1);
    
    /* Start the I2S interface */
    I2S_Start();
    
    while(1)
    {
        debugCount += 1;
        // let ringBufferProcess get two ahead
        if (starting && ringBufferWrite > 3) {
            starting = 0;    
        }
        // then let it catch up with processing the audio buffers
        if ((ringBufferProcess != ringBufferWrite) && starting == 0) {
            // __disable_irq();
            Cy_TCPWM_Counter_Init(Counter_1_HW, Counter_1_CNT_NUM, &Counter_1_config);
            Cy_TCPWM_Enable_Multiple(Counter_1_HW, Counter_1_CNT_MASK); 
           
            Cy_TCPWM_TriggerStart(Counter_1_HW, Counter_1_CNT_MASK); 
            counter = Cy_TCPWM_Counter_GetCounter(Counter_1_HW, Counter_1_CNT_NUM);
            
            rnnoise_process_frame(st, ringBuffer[ringBufferProcess], ringBuffer[ringBufferProcess]);
            
            counter = Cy_TCPWM_Counter_GetCounter(Counter_1_HW, Counter_1_CNT_NUM);
            // __enable_irq();
            ringBufferReadyOut = ringBufferProcess;
            INC_BUFFER(ringBufferProcess);
            if (ringBufferProcess == 0) {
                Cy_GPIO_Inv(RedLed_PORT, RedLed_NUM);
            }
        } 
    /* Check if the I2S ISR is disabled */
        if (NVIC_GetEnableIRQ(I2S_isr_cfg.intrSrc))
        {
            /* I2S is enabled, do not do anything */
        }
        else /* I2S is disabled, re-start the wave buffer */
        {    
            /* Re-enabled the I2S ISR */
            NVIC_EnableIRQ(I2S_isr_cfg.intrSrc);
        }
        
        Cy_SysLib_DelayUs(100);
    }
}

/* [] END OF FILE */
