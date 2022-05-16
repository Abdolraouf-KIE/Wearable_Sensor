# Progress of the project

- The reliabaility of the the MQTT message is high
  - The interval of sending MQTT messages is set to 10seconds and a for loop is used for to ensure the connection.


1. SPO2 measurements send with MQTT:
   1. only reading every 10seconds
   2. sending MQTT after 10seconds (MQTT deosnt contain the SPO2 data just yet)

# What is next:

1. Send SPO2 to broker at a suitable interval.
   1. package the message in json format
   2. observe the results on node red.
2. Temperature:
   1. Obtain the reading
3. Combine temp and MQTT together.