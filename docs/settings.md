# Throttle Settings

These are under the **Settings** button from the menu.\
Settings are across multiple pages, you can scroll through them by using the paging buttons at the bottom.

## Loco UI

**Speed Step** - Can be 1, 2 or 4. This is the step value used to control loco speeds. *This is for the throttle only and not related to DCC speed steps*.

Some basic swipe gestures are supported on the throttle, Up, Down, Left & Right.\
Each gesture can trigger an action.

> Gestures only work while on the loco display

* **Keypad** - Show the keypad to select loco address.
* **Name** - Show the list of locos the throttle has configs for.
* **Group** - Show the loco groups the throttle has configs for.
* **Next** - If multiple locos have been selected then control the next.
* **Prev** - If multiple locos have been selected then control the previous.
* **Release** - Release the current loco.

> Locos are remembered in the order they were selected.\
So **Next** and **Prev** will follow this order.

**After swipe release**\
The action to perform if a swipe action is set to **release**.\
If this is set to **Next** or **Prev** and there's no locos the throttle will go back to the menu.

## Screen Rotation

* **Standard** - Screen above with rotary encoder below
* **Inverted** - Screen at below with rotary encoder above.
* **Accelerometer** - *Only if used*, display will rotate with pyhsical orientation.

## Pin Protect

This is just some basic security if you're concerned about guests being able to make changes to your settings.\
A 9 digit max pin can be set which will be required to access the `T3 WiFi` and `Settings` displays.

If the button is idle and reads `Not Set` no pin is set, you can set one by pressing the button which will show a keypad.\
Enter a pin and press `Enter` to set it or `Cancel` to go back to the settings.

To remove a pin press the button when it reads `Pin Set` to show the keypad then just press `Enter`. This will remove the pin and the button will change to `Not Set`.

> If you forget your pin it's stored in the `settings.json` file on the MicroSD card so can be read or reset.

## Emergency Stop Trigger Hold Duration

The milisecond count the encoder is pressed for to trigger an emergency stop of all locos (can be between 1000 - 5000 miliseconds).
