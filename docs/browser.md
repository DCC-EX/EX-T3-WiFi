# Web Browser Uploading

> ⚠ Web browser uploading only works with Chromium based web browsers such as Chrome, Opera & Edge ⚠

Download the lastest release from https://github.com/DCC-EX/EX-T3-WiFi/releases (`firmware.bin`) then go to https://espressif.github.io/esptool-js/

Plug in the throttle to your computer and click the `Connect` button (You'll be prompted to select the COM port the throttle is on).

If the throttle has been detected it should read **Connected to device**.

On the `Flash Address` field change the value to `0x0` then click the `Choose File` button and select the `firmware.bin` file you downloaded earlier.

You can now click the `Program` button to begin uploading.\
The upload progress will show below.\
Wait until it gets to 100% and finishes with the output of `Leaving...` before disconnecting the throttle.
