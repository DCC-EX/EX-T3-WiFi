<h1>EX-T3-WiFi <small>(T3 = Tactile Touch Throttle)</small></h1>

## WiFi throttle for DCC-EX Command Stations

#### The CS will need to be WiFi compatible and version 4.1.3+

#
### ⚠ Please note before deciding to buy parts this has been a solo project and may have unknown bugs and require some tinkering, please report any issues on GitHub ⚠

### 🛈 Visual Studio Code and PlatformIO are required 🛈

#

## Parts
 - DFRobot FireBeetle 2 - DFR0654-F - https://www.dfrobot.com/product-2231.html (If you prefer a more permanent setup you could use the FireBeetle2 that doesn't have headers pre-soldered)
 - DFRobot Fermion: 3.5” 480x320 TFT LCD Capacitive Touchscreen - DFR0669 - https://www.dfrobot.com/product-2107.html
 - DFRobot Fermion: EC11 Rotary Encoder - SEN0235 - https://www.dfrobot.com/product-1611.html
 - DFRobot Fermion: LIS2DW12 Triple Axis Accelerometer - SEN0405 - https://www.dfrobot.com/product-2337.html **(Optional)**
 - Miniature Slide Switch SPDT - I used one from here https://www.hobbytronics.co.uk/slide-switch?search=switch
 to fit the case it'll need to be the same dimensions *(19mm long, 5.6mm wide, 5mm high and mounting holes 15mm apart)*
 - 3000mah\2000mah LiPo battery - https://shop.pimoroni.com/products/lipo-battery-pack?variant=20429082247 **⚠ This battery has the terminals the correct way but before connecting any battery check the wires are the right way round on the plug ⚠**
 - All screws are self tapping cap heads;
	- M2x5mm - x6 (+2 if using accelerometer) (+2 if using 3000mah battery)
	- M2x8mm - x6 (+2 if using 2000mah battery)
	- M3x6mm - x2
	- M3x20mm - x4
	<p align="center"><img src="docs/imgs/screws.jpg" width="256"></p>
 - Stranded and solid core wire depending on preference
 - If using headers then you'll need some right angle male, right angle female and standard female
 - Heatshrink for wires
 - MicroSD card (16GB is a good common size)

#
- [Assembly](/docs/assembly.md)
- [Uploading the code](/docs/code.md)
- [Throttle setup](/docs/setup.md)
- [Managing locos](/docs/locos.md)
- [Throttle menu](/docs/menu.md)
- [Throttle features](/docs/features.md)
- [Running Locos](/docs/running.md)
- [Decoder Programming](/docs/programming.md)
