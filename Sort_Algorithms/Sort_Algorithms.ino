#include "sortalgorithms.h"

String myInput;
const INDEXTYPE maxsize = 512;
DATATYPE data[maxsize];

void setup() {
  Serial.begin(9600);
  Serial.println("Ready.");

}

void loop() {
  long stopwatch;

  while (Serial.available()) {
    myInput += (char)Serial.read();
    if ( Serial.available() )
      continue;
    delay(10);
  }

  if ( myInput.length() > 0 ) {
    for(INDEXTYPE i = 0; i < myInput.length(); ++i) {
      data[i] = myInput[i];
    }
    Serial.print("======= No. ");
    Serial.print(millis());
    Serial.println(" ======= ");
    Serial.println("Before sorting: ");
    Serial.print("\'");
    Serial.print((String)myInput);
    Serial.println("\'");
    Serial.print(myInput.length());
    Serial.println(" elements:");
    //
    Serial.println("selection sort: ");
    stopwatch = micros();
    selectionSort(data, myInput.length());
    stopwatch = micros() - stopwatch;
    //
    Serial.print("It took ");
    Serial.print(stopwatch);
    Serial.println(" u secs, ");
    Serial.println();
    //
    for(INDEXTYPE i = 0; i < myInput.length(); ++i) {
      data[i] = myInput[i];
    }
    Serial.println("bubble sort");
    stopwatch = micros();
    bubbleSort(data, myInput.length());
    stopwatch = micros() - stopwatch;
    //
    Serial.print("It took ");
    Serial.print(stopwatch);
    Serial.println(" u secs, ");
    Serial.println("After sorting: ");
    for(INDEXTYPE i = 0; i < myInput.length(); ++i) {
      Serial.print((char)data[i]);
    }
    Serial.println();
    Serial.println();
    //
    for(INDEXTYPE i = 0; i < myInput.length(); ++i) {
      data[i] = myInput[i];
    }
    Serial.println("quick sort");
    stopwatch = micros();
    quickSort(data, myInput.length());
    stopwatch = micros() - stopwatch;
    //
    Serial.print("It took ");
    Serial.print(stopwatch);
    Serial.println(" u secs, ");
    Serial.println("After sorting: ");
    for(INDEXTYPE i = 0; i < myInput.length(); ++i) {
      Serial.print((char)data[i]);
    }
    Serial.println();
    Serial.println();
    //
    //
    //
    myInput = "";
  }
}


