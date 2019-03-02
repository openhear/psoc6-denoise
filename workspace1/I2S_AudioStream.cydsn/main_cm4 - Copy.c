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
    //uint16_t xL, xR;
    uint32_t xL, xR;
    float32_t xl;
    float32_t xr;
    
    // Get L/R audio sample
    //sample.uint32bit = I2S_ReadRxData();
    xL = I2S_ReadRxData();
    xR = I2S_ReadRxData();
    
    // LEFT = 1, RIGHT = 0
    //xl = (float32_t) sample.uint32bit[LEFT];
    //xr = (float32_t) sample.uint16bit[RIGHT];
    xl = (float32_t) xL;
    xr = (float32_t) xR;
    
    // Processing
    
    
    // Return L/R samples
    xL = (uint32_t) ( 16 * xl);
    xR = (uint32_t) ( 16 * xr);
    //sample.uint32bit[LEFT] = xL;
    //sample.uint32bit[RIGHT] = xR;
    //I2S_WriteTxData(sample.uint32bit);
     I2S_WriteTxData(xL);
     I2S_WriteTxData(xR);
    
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
