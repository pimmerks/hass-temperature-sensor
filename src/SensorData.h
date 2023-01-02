#ifndef SENSORDATA_H
#define SENSORDATA_H

/**
 * A structure of all the sensor data.
 */
struct SensorData {
  public:
    /**
     * The co2 concentration in ppm.
     */
    uint16_t co2 = 0;

    /**
     * The tvoc concentration in ppb.
     */
    uint16_t tvoc = 0;

    /**
     * The temperature in degrees celsius.
     */
    float tempC = 0.0f;

    /**
     * The pressure in Pascal.
     */
    float pressure = 0.0f;

    /**
     * The altitude in meters.
     */
    float altitude = 0.0f;

    /**
     * The humidity in percentage.
     */
    float humidity = 0.0f;

    /**
     * The error code, if available.
     */
    uint8_t error;
};

#endif