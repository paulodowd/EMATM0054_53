/**
 * An example of receiving a string from a connected
 * Pololu 3Pi+ robot, and then processing this visualise
 * what is happening.  This code is expecting comma
 * seperated values concerning the magnetometer.
 */


import processing.serial.*;

Serial myPort;  // Create object from Serial class
String str;      // Data received from the serial port

float angle = 0.0, mag = 0.0;
float y_mag = 0.0, x_mag = 0.0;

float draw_scale_up = 20;

void setup()
{
  size(500, 500);

  // I know that the first port in the serial list on my mac
  // is always my  FTDI adaptor, so I open Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  println( Serial.list() );
  delay(1000);

  // You will need to change [1] to pick the correct port
  // on your computer.
  String portName = Serial.list()[1];
  myPort = new Serial(this, portName, 9600);
}

void draw() {

  // Clear the screen of the last drawing.
  background(0);

  // Move the canvas, so x=0 and y=0 is in the
  // centre of the application window.
  pushMatrix();
  translate(width/2, height/2);


  // Check for new data
  if ( myPort.available() > 0) {  // If data is available,

    str = myPort.readString();         // read it and store

    // so we can debug the actual values.
    println( str );

    // Split by comma
    String[] sub = split( str, ',' );

    // Catching different string structures sent from the 3Pi+
    switch( sub.length ) {
    case 1: // Assumes you're sending <angle>\n

      mag = 10.0;
      angle = parseFloat( sub[0] );
      break;

    case 2: // Assumes you're sending <angle>,<magnitude>\n

      angle = parseFloat( sub[0] );
      mag = parseFloat( sub[1] );
      break;

    case 4: // assumes you're sending <angle>,<magnitude>,<x>,<y>\n

      angle = parseFloat( sub[0] ) - (PI/2);
      mag = parseFloat( sub[1] );

      // magnetometer axis readings      
      x_mag = parseFloat( sub[2] ) * draw_scale_up;
      y_mag = parseFloat( sub[3] ) * draw_scale_up;


      break;

    default:
      break;
    }
  }



  // If we captured just x axis magnetometer reading
  strokeWeight(3);
  stroke(0, 255, 0);
  line( 0, 0, x_mag, 0 );

  // If we captured just y axis magnetometer reading
  strokeWeight(3);
  stroke(255, 0, 0);
  line( 0, 0, 0, y_mag);

  // Plot where the magnet is as a result of angle
  // and magnitude
  float y = mag * cos(angle) * draw_scale_up;
  float x = mag * sin(angle) * draw_scale_up;

  // Magnet position
  stroke(1);
  fill(0, 255, 0);
  ellipse(x, y, 20, 20);

  // axis origin
  fill( 255 );
  ellipse(0, 0, 10, 10);

  // Line to magnet position
  stroke(255);
  line( 0, 0, x, y);

  popMatrix();
}
