# TacomeBetterHead
For my 2014 Toyota Tacoma head unit so that brightness is tied to the dash brightness as well as supporting multiple camera inputs.

The stock entertainment head unit has switchable screen brightness settings, one for day and the other for night. BUT, the
input that selects one or the other is the headlight switch. Which means the unit is in nighttime (dark) when running the 
headlights in the daytime. What?! This code looks at the dash PWM brightness signal to determine a switch point instead.

This code also selects between one of three camera inputs to show. In my case, that's the stock backup camera, a camera on the
back of our camp trailer, or a forward looking camera in the grill.

Intended for the Arduino platform, first installed version on the UNO.
