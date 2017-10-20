#include "DigiKeyboard.h"
#include <stdint.h>

/* 
    RST  = PB5
    MISO = PB1
    SCK  = PB2
    MOSI = PB0
*/
uint8_t pins[4] = {5,1,2,0};

uint8_t scan_patterns[12][4] = 
{
    // Order is as it is in the schematic: RST, MISO, SCK, MOSI
    // 0 = low, 1 = high, 2 = high Z, 3 = input-pullup
    {0, 3, 2, 2}, // Switch 1
    {2, 0, 3, 2}, // Switch 2
    {2, 2, 0, 3}, // Switch 3
    {0, 2, 3, 2}, // Switch 4
    {0, 2, 2, 3}, // Switch 5
    {2, 0, 2, 3}, // Switch 6
    {1, 0, 2, 2}, // LED 1
    {2, 1, 0, 2}, // LED 2
    {2, 2, 1, 0}, // LED 3
    {1, 2, 0, 2}, // LED 4
    {1, 2, 2, 0}, // LED 5
    {2, 1, 2, 0}  // LED 6
};

#define DEBOUNCE_BITS 3

boolean LED_states[6] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t switch_states[6] = {0, 0, 0, 0, 0, 0, 0, 0};
boolean debounced_states[6] = {0, 0, 0, 0, 0, 0, 0, 0};
boolean last_debounced_states[6] = {0, 0, 0, 0, 0, 0, 0, 0};
uint8_t debounce_mask = 0;

uint8_t current_input = 1; // Valid range [0-3]

void setup() 
{
    for(int i=0; i<DEBOUNCE_BITS; i++)
    {
        debounce_mask = (debounce_mask << 1) | 1;
    }
}


void loop() 
{
  // this is generally not necessary but with some older systems it seems to
  // prevent missing the first character after a delay:
  DigiKeyboard.sendKeyStroke(0);
  
  // Type out this string letter by letter on the computer (assumes US-style
  // keyboard)
  DigiKeyboard.println("Hello Digispark!");
  
  scanMatrix();
  debounce();

  for(int i=0; i<6; i++)
  {
      if(debounced_states[i] != last_debounced_states[i])
      {
          last_debounced_states[i] = debounced_states[i];
          if(debounced_states[i])
          {
              sendKeys(debounced_states[i]);
          }
          else
          {
              sendKeys(debounced_states[i]+6);
          }
      }
  }
}

void sendKeys(uint8_t key)
{
    switch(key)
    {
        case 0:
            // Whatever you want switch 0 to do when pressed
            break;

        case 1:
            // Whatever you want switch 1 to do when pressed
            break;

        case 2:
            // Whatever you want switch 2 to do when pressed
            break;

        case 4:
            // Whatever you want switch 3 to do when pressed
            break;

        case 5:
            // Whatever you want switch 4 to do when pressed
            break;

        
        case 6:
            // Whatever you want switch 0 to do when released
            break;

        case 7:
            // Whatever you want switch 0 to do when released
            break;

        case 8:
            // Whatever you want switch 0 to do when released
            break;

        case 9:
            // Whatever you want switch 0 to do when released
            break;

        case 10:
            // Whatever you want switch 0 to do when released
            break;

        case 11:
            // Whatever you want switch 0 to do when released
            break;

        case 12:
            // Whatever you want switch 0 to do when released
            break;
    }
}

void set_pin(uint8_t pin, uint8_t state)
{
    // Set pin either to low, high, highZ or input-pullup
    /*
        0 = low
        1 = high
        2 = highZ
        3 = input-pullup
    */
    switch(state)
    {
        case 0: // Set pin to low output
            bitClear(PORTB, pin);
            bitSet(DDRB, pin);   
            break;
        case 1: // Set pin to high output
            bitSet(PORTB, pin);   
            bitSet(DDRB, pin);
            break;
        case 2: // Set pin to input (highZ)
            bitClear(DDRB, pin);  
            bitClear(PORTB, pin);
            break;
        case 3: // Set pin to input with pullup
            bitClear(DDRB, pin);
            bitSet(PORTB, pin);
            current_input = pin; // Remember which pin is set up for input
            break;       
    }    
}

void debounce()
{

    for(int i=0; i<6; i++)
    {
        debounced_states[i] = ((switch_states[i] & debounce_mask) == debounce_mask);
    }

}

void scanMatrix()
{
    for(uint8_t i=0; i<6; i++) // Loop through the six switches
    {
        for(uint8_t k=0; k<4; k++) // Set up the pins to read the current switch
        {
            set_pin(pins[k], scan_patterns[i][k]);
        }

        // Shift current 8bit char left once, store new reading in LSB
        // This gives us an 8 scan history
        switch_states[i] = (switch_states[i] << 1) | bitRead(PINB, current_input);
    }

    for(uint8_t i=6; i<12; i++) // Loop through the six LEDs
    {
        if(LED_states[i-6]) // If the LED is wanted on, turn it on
        {
            for(uint8_t k=0; k<4; k++)
            {
                set_pin(pins[k], scan_patterns[i][k])
            }
        }
        else // Turn it off by setting all pins HighZ
        {
            for(uint8_t k=0; k<4; k++)
            {
                set_pin(pins[k], 2);
            }
        }
        // LEDs need to be on for a while for people to see it:
        DigiKeyboard.delay(2);
        // Going to experiment with strobing the LEDs if this delay causes issues
    }
}
