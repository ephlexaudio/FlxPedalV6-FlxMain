# FlxPedalV6-FlxMain

This is the main process running on the Raspberry Pi Compute Module. It launches FlxPedalUi that runs the LCD, buttons and rotary encoder, using request and response FIFOs and shared mapped memory for IPC with FlxPedalUi.  It also reads combo files containing JSON data and processes their data into running, interconnected signal processes.

This is the 6th Version of the pedal itself.  A new version generally means a change in hardware, or at least an addition or removal of a linux process.


