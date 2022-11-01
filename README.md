# Introduction to Robotics (2022 - 2023)
Introduction to Robotics laboratory homeworks, taken in the 3rd year at the Faculty of Mathematics and Computer Science, University of Bucharest. Each homework includes requirements, implementation details, code and image files.

<details>

<summary>
<b>
Homework 1 (RGB LED)
</b>
</summary>
<br>

#### Task:

A separate potentiometer is used in controlling each color of the RGB LED (Red, Green and Blue). The control is done with digital electronics (the values of the potentiometers are read with Arduino, and the corresponding mapped values are written to the pins connected to the led).

#### Components:
  * Arduino Uno microcontroller board
  * Breadboard
  * RBG LED
  * Potentiometers (x3)
  * Resistors and wires (per logic)

#### The showcase video can be found [here](https://www.youtube.com/watch?v=5kBeWEaKKo8).

#### Photo of the setup:
<img src="/homework_1/h1_photo.jpg?raw=true" width=65% height=65% />

</details>

<details>

<summary>
<b>
Homework 2 (Crosswalk)
</b>
</summary>
<br>

#### Task:

Building the traffic lights for a crosswalk. Two LEDs are used to represent the traffic lights for pedestrians (red and green) and three LEDs are used to represent the traffic lights for cars (red, yellow and green). By default, the system will show green for cars and red for pedestrians. The button has the role of signaling a request from pedestrians for crossing the street (it has no effect if the system is already processing such a request). The buzzer will emit a beeping sound when it's green for pedestrians. There are 4 states which the system goes through:
  * State 1 (default, reinstated after state 4 ends): green light for cars, red light for people, no sounds. Duration: indefinite, changed by pressing the button.
  * State 2 (initiated by counting down 8 seconds after a button press): yellow light for cars, red for people and no sounds. Duration: 3 seconds.
  * State 3 (initiated after state 2 ends): red for cars, green for people and a beeping sound from the buzzer at a constant interval. Duration: 8 seconds.
  * State 4 (initiated after state 3 ends): red for cars, blinking green for people and a beeping sound from the buzzer, at a constant interval, faster than the beeping in state 3. This state lasts 4 seconds.

#### Components:
  * Arduino Uno microcontroller board
  * Breadboard
  * LEDs (x5)
  * Button
  * Buzzer
  * Resistors and wires (per logic)
  
#### The showcase video can be found [here](https://youtu.be/8GogUdYh78w).

#### Photo of the setup:
<img src="/homework_2/h2_photo.jpg?raw=true" width=65% height=65% />

</details>
