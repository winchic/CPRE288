/**
 * main.c
 */

#include "Timer.h"
#include "lcd.h"
#include "adc.h"
#include "tm4c123gh6pm.h" // For register definitions

// Initialize the ADC and configure PB4 (AIN10) as the analog input
void ADC0_init(void) {
    // Enable clock for ADC0 and Port B
    SYSCTL_RCGCADC_R |= 0x01;       // Enable the clock for ADC0 (bit 0)
    SYSCTL_RCGCGPIO_R |= 0x2;       // Enable clock for Port B (bit 1)

    // Configure PB4 for analog input
    GPIO_PORTB_AFSEL_R |= 0x10;     // Enable alternate function on PB4 (bit 4)
    GPIO_PORTB_DEN_R &= ~0x10;      // Disable digital function on PB4
    GPIO_PORTB_AMSEL_R |= 0x10;     // Enable analog function on PB4

    // Configure ADC0, Sample Sequencer 3
    ADC0_ACTSS_R &= ~0x8;           // Disable SS3 during configuration
    ADC0_EMUX_R &= ~0xF000;         // Set SS3 to use software trigger (clear bits 15-12)
    ADC0_SSMUX3_R = 10;             // Set SS3 to sample AIN10 (PB4)
    ADC0_SSCTL3_R = 0x06;           // Set end of sequence and interrupt flag for SS3
    ADC0_SAC_R = 0x4;               // Enable 16x hardware averaging
    ADC0_ACTSS_R |= 0x8;            // Enable SS3
}

// Trigger ADC conversion and get the result
int ADC0_InSeq3(void) {
    ADC0_PSSI_R = 0x8;               // Start conversion on SS3
    while ((ADC0_RIS_R & 0x8) == 0); // Wait for conversion to complete
    int result = ADC0_SSFIFO3_R & 0xFFF; // Read 12-bit result from SS3 FIFO
    ADC0_ISC_R = 0x8;                // Clear completion flag for SS3
    return result;                   // Return the result
}



//this is for part 2
// uncomment when need to demo
//
//
//int calculate_distance(int adc_value) {
//    // need to put a formula here, use calibration data from ir sensor
//    return (int)(12345 / (adc_value - 100));
//    // ^^ this is a placeholder
//
//}
//
//int adc_read_avg(void) {
//    int total = 0;
//    for (int i = 0; i < 16; i++) {
//        total += ADC0_InSeq3();
//    }
//    return total / 16;  // Average result
//}


int main(void) {
    // Initialize ADC and LCD
    ADC0_init();
    lcd_init();

    // Variable to hold ADC result
    int adc_result;
//    int distance; // uncomment when demoing part 2

    while (1) {
        // Trigger ADC conversion and get the result
        adc_result = ADC0_InSeq3();

        // distance stuff
        adc_result = adc_read_avg();
//        distance = calculate_distance(adc_result); // uncomment when demoing part 2

        // Display the result on the LCD
        lcd_clear();
        lcd_printf("ADC: %d", adc_result);

        //lcd_printf("ADC: %d\nDist: %d cm", adc_result, distance);  //uncomment when demoing part 2


        delay_ms(500);  // Delay to make the display readable
    }

    return 0;
}
