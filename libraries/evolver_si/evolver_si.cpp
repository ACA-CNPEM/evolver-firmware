#include "Arduino.h"
#include "evolver_si.h"


// CONSTRUCTOR
evolver_si::evolver_si(char* start_address, char* end_address, int num_of_vials) {
	// DO NOT use commas in the start or end address
	_start_address = start_address;
	_end_address = end_address;
	address_found = false;
	num_vials = num_of_vials;
}


// DESTRUCTOR	
evolver_si::~evolver_si() {
	delete[] input_array;
}


// SI MANIPULATION FUNCTION
void evolver_si::analyzeAndCheck(String input_string){
   /* Debugging help
   SerialUSB.println("Starting!!!");
   SerialUSB.print("input_string = ");
   SerialUSB.println(input_string);
   SerialUSB.print("_start_address = ");
   SerialUSB.println(_start_address);
   SerialUSB.print("_end_address = ");
   SerialUSB.println(_end_address);
   SerialUSB.print("address_found = ");
   SerialUSB.println(address_found);
   SerialUSB.print("num_vials = ");
   SerialUSB.println(num_vials);
   SerialUSB.println("sizeof start and end address:");
   SerialUSB.println(sizeof(_start_address));
   SerialUSB.println(sizeof(_end_address));*/

	// Finds the index for where the _start_address and _end_address begin in the input_string
	int start_index = input_string.lastIndexOf(_start_address);
	int end_index = input_string.lastIndexOf(_end_address);

	if (start_index != -1 && end_index != -1){

		// Find segment from input_string with correct header
		input_string = input_string.substring(start_index, end_index + sizeof(_end_address));
		start_index = input_string.indexOf(_start_address);
		end_index  = input_string.indexOf(_end_address);

		// Deals with edge cases from serial queue (i.g. extra ending indicator)
		input_string = input_string.substring(start_index, end_index + sizeof(_end_address));
	 	start_index = input_string.lastIndexOf(_start_address);
	 	end_index = input_string.lastIndexOf(_end_address);

		// Counting the number of commands; num_commands strats in -1 to account
		// for the fist comma, that separates the header from the first command
		int num_commands = 0; 
		int j = 0;
		String temp_string = "";

		for(int h = 0; h < input_string.length(); h++){
			if(input_string[h] == ','){
				num_commands++;
			}
		}
		
		if(num_commands == num_vials){
			// If the number of commands equals the number of vials, go though the string
			// saving them onto the input_array

			for(int i = (start_index + strlen(_start_address) + 1); i < end_index; i++){
				if (input_string[i] == ',') {
					// A command has ended, another should begin
					input_array[j] = temp_string;
					temp_string = "";
					j++;

				}else{
					// Combine the elements of the command between commas
					temp_string += input_string[i];
				}
			}

			// Gives Condition To Do User Requests
			address_found = true;
		}
	}
}
