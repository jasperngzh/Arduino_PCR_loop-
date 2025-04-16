# Arduino_PCR_loop-
PCR Thermocycler
This project demonstrates an Arduino-based PCR (Polymerase Chain Reaction) Thermocycler for DNA amplification. It uses a thermistor to measure temperature and a Peltier module (with fan control) to maintain the specific temperature ranges required for PCR cycles. The device is controlled through a sequence of steps: denaturation, annealing, and extension, which are essential for PCR-based experiments. The Arduino codes run based on easy to understand switch statement and another on generic switch blocks which may be eirsier to decode.

 Hardware Components
Arduino Uno: Microcontroller to run the system.
Thermistor: Used to measure temperature.
Peltier Module: For heating and cooling, controlled through PWM.
Fan: Used to cool down the system after denaturation.
LCD 16x2 (I2C): Displays current temperature and PCR cycle status.
Resistors: For voltage divider (used with the thermistor).
Wires & Breadboard: For connecting all components.

 Features
Automatic Temperature Control: Maintains specific temperatures for each PCR step.
Denaturation: 94°C
Annealing: 55°C
Extension: 72°C
Real-Time Temperature Display: Shows the current temperature on an LCD screen.
Timing Management: Uses Arduino's millis() function to manage the timing of each PCR stage (denaturation, annealing, extension).
Cooling Fan Control: Automatic fan control based on temperature to prevent overheating.

 How It Works
The device uses an Arduino microcontroller to cycle through a series of PCR steps. The following temperature stages are controlled automatically:
Denaturation: The DNA sample is heated to 94°C for 30 seconds to separate the DNA strands.
Annealing: The temperature is lowered to 55°C for 30 seconds to allow primers to bind.
Extension: The temperature is raised to 72°C for the polymerase enzyme to replicate the DNA.
The Peltier module is controlled via PWM to either heat or cool the system, while the fan is turned on/off based on the temperature to assist with cooling.
