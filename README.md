# Progress of the project

- The reliabaility of the the MQTT message is low (there is packet loss).
  - The interval of messages is set to 10seconds and a for loop is used for to ensure the connection.
  - I will refer to my MQTT project before to see how i did it.
    - I will use the function MQTTSend. 
> The function didn't do much but the nice thing about it is that it will say message sending failed if there is failure. Even though some messages failed to send the message failure debug line wasn't given (not sure why).
    - I used a different reconnect() function where there is no subscribe in it.
    - out of 78 messages to only humid topic, all were sent.

# What is next:

1. SPO2 measurements:
   1. Obtain the readings
2. Temperature:
   1. Obtain the reading
3. Node red
   1. Visualize the sensor information on graph
      1. **Send dummy json messages of the temperature every 10 seconds.
      2. Visualize the data