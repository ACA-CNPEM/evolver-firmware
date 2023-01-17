/* COMMANDS:
  Recurring Command: 'od_135r,1000,_!'
  Immediate Command: 'od_135i,1000,_!'
  Acknowledgement to Run: 'od_135a,1000,_!'

  Recurring Command: 'od_ledr,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,_!"
  Immediate Command: 'od_ledi,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,_!"
  Acknowledgement to Run: "od_leda,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,_!"
*/


/* LIBRARIES */
#include <evolver_si.h>
#include <Tlc5940.h>


/* VARIABLES */
// Serial event variables
String input_string = "";
boolean string_complete = false;
boolean serial_available = true;

// Echo response variables
String comma = ",";
String end_mark = "end";
String photodiode_address = "od_135";
String led_address = "od_led";

// Input variables
int num_vials = 16;
int expected_PDinputs = 2;
evolver_si in("od_90", "_!", expected_PDinputs); //2 CSV Inputs from RPI
evolver_si led("od_led", "_!", num_vials+1); // 17 CSV-inputs from RPI
boolean new_PDinput = false;
int saved_PDaveraged = 1000; // saved input from Serial Comm.
boolean new_LEDinput = false;
int saved_LEDinputs[] = {4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095};

// Mux Shield Components and Control Pins
int s0 = 7, s1 = 8, s2 = 9, s3 = 10, SIG_pin = A0;
int num_vials = 16;
int mux_readings[16]; // The size Assumes number of vials
int active_vial = 0;
int PDtimes_averaged = 1000;
int output[] = {60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000};
int Input[] = {4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095};




void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
