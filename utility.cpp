/*****************************************
 *  
 *  
 *****************************************/

#include "utility.h"

// Prints a string to serial
//  print_string    string to print
void serial_print(const char* print_string, bool side) {
  const char* side_string;
  if (side) {
    side_string = "Left";
  } else {
    side_string = "Right";
  }
  if (Serial) {
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] ");
    Serial.print(print_string);
    Serial.print(" ");
    Serial.print(side_string);
    Serial.print("\n");
  }
}

// Prints a named value to serial
//  print_string    description
//  print_val       value
void serial_print_val(const char* print_string, unsigned long print_val, bool side) {
    const char* side_string;
    if (side) {
      side_string = "Left";
    } else {
      side_string = "Right";
    }
    if (Serial) {
          Serial.print("[");
          Serial.print(millis());
          Serial.print("] ");
          Serial.print(print_string);
          Serial.print(" ");
          Serial.print(print_val);
          Serial.print(" ");
          Serial.print(side_string);
          Serial.print("\n");
    }
}

void serial_print_uval(const char* print_string, int32_t print_val, bool side) {
    const char* side_string;
    if (side) {
      side_string = "Left";
    } else {
      side_string = "Right";
    }
    if (Serial) {
          Serial.print("[");
          Serial.print(millis());
          Serial.print("] ");
          Serial.print(print_string);
          Serial.print(" ");
          Serial.print(print_val);
          Serial.print(" ");
          Serial.print(side_string);
          Serial.print("\n");
    }
}


// clever print function
//https://gist.github.com/EleotleCram/eb586037e2976a8d9884
int aprintf(char *str, ...) {
	int i, j, count = 0;

	va_list argv;
	va_start(argv, str);
	for(i = 0, j = 0; str[i] != '\0'; i++) {
		if (str[i] == '%') {
			count++;

			Serial.write(reinterpret_cast<const uint8_t*>(str+j), i-j);

			switch (str[++i]) {
				case 'd': Serial.print(va_arg(argv, int));
					break;
				case 'l': Serial.print(va_arg(argv, long));
					break;
				case 'f': Serial.print(va_arg(argv, double));
					break;
				case 'c': Serial.print((char) va_arg(argv, int));
					break;
				case 's': Serial.print(va_arg(argv, char *));
					break;
				case '%': Serial.print("%");
					break;
				default:;
			};

			j = i+1;
		}
	};
	va_end(argv);

	if(i > j) {
		Serial.write(reinterpret_cast<const uint8_t*>(str+j), i-j);
	}

	return count;
}
