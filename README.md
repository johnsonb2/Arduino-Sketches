Arduino-Sketches
================

This fork of the project has adapted the "Quartzless Control" sketch to work 
with the Arduino Mega ADK.

Instead of using a shift register, the Port A pins are connected to the
corresponding DA pins. This allows for sending a byte to the AY-3-8910 by
simply writing to Port A.
