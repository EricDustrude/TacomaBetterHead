# TacomaBetterHead
For my 2014 Toyota Tacoma head unit so that brightness is tied to the dash brightness as well as supporting multiple camera inputs.

The stock entertainment head unit has switchable screen brightness settings, one for day and the other for night. BUT, the
input that selects one or the other is the headlight switch. Which means the unit is in nighttime (dark) when running the 
headlights in the daytime. What?! This code looks at the dash PWM brightness signal to determine a switch point instead.

This code also selects between one of three camera inputs to show. In my case, that's the stock backup camera, a camera on the
back of our camp trailer, or a forward looking camera in the grill.

Intended for the Arduino platform, first installed version on the UNO.

## Hardware design for the UNO Shield

I used [ExpressPCB Windows software](https://www.expresspcb.com/) to do the shield layout, and then had Express PCB make
sample boards. You can use this to print and make your own, or do as I did and upload the file and have beautiful boards
mailed to you.

![Express PCB board layout](/ExpressPCB/ExpressPCB-Taco.png)

## Nearly completed shield

![Populated Shield](/ExpressPCB/Shield.jpeg)

## Parts List

The board provides for 5 relays, 3 to select the camera input, one to trigger the reverse input to the head unit, and the last
to switch day/night display mode.

There's a 5volt regulator on board to power the relays, which are switched from Arduino digital pins through transitors.

Camera input selection
* 5ea [Relay](https://www.sparkfun.com/products/100)
* 5ea [2N3904 transistor](https://www.sparkfun.com/products/521)
* 5ea [1N4004 diode](https://www.sparkfun.com/products/14884)
* 5ea 1K 1/4 watt [resistor](https://www.sparkfun.com/products/10969)

Voltage dividers for switch/dial inputs (Rev & PWM)
* 2ea 4.7K 1/4 watt resistor
* 2ea 10k 1/4 watt resistor

Resistors to prevent input float
* 5ea 10k 1/4 watt resistor (On, Sel, Vin signals - some were soldered on the underside of the board and are not visible above)

7805 voltage regulator and capacitors
