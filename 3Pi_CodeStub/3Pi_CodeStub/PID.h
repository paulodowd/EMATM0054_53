/***************************************
   ,        .       .           .      , 
   |        |       |           |     '| 
   |    ,-: |-. ,-. |-. ,-. ,-. |-     | 
   |    | | | | `-. | | |-' |-' |      | 
   `--' `-` `-' `-' ' ' `-' `-' `-'    ' 
 A D V A N C E D   E X E R C I S E S
****************************************/

// this #ifndef stops this file
// from being included mored than
// once by the compiler. 
#ifndef _PID_H
#define _PID_H

// Class to contain generic PID algorithm.
class PID_c {
  public:

    // PID update variables.
    float last_error;
    float p_term;
    float i_term;
    float d_term;
    float i_sum;
    float feedback;

    // To store gains.
    float p_gain;
    float i_gain;
    float d_gain;

    // To determine time elapsed.
    unsigned long ms_last_t;
  
    // Constructor, must exist.
    PID_c() {
      // leaving this empty.
      // ensure initialise() is called
      // instead.
    } 

    // To setup PID.
    // Pass in gain values for kp, ki, kd.
    void initialise( float p, float i, float d ) {
       feedback = 0;
       last_error = 0;
       p_term = 0;
       i_term = 0;
       d_term = 0;
       i_sum = 0;

       p_gain = p;
       i_gain = i;
       d_gain = d;

       ms_last_t = millis();
    }

    // If PID hasn't been called in a long time,
    // the time elapsed will be huge and cause a
    // massive overshoot.  It is best to reset if
    // we know the robot has been delayed or motors
    // disabled.
    void reset() {
      p_term = 0;
      i_term = 0;
      d_term = 0;
      i_sum = 0;
      last_error = 0;
      feedback = 0;
      ms_last_t = millis();
    }

    // To imlpement this on a microcontroller, this is a good
    // referenece:
    // https://en.wikipedia.org/wiki/PID_controller#Discrete_implementation
    float update( float demand, float measurement ) {
      float error;
      unsigned long ms_now_t;
      unsigned long ms_dt;
      float float_dt;
      float diff_error;

      // Grab time to calc elapsed time.
      ms_now_t = millis();
      ms_dt = ms_now_t - ms_last_t;

      // ms_last_t has been used, so update
      // it for the next call of this update.
      ms_last_t = millis();
      
      // typecasting the different of two
      // unsigned longs is safer.
      float_dt = (float)ms_dt;

      // NOTE: A serious error can occur
      // here if dt is 0, this causes divide
      // by zero errors.  This can happen if
      // PID.update() is called faster than 1ms.
      // Here, we catch the error by returning
      // the last feedback value.
      if( float_dt == 0.0 ) return feedback;

      // Calculate error signal.
      error = demand - measurement;
      last_error = error;

      // P term, nice and easy
      p_term = p_gain * error;

      // discrete integration
      i_sum = i_sum + (error * float_dt);

      // i_term.
      i_term = i_gain * i_sum;
      
      // d_term.
      // Note, sometimes this needs to be inverted
      // to last_error - error.  It depends on the
      // error signal sign in relation to the system.
      diff_error = (error - last_error) / float_dt;
      d_term = diff_error * d_gain;

      // Sometimes this needs to be - d_term.
      // d_term should counteract sudden changes, which
      // means it is subtractive.  Another way to achieve
      // this is to have a negative gain.  The best way 
      // to troubleshoot is to plot what it is doing.
      feedback = p_term + i_term + d_term;

      // done
      return feedback;
    }

}; // End of PID_c class definition



#endif
