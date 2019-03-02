/* ========================================
 * I2S Pmod AudioStream Project
 *
 * Project: I2S_AudioStream
 * By: Jared Backofen
 * Date Modified: 1/24/2019
 *
 * ========================================
*/

#include "project.h"
#include "audio.h"


void I2S_isr_Handler(void)
{   
    /////////////////////////UNION TEST////////////////////
    
    union sampleData sample;
    uint16_t xL, xR;
    float32_t xl;
    float32_t xr;
    
    // Get L/R audio sample
    sample.uint32bit = I2S_ReadRxData();
    
    // LEFT = 1, RIGHT = 0
    xl = (float32_t) sample.uint16bit[LEFT];
    xr = (float32_t) sample.uint16bit[RIGHT];
    
    // Processing
    
    
    // Return L/R samples
    xL = (uint16_t) ( 2 * xl);
    xR = (uint16_t) ( 2 * xr);
    sample.uint16bit[LEFT] = xL;
    sample.uint16bit[RIGHT] = xR;
    I2S_WriteTxData(sample.uint32bit);
    
    ///////////////////////////////////////////////////
    

    /* Clear I2S Interrupt */
    I2S_ClearInterrupt(I2S_INTR_TX_TRIGGER_Msk);
}


int main(void)
{
    
     /* Initialize the I2S interrupt */
    Cy_SysInt_Init(&I2S_isr_cfg, I2S_isr_Handler);
    NVIC_EnableIRQ(I2S_isr_cfg.intrSrc);
 
    /* Enable global interrupts. */
    __enable_irq();
        
    /* Wait for the MCLK to clock the Audio Codec */
    CyDelay(1);
    
    /* Start the I2S interface */
    I2S_Start();

    for(;;)
    {
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
    }
}

/* [] END OF FILE */
