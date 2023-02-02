/* COMMANDS:
  Recurring Command: 'od_135r,1000,_!'
  Immediate Command: 'od_135i,1000,_!'
  Acknowledgement to Run: 'od_135a,1000,_!'

  Recurring Command: 'od_ledr,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,_!"
  Immediate Command: 'od_ledi,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,_!"
  Acknowledgement to Run: "od_leda,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,_!"
*/


/* LIBRARIES */
//#include <Arduino.h>
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
String pd_address = "od_135";
String led_address = "od_led";

// Input variables
int num_vials = 16;
int expected_pd = 1;

evolver_si pd_si("od_135", "_!", expected_pd + 1); // 2 CSV Inputs from RPI
evolver_si led_si("od_led", "_!", num_vials + 1); // 17 CSV-inputs from RPI

boolean new_pd_input = false;
boolean new_led_input = false;

int saved_pd_avg = 1000;
int saved_led_inputs[] = {4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095};

// Mux control pin variables; for the ADC board
int m0 = 7, m1 = 8, m2 = 9, m3 = 10, m_out = A0;
int active_vial = 0;
int pd_avg_of = 1000;
int output[] = {60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000,60000};
int Input[] = {4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095};



/********************************** SETUP **********************************/
void setup() {
  SerialUSB.begin(9600);
  Serial1.begin(9600);
  while (!Serial1);

  // Communication 
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  input_string.reserve(1000); // reserve 1000 bytes for the input_string

  // ADC control pins
  pinMode(m0, OUTPUT);
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);

  digitalWrite(m0, LOW);
  digitalWrite(m1, LOW);
  digitalWrite(m2, LOW);
  digitalWrite(m3, LOW);

  analogReadResolution(16);

  // PWM control object
  Tlc.init(LEFT_PWM, 4095);

  for (int i = 0; i < num_vials; i++){
    Tlc.set(LEFT_PWM, i, 4095 - Input[i]);
  }

  while(Tlc.update());
}



/********************************** LOOP **********************************/
void loop(){
  readMuxShield();

  if (string_complete){
    pd_si.analyzeAndCheck(input_string);
    led_si.analyzeAndCheck(input_string);

    // Photodiode logic
    if (pd_si.address_found){

      // Instant or recurring command 
      if (pd_si.input_array[0] == "i" || pd_si.input_array[0] == "r"){
        SerialUSB.println("Saving PD Settings...");
        saved_pd_avg = pd_si.input_array[1].toInt();

        SerialUSB.println("Broadcasting new PD command response: ");
        broadcastResponse();

        SerialUSB.println("Waiting for OK to execute...\n");
        new_pd_input = true;
      }

      // Acknoledgment to run command
      if (pd_si.input_array[0] == "a" && new_pd_input){
        pd_avg_of = saved_pd_avg;
        SerialUSB.println("PD Command Executed!");
        new_pd_input = false;
      }

      pd_si.address_found = false;
      input_string = "";
    }

    // LED logic
    if (led_si.address_found){

      // Instant or recurring command      
      if (led_si.input_array[0] == "i" || led_si.input_array[0] == "r"){
        
        SerialUSB.println("Saving LED Setpoints...");
        for (int i = 1; i < num_vials + 1; i++){
          saved_led_inputs[i - 1] = led_si.input_array[i].toInt();
        }
        
        SerialUSB.println("Echoing new LED commands:");
        echoCommand();
        
        SerialUSB.println("Waiting for OK to execute...\n");
        new_led_input = true;
      }

      // Acknoledgment to run command
      if (led_si.input_array[0] == "a" && new_led_input){
        updateValues();
        SerialUSB.println("Command Executed!");
        new_led_input = false;
      }

      led_si.address_found = false;
      input_string = "";
    }

    // Clears strings if too long
    // Should be checked server-side to avoid malfunctioning
    if (input_string.length() > 2000){
      SerialUSB.println("Cleared Input String");
      input_string = "";
    }
  }

  string_complete = false;
}



/********************************** FUNCTIONS **********************************/
// This function will read the Serial1 object onto input_string
void serialEvent(){

  while (Serial1.available()){    
    char input_char = (char)Serial1.read();
    input_string += input_char;

    if (input_char == '!'){
      string_complete = true;
      break;
    }
  }
}


// This function echoes the LED commands received
void echoCommand() {
  digitalWrite(12, HIGH);
  String output_string = led_address + "e,";

  for (int i = 1; i < num_vials + 1 ; i++) {
    output_string += led_si.input_array[i] + comma;
  }
  output_string += end_mark;

  delay(100);
  SerialUSB.println(output_string);
  Serial1.print(output_string);

  delay(100);
  digitalWrite(12, LOW);
}


// This function will broadcast the PD
void broadcastResponse(){
  digitalWrite(12, HIGH);
  String output_string = pd_address + "b,";
  
  for (int i = 0; i < num_vials; i++){
    output_string += output[i] + comma;
  }
  output_string += end_mark;

  delay(100); 
  SerialUSB.println(output_string);
  Serial1.print(output_string);

  delay(100);
  digitalWrite(12, LOW);
}


// This function will
void updateValues() {
  for (int i = 0; i < num_vials; i++) {
    Tlc.set(LEFT_PWM, i, 4095 - saved_led_inputs[i]);
  }
  while(Tlc.update());
}


// This function will read
void readMuxShield(){
  unsigned long avg_sum = 0;
  
  for (int i = 0; i < pd_avg_of; i++){
    avg_sum += readMux(active_vial);
    serialEvent();

    if (string_complete){
      break;
    }
  }

  if (!string_complete){
    output[active_vial] = avg_sum / pd_avg_of;

    if (active_vial == 15){
      active_vial = 0;
    }else{
      active_vial++;
    }
  }
}


int readMux(int channel){
  
  int control_pins[] = {m0, m1, m2, m3};
  int mux_channels[16][4] = {
    {0, 0, 0, 0}, //channel 0; Vial 1
    {1, 1, 0, 0}, //channel 3; Vial 2
    {1, 0, 0, 0}, //channel 1; Vial 3
    {0, 1, 0, 0}, //channel 2; Vial 4
    {0, 0, 1, 0}, //channel 4; Vial 5
    {1, 1, 1, 0}, //channel 7; Vial 6
    {1, 0, 1, 0}, //channel 5; Vial 7
    {0, 1, 1, 0}, //channel 6; Vial 8
    {0, 0, 0, 1}, //channel 8; Vial 9
    {1, 1, 0, 1}, //channel 11; Vial 10
    {1, 0, 0, 1}, //channel 9; Vial 11
    {0, 1, 0, 1}, //channel 10; Vial 12
    {0, 0, 1, 1}, //channel 12; Vial 13
    {1, 1, 1, 1}, //channel 15; Vial 14
    {1, 0, 1, 1}, //channel 13; Vial 15
    {0, 1, 1, 1}, //channel 14; Vial 16
  };

  // Sweeping through control pins to select desired channel
  for (int i = 0; i < 4; i ++) {
    digitalWrite(control_pins[i], mux_channels[channel][i]);
  }

  // Read the value at the channel
  int value = analogRead(m_out);
  return value;
}
