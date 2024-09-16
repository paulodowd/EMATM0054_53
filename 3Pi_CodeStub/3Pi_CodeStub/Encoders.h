/*
 * 
 * This code has been completed for you and should not be changed.
 * This code will be discussed in Labsheet 4.
 * 
 */

#ifndef _ENCODERS_H
#define _ENCODERS_H

#define ENCODER_0_A_PIN  7
#define ENCODER_0_B_PIN  23
#define ENCODER_1_A_PIN  26
//#define ENCODER_1_B_PIN Non-standard pin!


// Volatile Global variables used by Encoder ISR.
volatile long count_e0; // used by encoder to count the rotation
volatile byte state_e0; // used to determine quadrature state
volatile long count_e1;
volatile byte state_e1;


// This ISR handles just Encoder 1
// ISR to read the Encoder1 Channel A and B pins
// and then look up based on  transition what kind of
// rotation must have occured.
ISR( INT6_vect ) {
  // We know that the ISR is only called when a pin changes.
  // We also know only 1 pin can change at a time.
  // The XOR(AB) signal change from "Channel A" triggers ISR.

  // First, Read in the new state of the encoder pins.
  // Standard pins, so standard read functions.
  boolean e0_B = digitalRead( ENCODER_0_B_PIN ); // normal B state
  boolean e0_A = digitalRead( ENCODER_0_A_PIN ); // XOR(AB)

  // Software XOR (^) logically infers
  // the true value of A given the state of B
  e0_A = e0_A ^ e0_B;

  // Shift our (new) current readings into bit positions
  // 2 and 3 in the state variable (current state)
  // State: (bit3)  (bit2)  (bit1)   (bit0)
  // State:  new B   new A   old B   old A
  state_e0 = state_e0 | ( e0_B  << 3 );
  state_e0 = state_e0 | ( e0_A  << 2 );

  if ( state_e0 == 1 ) {
    count_e0--;
  } else if ( state_e0 == 2) {
    count_e0++;
  } else if ( state_e0 == 4) {
    count_e0++;
  } else if ( state_e0 == 7) {
    count_e0--;
  } else if ( state_e0 == 8) {
    count_e0--;
  } else if ( state_e0 == 11) {
    count_e0++;
  } else if ( state_e0 == 13) {
    count_e0++;
  } else if ( state_e0 == 14) {
    count_e0--;
  }

  // Shift the current readings (bits 3 and 2) down
  // into position 1 and 0 (to become prior readings)
  // This bumps bits 1 and 0 off to the right, "deleting"
  // them for the next ISR call.
  state_e0 = state_e0 >> 2;

}




// This ISR handles just Encoder 0
// ISR to read the Encoder0 Channel A and B pins
// and then look up based on  transition what kind of
// rotation must have occured.
ISR( PCINT0_vect ) {

  // First, Read in the new state of the encoder pins.

  // Mask for a specific pin from the port.
  // Non-standard pin, so we access the register
  // directly.
  // Reading just PINE would give us a number
  // composed of all 8 bits.  We want only bit 2.
  // B00000100 masks out all but bit 2
  // It is more portable to use the PINE2 keyword.
  boolean e1_B = PINE & (1 << PINE2);
  //boolean e1_B = PINE & B00000100;  // Does same as above.

  // Standard read from the other pin.
  boolean e1_A = digitalRead( ENCODER_1_A_PIN ); // 26 the same as A8


  e1_A = e1_A ^ e1_B;

  // Create a bitwise representation of our states
  // We do this by shifting the boolean value up by
  // the appropriate number of bits, as per our table
  // header:
  //
  // State :  (bit3)  (bit2)  (bit1)  (bit0)
  // State :  New A,  New B,  Old A,  Old B.
  state_e1 = state_e1 | ( e1_B  << 3 );
  state_e1 = state_e1 | ( e1_A  << 2 );


  // This is an inefficient way of determining
  // the direction.  However it illustrates well
  // against the lecture slides.
  if ( state_e1 == 1 ) {
    count_e1--;
  } else if ( state_e1 == 2) {
    count_e1++;
  } else if ( state_e1 == 4) {
    count_e1++;
  } else if ( state_e1 == 7) {
    count_e1--;
  } else if ( state_e1 == 8) {
    count_e1--;
  } else if ( state_e1 == 11) {
    count_e1++;
  } else if ( state_e1 == 13) {
    count_e1++;
  } else if ( state_e1 == 14) {
    count_e1--;
  }

  // Shift the current readings (bits 3 and 2) down
  // into position 1 and 0 (to become prior readings)
  // This bumps bits 1 and 0 off to the right, "deleting"
  // them for the next ISR call.
  state_e1 = state_e1 >> 2;
}


/*
   This setup routine enables interrupts for
   encoder1.  The interrupt is automatically
   triggered when one of the encoder pin changes.
   This is really convenient!  It means we don't
   have to check the encoder manually.
*/
void setupEncoder0()
{
  count_e0 = 0;

  // Setup pins for right encoder
  pinMode( ENCODER_0_A_PIN, INPUT );
  pinMode( ENCODER_0_B_PIN, INPUT );

  // initialise the recorded state of e0 encoder.
  state_e0 = 0;

  // Get initial state of encoder pins A + B
  boolean e0_A = digitalRead( ENCODER_0_A_PIN );
  boolean e0_B = digitalRead( ENCODER_0_B_PIN );
  e0_A = e0_A ^ e0_B;

  // Shift values into correct place in state.
  // Bits 1 and 0  are prior states.
  state_e0 = state_e0 | ( e0_B << 1 );
  state_e0 = state_e0 | ( e0_B << 0 );


  // Now to set up PE6 as an external interupt (INT6), which means it can
  // have its own dedicated ISR vector INT6_vector

  // Page 90, 11.1.3 External Interrupt Mask Register – EIMSK
  // Disable external interrupts for INT6 first
  // Set INT6 bit low, preserve other bits
  EIMSK = EIMSK & ~(1 << INT6);
  //EIMSK = EIMSK & B1011111; // Same as above.

  // Page 89, 11.1.2 External Interrupt Control Register B – EICRB
  // Used to set up INT6 interrupt
  EICRB |= ( 1 << ISC60 );  // using header file names, push 1 to bit ISC60
  //EICRB |= B00010000; // does same as above

  // Page 90, 11.1.4 External Interrupt Flag Register – EIFR
  // Setting a 1 in bit 6 (INTF6) clears the interrupt flag.
  EIFR |= ( 1 << INTF6 );
  //EIFR |= B01000000;  // same as above

  // Now that we have set INT6 interrupt up, we can enable
  // the interrupt to happen
  // Page 90, 11.1.3 External Interrupt Mask Register – EIMSK
  // Disable external interrupts for INT6 first
  // Set INT6 bit high, preserve other bits
  EIMSK |= ( 1 << INT6 );
  //EIMSK |= B01000000; // Same as above

}


void setupEncoder1(){

  count_e1 = 0;

  // Setting up left encoder:
  // The 3Pi board uses the pin PE2 (port E, pin 2) which is
  // very unconventional.  It doesn't have a standard
  // arduino alias (like d6, or a5, for example).
  // We set it up here with direct register access
  // Writing a 0 to a DDR sets as input
  // DDRE = Data Direction Register (Port)E
  // We want pin PE2, which means bit 2 (counting from 0)
  // PE Register bits [ 7  6  5  4  3  2  1  0 ]
  // Binary mask      [ 1  1  1  1  1  0  1  1 ]
  //
  // By performing an & here, the 0 sets low, all 1's preserve
  // any previous state.
  DDRE = DDRE & ~(1 << DDE6);
  //DDRE = DDRE & B11111011; // Same as above.

  // We need to enable the pull up resistor for the pin
  // To do this, once a pin is set to input (as above)
  // You write a 1 to the bit in the output register
  PORTE = PORTE | (1 << PORTE2 );
  //PORTE = PORTE | 0B00000100;

  // Encoder1 uses conventional pin 26
  pinMode( ENCODER_1_A_PIN, INPUT );
  digitalWrite( ENCODER_1_A_PIN, HIGH ); // Encoder 1 xor

  // initialise the recorded state of e1 encoder.
  state_e1 = 0;

  // Get initial state of encoder.
  boolean e1_B = PINE & (1 << PINE2);
  //boolean e1_B = PINE & B00000100;  // Does same as above.

  // Standard read from the other pin.
  boolean e1_A = digitalRead( ENCODER_1_A_PIN ); // 26 the same as A8

  // Some clever electronics combines the
  // signals and this XOR restores the
  // true value.
  e1_A = e1_A ^ e1_B;


  // Enable pin-change interrupt on A8 (PB4) for encoder0, and disable other
  // pin-change interrupts.
  // Note, this register will normally create an interrupt a change to any pins
  // on the port, but we use PCMSK0 to set it only for PCINT4 which is A8 (PB4)
  // When we set these registers, the compiler will now look for a routine called
  // ISR( PCINT0_vect ) when it detects a change on the pin.  PCINT0 seems like a
  // mismatch to PCINT4, however there is only the one vector servicing a change
  // to all PCINT0->7 pins.
  // See Manual 11.1.5 Pin Change Interrupt Control Register - PCICR

  // Page 91, 11.1.5, Pin Change Interrupt Control Register
  // Disable interrupt first
  PCICR = PCICR & ~( 1 << PCIE0 );
  // PCICR &= B11111110;  // Same as above

  // 11.1.7 Pin Change Mask Register 0 – PCMSK0
  PCMSK0 |= (1 << PCINT4);

  // Page 91, 11.1.6 Pin Change Interrupt Flag Register – PCIFR
  PCIFR |= (1 << PCIF0);  // Clear its interrupt flag by writing a 1.

  // Enable
  PCICR |= (1 << PCIE0);
}

#endif
