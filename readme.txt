TMOOS - Teensy Multifunction Othogon Operating System
=====================================================

TMOOS is a software system for the handheld game device project currently underway by Brandon Foltz. It's objective is to handle the following generalized features:

Drisplay/Graphics driver and API for monochrome LED dot matrix display.
Handle voltage driven discreet frequency generator for sound.
Provide access to input (potentiometer, pushbutton) via a simple API to the rest of the system.
Manage a long-term storage system for configuration settings and data.
Provide a terminal interface via serial tty connection for system configuration and debugging

All of the above culminates into the primary objective: to play games!

Guidelines and rules for code:
*Absolutely no recursion! 
*All memory must be statically allocated.
*Initialize variables explicitly in functions, not implicitly (please).
*Follows Quake 2 code convention (see code_style.c).

The project was started in January 2013. Copyright Brandon Foltz. See license.txt for details.
