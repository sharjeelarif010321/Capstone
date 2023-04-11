This directory contains all schematics for the Baby Safety Subsystem, created in KiCAD.
<br>
<br>
**/PCB/** is the schematic used to design the PCB.
<br>
<br>
**/PCB_backwards_STM/** is an identical schematic to the previous, with the STM32F103RB's footprint mirrored on the PCB layout.
This was the first layout for the PCB I created, but I realized that the rear pins on my STM32F103RB board were bent, which means the STM board would not have been able to mount to the PCB properly.
<br>
<br>
**/STM/** is the schematic that shows the entire circuit, including the layout of the pressure mat. Since the previous two schematics have only a terminal block, it is not easy to visualize the connection to the pressure mat. This schematic does not include any PCB layout.
<br>
<br>
**/Wearable/** is the schematic for the wearable heart rate and blood oxygen monitor. A PCB was not necessary for this schematic.
<br>
<br>
**/libraries/** contains the two custom libraries that were necessary for me to get proper footprints for all of my components.
