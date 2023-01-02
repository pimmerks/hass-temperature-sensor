#include <Wire.h>
#include <SparkFunBME280.h> //Click here to get the library: http://librarymanager/All#SparkFun_BME280
#include <SparkFunCCS811.h> //Click here to get the library: http://librarymanager/All#SparkFun_CCS811

#include "SensorData.h"

#define CCS811_ADDR 0x5B // Default I2C Address


// Base class/ interface for sensor readers
class BaseSensorReader {
  public:
    virtual void begin();
    virtual SensorData getData();
};


class TempReader: public BaseSensorReader {
private:
  CCS811 myCCS811;
  BME280 myBME280;

public:
  TempReader() : myCCS811(CCS811_ADDR) {
  }

  void begin() {
    Wire.begin();

    myBME280.setTemperatureCorrection(-2.0f);
    myBME280.setStandbyTime(5);

    myBME280.setTempOverSample(2);
    myBME280.setPressureOverSample(2);
    myBME280.setHumidityOverSample(2);
    myBME280.setFilter(1);

    // Calling .begin() causes the settings to be loaded
    delay(10); // Make sure sensor had enough time to turn on. BME280 requires 2ms to start up.
    myBME280.begin();

    // This begins the CCS811 sensor and prints error status of .beginWithStatus()
    CCS811Core::CCS811_Status_e returnCode = myCCS811.beginWithStatus();
    Serial.print("CCS811 begin exited with: ");
    Serial.println(myCCS811.statusString(returnCode));
  }

  /**
   * Parse error
   */
  String parseError(uint8_t error)
  {
    if (error == 0xFF) // comm error
    {
      return "Failed to get ERROR_ID register";
    }

    if (error & 1 << 5)
      return "HeaterSupply";
    if (error & 1 << 4)
      return "HeaterFault";
    if (error & 1 << 3)
      return "MaxResistance";
    if (error & 1 << 2)
      return "MeasModeInvalid";
    if (error & 1 << 1)
      return "ReadRegInvalid";
    if (error & 1 << 0)
      return "MsgInvalid";
  }

  /**
   * Retrieve the sensor data
   */
  SensorData getData()
  {
    SensorData data;

    // Wait for data available
    while(!myCCS811.dataAvailable())
    {
      if (myCCS811.checkForStatusError())
      {
        //If the CCS811 found an internal error, return it.
        data.error = myCCS811.getErrorRegister();
        return data;
      }

      delay(1000);
    }

    data.tempC = myBME280.readTempC();
    data.pressure = myBME280.readFloatPressure();
    data.altitude = myBME280.readFloatAltitudeMeters();
    data.humidity = myBME280.readFloatHumidity();

    myCCS811.setEnvironmentalData(data.humidity, data.tempC);

    // Calling this function updates the global tVOC and eCO2 variables
    myCCS811.readAlgorithmResults();

    // Now our data is available!
    // Let's read it, then correct it.
    data.co2 = myCCS811.getCO2();
    data.tvoc = myCCS811.getTVOC();

    return data;
  }
};
