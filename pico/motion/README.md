# Motion Sensor

## Description

Motion sensor code that works alongside an MQTT server to trigger particular events when motion is detected.

## State Machine

The device can be in 3 different states:

### INACTIVE (0)

This is when the device should not read from the motion sensor. This is useful for light triggers when the sensor does not need to be used during daytime hours.

The inactive state works by disabling interrupts based on an MQTT message.

### SENSING (1)

This is the default state when the device is reading the motion sensor values

### ON (2)

This is the post-detection state after motion has been detected. It can be set to trigger another MQTT event if it has been in this state for X minutes with no motion detected.
