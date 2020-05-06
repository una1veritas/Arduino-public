#include "sortalgorithms.h"

const INDEXTYPE maxsize = 512;
DATATYPE data[maxsize];
const DATATYPE DATA_MAX = 100000000UL;

INDEXTYPE get_length() {
	unsigned long swatch = millis();
	unsigned int tcount = 500;
	String inputstr = "";
	while ( Serial.available() > 0 || tcount > 0 ) {
		if ( Serial.available() > 0 ) {
			inputstr += (char) Serial.read();
			swatch = millis();
			tcount = 500;
		} else {
			if ( swatch != millis() ) {
				tcount--;
				swatch = millis();
			}
		}
	}

	if ( inputstr.length() > 0 ) {
		return inputstr.toInt();
	}
	return 0;
}

void setup() {
  Serial.begin(57600);
  Serial.println("Ready.");

}

void loop() {
  long stopwatch;
  long sum, tmax;
  long rseed;
  INDEXTYPE num = 0;
  unsigned int tries = 16;

  if ( Serial.available() > 0 ) {
	  num = get_length();
	  Serial.print(num);
	  Serial.println(" Ok.");
	  num = min(num, maxsize);
  }
  if ( num > 0 ) {
	  rseed = millis();
	  Serial.print("======= No. ");
    Serial.print(millis());
    Serial.println(" ======= ");
    /*
    Serial.println("Before sorting: ");
    for(INDEXTYPE i = 0; i < min(num, 10); ++i) {
      Serial.print(data[i]);
      Serial.print(", ");
    }
    Serial.println();
    */
    Serial.print(num);
    Serial.println(" elements.");
    Serial.println();

    Serial.print("selection sort: ");
    //
    randomSeed(rseed);
    sum = 0;
    tmax = 0;
    for(unsigned int t = 0; t < tries; ++t) {
      for(INDEXTYPE i = 0; i < num; ++i) {
        data[i] = random(0, DATA_MAX);
      }
      stopwatch = micros();
    	selectionSort(data, num);
    	stopwatch = micros() - stopwatch;
      
      if (stopwatch > tmax )
        tmax = stopwatch;
      sum += stopwatch;
    }
    //
    Serial.print("max. ");
    Serial.print(tmax);
    Serial.print(", avr. ");
    Serial.print(sum/tries);
    Serial.println(" u sec, ");
    Serial.println();
    //
    Serial.print("bubble sort: ");
    randomSeed(rseed);
    sum = 0;
    tmax = 0;
    for(unsigned int t = 0; t < tries; ++t) {
  	  for(INDEXTYPE i = 0; i < num; ++i) {
	  	  data[i] = random(0, DATA_MAX);
	    }
  	  stopwatch = micros();
  	  bubbleSort(data, num);
  	  stopwatch = micros() - stopwatch;
      if (tmax < stopwatch)
        tmax = stopwatch;
	    sum += stopwatch;
    }
    //
    Serial.print("max. ");
    Serial.print(tmax);
    Serial.print(", avr. ");
    Serial.print(sum/tries);
    Serial.println(" u secs, ");
    Serial.println();
    //
    Serial.print("merge (r) sort: ");
    randomSeed(rseed);
    sum = 0;
    tmax = 0;
    for(unsigned int t = 0; t < tries; ++t) {
  	  for(INDEXTYPE i = 0; i < num; ++i) {
  		  data[i] = random(0, DATA_MAX);
  	  }
  	  stopwatch = micros();
  	  mergeSort_recursive(data, num);
  	  stopwatch = micros() - stopwatch;
      if (stopwatch > tmax)
        tmax = stopwatch;
	    sum += stopwatch;
    }
    //
    Serial.print("max. ");
    Serial.print(tmax);
    Serial.print(", avr. ");
    Serial.print(sum/tries);
    Serial.println(" u secs, ");
    Serial.println();
    //
    Serial.print("quick sort: ");
    randomSeed(rseed);
    sum = 0;
    tmax = 0;
    for(unsigned int t = 0; t < tries; ++t) {
  	  for(INDEXTYPE i = 0; i < num; ++i) {
  		  data[i] = random(0, DATA_MAX);
  	  }
  	  stopwatch = micros();
  	  quickSort(data, num);
  	  stopwatch = micros() - stopwatch;
      if (tmax < stopwatch)
        tmax = stopwatch;
  	    sum += stopwatch;
    }
    //
    Serial.print("max. ");
    Serial.print(tmax);
    Serial.print(", avr. ");
    Serial.print(sum/tries);
    Serial.println(" usec, ");

    Serial.println("After sorting: ");
    for(INDEXTYPE i = 0; i < min(num,10); ++i) {
        Serial.print(data[i]);
        Serial.print(", ");
    }
    Serial.println();
    Serial.println();
    //
    //
  }
}
