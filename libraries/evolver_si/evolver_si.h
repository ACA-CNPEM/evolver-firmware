/* Module Header That Has Common Functions Used Across eVOLVER Experiments */

#ifndef evolver_si_h
#define evolver_si_h
#include "Arduino.h"


class evolver_si {
	public:
		evolver_si(char* start_address, char* end_address, int num_of_vials);
		~evolver_si(void);
		void analyze_and_check(String input_string);
		boolean address_found;
		String input_array[50];
		int num_vials;

	private:
		char* _start_address;
		char* _end_address;
};


#endif
