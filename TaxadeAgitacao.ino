// Recurring Command: "stirr,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,_!"
// Immediate Command: "stiri,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,_!"
// Acknowledgement to Run: "stira,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,_!"

#include <evolver_si.h>
#include <Tlc5940.h>                  //Bibliotecas

// Input Variables Used
String inputString = "";         // string para sergurar dados de input
boolean stringComplete = false;  //booleano para indicar se a string está completa

// Serial Communication Variables
String comma = ",";
String end_mark = "end";
int num_vials = 16;
String address = "stir";  //sinaliza que o input é um comando para agitar
evolver_si in("stir","_!", num_vials+1); // 17 CSV-inputs from RPI, responsável pelo input do usuário
boolean new_input = false; // indica se houve uym novo input
int saved_inputs[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}; //input salvos

int Input[] = {8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}; //input

void setup()
{

  Tlc.init(LEFT_PWM,4095); // initialise TLC5940 and set all channels off
  SerialUSB.begin(9600);//começa serial USB (comunicação com o monitor serial)
  Serial1.begin(9600);
  // reserve 200 bytes for the inputString:
  inputString.reserve(1000);
  while (!SerialUSB); 
  pinMode(12, OUTPUT);
  digitalWrite(12, LOW); //Enquanto não houver input dfo Monitor serial, não emite nada;

}

void loop(){
  serialEvent(1);
   if (stringComplete) { //verifica se a string está completa. Se sim:
    SerialUSB.println(inputString);  //exibe no monitor serial a inputString
    in.analyzeAndCheck(inputString);  //percorre a string em busca de:

    if(in.addressFound){ //se encontrar a string "stir", que é o nosso address, 
      if (in.input_array[0] == "a" && new_input) {  // e se a letra logo em seguida for 'a' ('stira') e se houver novo input:
        update_values();  //atualiza os valores do input
        SerialUSB.println("Command Executed!"); //exibe "command executed no monitor serial"
        new_input = false; //volta a booleana de volta para false, aguardando novo input.
      }

      inputString = "";  //limpa a inputstring

    }

    //Clears strings if too long
    if (inputString.length() > 900){
      SerialUSB.println("Cleared Input String");
      inputString = ""; // limpa a input string se a mesma tiver mais do que 900 caracteres
    }

    stringComplete = false;
    in.addressFound = false;
  }
  exec_stir(); //executa o comando para agitar a amostra.
}





void serialEvent(int time_wait) { //função que verifica os valores do input da string, seu tipo (stira, stirr ou stiri) e se a mesma está completa (último caractere = _!)
  for (int n=0; n<time_wait; n++) {
      while (SerialUSB.available()) { //enquanto houver input do monitor serial
        char inChar = (char)SerialUSB.read();//criar um char "inChar", que assume valores 
        inputString += inChar;
        if (inChar == '!') {
          stringComplete = true;
        }
      }
    delay(1);
  }

}

void exec_stir()
{
  for (int i = 0; i < num_vials; i++) {
    if (Input[i]  != 0) {
      Tlc.set(LEFT_PWM,i, 0);
      //Serial.print("Code: " + String(in.output_array[i]) + " ");
    }
  }
  //Serial.println();

    while(Tlc.update());
    serialEvent(12);

   // 10 settings for the stir rate
   for (int n = 0; n < 98; n++) {
    for (int i = 0; i < num_vials; i++) {

      if (Input[i] == n) {
        Tlc.set(LEFT_PWM,i, 4095);
      }
    }
    while(Tlc.update());
    serialEvent(1);
   }

   serialEvent(70);
}

void update_values() {
  for (int i = 0; i < num_vials; i++) {
     Input[i] =  saved_inputs[i];
  }
}
