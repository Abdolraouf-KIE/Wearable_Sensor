# Progress of the project

- The reliabaility of the the MQTT message is high
  - The interval of sending MQTT messages is set to 10seconds and a for loop is used for to ensure the connection.


1. SPO2 measurements send with MQTT:
   1. reading every 1seconds
   2. making the JSON format every 10seconds (aka every 10index)

# What is next:

1. send MQTT after 10seconds (MQTT deosnt contain the SPO2 data just yet
   1. This should be done to avoid the missing of beat detection.
   2. JSON array format
2. Temperature:
   1. Obtain the reading
3. Combine temp and MQTT together.