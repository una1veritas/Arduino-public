
String myInput;

void setup() {
  Serial.begin(9600);
  Serial.println("Ready.");

}

void loop() {
  String inputcopy;
  long stopwatch;

  while (Serial.available()) {
    myInput += Serial.read();
    if ( Serial.available() )
      continue;
    delay(10);
  }

  if ( myInput.length() > 0 ) {
    inputcopy = String(myInput);
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
    stopwatch = micros();
    SelectionSort(myInput, myInput.length());
    stopwatch = micros() - stopwatch;
    //
    Serial.println();
    Serial.print("It took ");
    Serial.print(stopwatch);
    Serial.println(" micro seconds, ");
    Serial.println("After sorting: ");
    Serial.println(myInput);
    Serial.println();
    //
    stopwatch = micros();
    BubbleSort(inputcopy, myInput.length());
    stopwatch = micros() - stopwatch;
    //
    Serial.println();
    Serial.print("It took ");
    Serial.print(stopwatch);
    Serial.println(" micro seconds, ");
    Serial.println("After sorting: ");
    Serial.println(inputcopy);
    Serial.println();
    //
    //
    myInput = "";
  }
}

void SelectionSort(String chararray, int size) {
  int i, j;
  char tmp;

  for (j = 0; j < size-1; j++) {
    for (i = j; i < size; i++) {
      tmp = chararray[i];
      if ( tmp > chararray[j] ) {
        chararray[i] = chararray[j];
        chararray[j] = tmp;
      }
    }
  }
}

void BubbleSort(String chararray, int n) {
  int i, j;
  char tmp;

  for (i = 0; i+1 < n; i++) {
    for (j = 0; j+1 < n; j++) {
      if ( chararray[j+1] > chararray[j] ) {
        tmp = chararray[j];
        chararray[j] = chararray[j+1];
        chararray[j+1] = tmp;
      }
    }
  }
}




