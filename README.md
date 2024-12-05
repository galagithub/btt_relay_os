Alternative Firmware for BTT Relay v1.2

ATTENTION!
The relay board contains high voltage area, which may reault in serious harm to the operator (even death).
The author assumes no liability which may come from using the software or by following the instructions included in this project.

It is using platform.io and written using C language.
All the build requirements should be automatically handled by platform.io.
Development environment assumes you are using vscode with platform.io installed and have a 5V serial adapter to flash the MCU.

To flash MCU:
1) the relay board should have no power
2) connect 5V serial adapter to board 4 pin header reversing TX and RX, do not connect the 5V pin!
3) put a jumper on the reset header.
4) power on the relay board (high voltage is present!)
5) click "Upload" in platform.io controls, a new terminal will appear
6) when message "Cycling power: done" appears remove jumper from reset pins (high voltage may be present!)
7) wait for program to flash, check messages in terminal for errors

Current features:
- response time: 100ms - changes in conditions faster than this won't be detected
- startup delay: 60s - the relay will not trip is PS_ON is disconnected (or 0) and power has just been applied
- power off delay: 20s - time to allow the board/PC to shutdown cleanly
- recovery delay: 5s - recover with PS_ON after power off, blanking time to avoid fast power off/on cycle

Leds show state:
- green off and red off - no power to the relay
- green on and red on -  MCU is reset or in programming mode
- green on and red blinking - startup delay, power to system, only SC is available
- green on and red off - power to the system
- green blinking and red off - PS_ON triggered, power off delay
- green off and red on - no power to the system, relay is in power off state
- green alternate red blinking - SC triggered

More details in the C code.

Relay information can be found here: https://github.com/bigtreetech/BIGTREETECH-Relay-V1.2
MCU vendor web page: https://www.stcmicro.com/STC/STC15W204S.html

Alternative firmwares:
1) https://github.com/talv2010/biqu-relay-v1.2-firmware-modified
2) https://github.com/sobieh/bttrelay