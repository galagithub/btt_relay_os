Alternative Firmware for BTT Relay v1.2

ATTENTION!
The relay board contains high voltage area, which may reault in serious harm to the operator (even death).
The author assumes no liability which may come from using the software or by following the instructions included in this project.

It is using platform.io and written using C language.
All the build requirements should be automatically handled by platform.io.

To flah MCU (assumes vscode with platform.io installed):
1) connect 5V serial adapter to board 4 pin header reversing TX and RX
   Do not connect the 5V pin!
2) put a jumper on the reset header.
3) power on the relay board (high voltage is present!)
4) click "Upload" in platform.io controls, a new terminal will appear
5) when message "Cycling power: done" appears remove jumper from reset pins (high voltage may be present!)
6) wait for program to flash, check messages in terminal for errors

Current features:
- response time: 100ms - changes in conditions faster than this won't be detected
- startup delay: 45s - the relay will not trip is PS_ON is disconnected (or 0) and power has just been applied
- power off delay: 15s - time to allow the board/PC to shutdown cleanly

More details in the C code.
