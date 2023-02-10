/* COMMANDS:
  Recurring Command: "stirr,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,_!"
  Immediate Command: "stiri,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,_!"
  Acknowledgement to Run: "stira,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,_!"
*/


/* LIBRARIES */
#include <Arduino.h>
#include <evolver_si.h>
#include <Tlc5940.h>

/* VARIABLES */
// Serial event variables
String input_string = "";
boolean string_complete = false;

// Broadcast response variables
String comma = ",";
String end_mark = "end";
String address = "stir";

// Input variables
int num_vials = 16;
evolver_si si("stir", "_!", num_vials + 1); // 17 CSV-inputs from RPI, responsável pelo input do usuário
boolean new_input = false;
int saved_inputs[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8};

int Input[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}; //input



/********************************** FUNCTIONS **********************************/
// This function will read the Serial1 object onto input_string
void serialEvent(int time_wait){
  for (int i = 0; i < time_wait; i++){

      while (Serial1.available()){
        char input_char = (char)Serial1.read();
        input_string += input_char;
        
        if (input_char == '!'){
          string_complete = true;
        }
      }
    
    delay(1);
  }
}


// This function executes the stirring
void exec_stir(){
  for (int i = 0; i < num_vials; i++){
    if (Input[i] != 0){
    Tlc.set(LEFT_PWM, i, 0);
    }
  }
  
  while (Tlc.update()); 
  serialEvent(12);

  // 10 settings for the stir rate
  for (int i = 0; i < 98; i++){
    for (int j = 0; j < num_vials; j++){
      if (Input[j] == i){
        Tlc.set(LEFT_PWM, j, 4095);
      }
    }
    
    while (Tlc.update());
    serialEvent(1);
  }
  serialEvent(70);
}


// This function updates the input values
void updateValues(){
  for (int i = 0; i < num_vials; i++) {
     Input[i] = saved_inputs[i];
  }
}


// This function echoes the commands received
void echoCommand(){
  digitalWrite(12, HIGH);
   String output_string = address + "e,";
   
   for (int i = 0; i < num_vials; i++){
    output_string += saved_inputs[i] + comma;
   }
   output_string += end_mark;

   SerialUSB.println(output_string);
   Serial1.print(output_string);

   delay(300);
   digitalWrite(12, LOW);
}



/********************************** SETUP **********************************/
void setup(){
  SerialUSB.begin(9600);
  Serial1.begin(9600);
  while (!Serial1);

  // Communication 
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW);
  input_string.reserve(2000); // reserve 2000 bytes for the input_string

  // PWM control object  
  Tlc.init(LEFT_PWM, 4095);  
}



/********************************** LOOP **********************************/
void loop(){
  serialEvent(1);
   if (string_complete){ 

    si.analyzeAndCheck(input_string);
    if (si.address_found){
      
      // Instant or recurring command 
      if (si.input_array[0] == "i" || si.input_array[0] == "r"){
        
        SerialUSB.println("Saving Setpoints...");
        for (int i = 1; i < num_vials + 1; i++){
          saved_inputs[i - 1] = si.input_array[i].toInt();
        }

        SerialUSB.println("Echoing commands:");
        echoCommand();
        SerialUSB.println("Waiting for OK to execute...\n");
        new_input = true;
      }

      // Acknoledgment to run command
      if (si.input_array[0] == "a" && new_input){
        updateValues();
        SerialUSB.println("Command Executed!\n");
        new_input = false;
      }

      input_string = "";       
    }

    //Clears strings if too long
    if (input_string.length() > 900){
      SerialUSB.println("Cleared Input String");
      input_string = "";
    }

    si.address_found = false;
    string_complete = false;
  }

  // Executes the stirring command
  exec_stir();
}

