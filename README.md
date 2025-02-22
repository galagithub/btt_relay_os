# Alternative Firmware for BTT Relay v1.2

> [!CAUTION]  
> <p>The relay board contains high voltage area, which may result in serious harm to the operator (<strong>even death</strong>).</p>
> <p>The author assumes no liability which may come from using the software or by following the instructions included in this project.</p>

### How it works:
1) AC power on to relay
2) relay powers the printer main power supply
3) after 60 seconds the relay looks for PS_ON signal to be high (over 1 Volt)
4) if PS_ON if high, then relay goes to "<strong>running</strong>" state and awaits PS_ON to go low (0 Volts)
5) if PS_ON is low, then the relay will go to "<strong>power delay</strong>" state, which means it will shut down after 20 seconds
6) when the relay shuts down (cuts power to the printer) it will go into "<strong>recovery delay</strong>" state for 30 seconds
7) after recovery delay, the relay will go into "<strong>tripped</strong>" state, where it will reset itself if PS_ON signal goes to high

> [!TIP]
> <p>At any time, if enabled, the short-circuit protection will engage when 5V_IN signal is lower than 4.5 Volts. Power to the printer will be cut until reset or full poweroff.</p>

> [!TIP]
> <p>The relay can be reset at any time from any state by shorting the RESET pin on board to relay 5V pin momentarily. If these 2 pins are always shorted, then the relay is disabled, thus providing power to the printer with no control over it. Relay states are signaled via on-board leds.</p>

### To build:
1) install git
2) install vscode
3) install PlatformIO extension
4) chekout code using preferred method
5) use the PlatformIO tab to build all targets

<p>Written using C language. All the build requirements are automatically handled by PlatformIO.</p>
<p>Development environment assumes you have a 5V serial adapter to flash the MCU (to use the Upload feature form PlatformIO). The oficial upload tool is available on the MCU vendor web page.</p>
<p>Uses [mgoblin/STC15lib](https://github.com/mgoblin/STC15lib) MCU library.</p>

> [!NOTE]
> Due to an upstream bug in the MCU library, there is no support for building on Windows.
> Building on MacOS not yet tested.

### To flash MCU:
1) the relay board should have no power
2) connect 5V serial adapter to board 4 pin header reversing TX and RX, do not connect the 5V pin!
3) put a jumper on the reset header.
4) power on the relay board (high voltage is present!)
5) click "Upload" in platform.io controls, a new terminal will appear
6) when message "Cycling power: done" appears remove jumper from reset pins (high voltage may be present!)
7) wait for program to flash, check messages in terminal for errors

> [!NOTE]
> <p>You can find hex files ready to flash under "<strong>bin</strong>" folder. This folder is manually updated for now.</p>

### Current features:
- response time: 100ms - changes in conditions faster than this won't be detected, limit to max 500ms
- startup delay: 60s - the relay will not trip is PS_ON is disconnected (or 0) and power has just been applied
- power off delay: 20s - time to allow the board/PC to shutdown cleanly
- recovery delay: 30s - blanking time to avoid fast power off/on cycle, after which relay can recover with PS_ON (self-reset)

### Leds show state:
- green off and red off - no power to the relay
- green on and red on -  MCU is reset or in programming mode
- green on and red blinking - startup delay, power to system, only SC is available
- green on and red off - power to the system
- green blinking and red off - PS_ON triggered, power off delay
- green and red blinking - no power to the system, wait for recovery delay
- green off and red on - no power to the system, relay is in power off state
- green alternate red blinking - SC triggered

More details in the C code.

### Information regarding HW:
- BTT docs: https://github.com/bigtreetech/BIGTREETECH-Relay-V1.2
- MCU vendor web page: https://www.stcmicro.com/STC/STC15W204S.html

### Alternative firmwares:
1) https://github.com/talv2010/biqu-relay-v1.2-firmware-modified
2) https://github.com/sobieh/bttrelay

### Related projects
- Display PCB: https://github.com/galagithub/btt_relay_display
