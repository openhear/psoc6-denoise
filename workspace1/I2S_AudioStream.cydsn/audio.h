/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef AUDIO_H
    #define AUDIO_H	
	
    #include <stdint.h> 
    
    #define LEFT 1
    #define RIGHT 0
    
    union sampleData
    {
        uint32_t uint32bit;
        uint16_t uint16bit[2];
    };
    
#endif /* #ifndef AUDIO_H */

/* [] END OF FILE */
