# Uploading the Code

Start by uploading the embedded file system.\
On the PlatformIO tasks list click the `Upload Filesystem Image` task.
<p align="center">
  <img src="./imgs/code-filesystem.png" width="250" />
</p>

**Optional Accelerometer**\
If you're using an accelerometer you'll need to uncomment the flag in the `platformio.ini` file (Just remove the `;`)
```ini
build_flags =
	-DUSE_ACCELEROMETER
```

Now the throttle code can be uploaded.\
Either of the options below will upload the code to the FireBeetle but **Upload and Monitor** can help with debugging any issues.
<p align="center">
  <img src="./imgs/code-upload.png" width="250" />
</p>

On first compile a random access point name and password will be generated *(This can be changed if you want to set your own later on `include\ThrottleWiFiSettings.h`)*.\
*These can be viewed on the throttle so there's no need to remember them.*

# Command Station

If you want some extra functionality you'll need to update your **Command Station**.\
This step is optional and the throttle will still function without it but it won't be able to query the **CS** for the current power states.

Included is a file named `myFilter.cpp`, this needs to be copied to the folder that contains the **Command Station** code and uploaded to your **Command Station**.