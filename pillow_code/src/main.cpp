#include <Wire.h>
#include <Adafruit_ADS1X15.h>

// command to read monitor: pio device monitor | Tee-Object -FilePath log.txt

HardwareSerial Serial1(USART1);  // Use USART1: PA9 (TX), PA10 (RX)

TwoWire I2C_1(PB7, PB6);  // SDA, SCL

Adafruit_ADS1115 ads;  // This will only compile on 2.5.0+

const int sampleIntervalMcs = 62500; // Sample every 1000 ms = 1 Hz
unsigned long lastSampleTime = 0;

void scan_i2c(){
  Serial1.println("I2C Scanner...");

  for (uint8_t address = 1; address < 127; address++) {
    Serial1.print("Scanning address 0x");
    Serial1.print(address, HEX);
    Serial1.print("...\n");
    I2C_1.beginTransmission(address);
    if (I2C_1.endTransmission() == 0) {
      Serial1.print("Found I2C device at 0x");
      Serial1.println(address, HEX);
      delay(10);
    }
  }
  Serial1.println("I2C scan complete.");
}



void setup() {
  I2C_1.begin();           // Start I2C1 on PB7/PB6
  I2C_1.setClock(400000);  // Set I2C speed to 100kHz
  Serial1.begin(921600);
  delay(5000);

  scan_i2c();

  Serial1.println("Initializing ADS1115...");

  if (!ads.begin(0x48, &I2C_1)) {  // Default I2C address is 0x48
    Serial1.println("ADS1115 not found!");
    while (1);
  }

  //ads.setDataRate(ADS1115_DR_128SPS); // Sample rate: 128 samples per second
  ads.setGain(GAIN_TWOTHIRDS);        // +/- 6.144V range (default)
  // ads.setGain(GAIN_ONE);              // +/- 4.096V
  // ads.setGain(GAIN_TWO);              // +/- 2.048V
  // ads.setGain(GAIN_FOUR);             // +/- 1.024V
  // ads.setGain(GAIN_EIGHT);            // +/- 0.512V
  // ads.setGain(GAIN_SIXTEEN);          // +/- 0.256V

  Serial1.println("ADS1115 initialized");
}

void loop() {
  unsigned long now = micros();
  uint32_t sampletime;
  if (now - lastSampleTime >= sampleIntervalMcs) {
    lastSampleTime = now;

    int16_t a0 = ads.readADC_SingleEnded(0);
    int16_t a1 = ads.readADC_SingleEnded(1);
    sampletime = micros();
    int16_t a2 = ads.readADC_SingleEnded(2);
    int16_t a3 = ads.readADC_SingleEnded(3);

    Serial1.print(sampletime);
    Serial1.print(","); Serial1.print(a0);
    Serial1.print(","); Serial1.print(a1);
    Serial1.print(","); Serial1.print(a2);
    Serial1.print(","); Serial1.println(a3);
  }
}
