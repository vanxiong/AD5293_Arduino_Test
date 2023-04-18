#include <SPI.h> // use the SPI library. It's built in to the IDE so no need to install

//Settings for "automode" of data taking
#define ADC_PIN A0
//    
//     5V
//    ---
//     |
//     Z (fixed resistor ~ 150 or 1000 ohm)
//     |
//     +----- ADC_Reading
//     |
//     Z (digipot 100||147 or 10k||2.2k)
//     |
//    GND
//

//SPI settings
#define CHIP_SELECT_PIN 10
#define MAX_SPI_SPEED   1000000 // 1Mhz
#define DATA_ORDER      MSBFIRST
#define DATA_MODE       SPI_MODE1 // CPOL=0 CPHA=1

//Some common AD5293 SPI command
#define NOP                 0x0000 // do nothing
#define READ_RDAC           0x0800 // read wiper value from SDO output in the next frame
#define RESET_RDAC          0x1000 // Reset RDAC to midrange and control reg to default
#define READ_CONTROL        0x1C00
#define POWER_DOWN          0x2000
#define PERFORMANCE_MODE    0x1802 // best for rheostat usage. Rwb(D)=(D/1024)*Rab   or   Rwa(D)=([1024-D]/1024)*Rab
#define NORMAL_MODE         0x1803 // best for normal voltage divider usage
// #define ENABLE_WIPER_WRITE  0x1802 // enable changing of resistor
#define WRITE_DATA          0x0400 // need to OR with value 0-1024

SPISettings mySpiSetting(MAX_SPI_SPEED, DATA_ORDER, DATA_MODE);

void setup() {
  Serial.begin(9600);
  digiPotSetup(PERFORMANCE_MODE); // enable RDAC write and select performance mode (rheostat)

  //setup the ADC for external reference - 3.3V probably
  //analogReference(EXTERNAL);
}

void loop() {
  // put your main code here, to run repeatedly:
  //manualMode();
  autoMode();
}

void autoMode(){
  // cycle from 1 to 1023 step then wait
  Serial.println("--------------------------------------------");
  Serial.println("Step, ADC_Reading");
  for(uint16_t i=1; i<=1023; i++){
    digiPotWrite(i);
    int adcData = analogRead(ADC_PIN);
    Serial.print(i);
    Serial.print(",");
    Serial.println(adcData);
    delay(10); // wait 10 ms
  }

  while(1){
    // do nothing and just wait here
  }
  

}

void manualMode(){
  Serial.println("Input digipot step: ");
  while (Serial.available()==0){
    //do nothing... just wait until new input
  }
  uint16_t data = Serial.parseInt();
  digiPotWrite(data);
}

void digiPotSetup(uint16_t data) {
  pinMode(CHIP_SELECT_PIN, OUTPUT);    // set the chip select pin as output
  SPI.begin(); // initialize SPI
  SPI.beginTransaction(mySpiSetting);  // set SPI setting
  digitalWrite(CHIP_SELECT_PIN, LOW);  // assert chip select - active low
  SPI.transfer16(data);                // 16 bit data, MSB-first if set properly in setup
  digitalWrite(CHIP_SELECT_PIN, HIGH); // de-assert chip select - active low
  SPI.endTransaction();                // end SPI transaction
}

void digiPotWrite(uint16_t data) {
  if(data>1023) data = 1023;
  else if (data<0) data = 0;
  data = data | WRITE_DATA;
  digitalWrite(CHIP_SELECT_PIN, LOW);  // assert chip select - active low
  SPI.transfer16(data);                // 16 bit data, MSB-first if set properly in setup
  digitalWrite(CHIP_SELECT_PIN, HIGH); // de-assert chip select - active low
  SPI.endTransaction();                // end SPI transaction
}
