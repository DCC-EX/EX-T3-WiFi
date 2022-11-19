Import("env")
import os, inspect, uuid

# Generate an SSID and password file if one doesn't exist
ap_path = env.get("PROJECT_DIR") + "/include/ThrottleWiFiSettings.h"
if not os.path.exists(ap_path):
  with open(ap_path, 'w') as ap:
    inc = """
    #ifndef THROTTLE_WIFI_SETTINGS_H
    #define THROTTLE_WIFI_SETTINGS_H

    #define THROTTLE_AP_NAME "DCCEx-T3-{ssid}"
    #define THROTTLE_AP_PWD "{pwd}"

    #endif
    """
    ap.write(inspect.cleandoc(inc.format(ssid = uuid.uuid4().hex[:4], pwd = uuid.uuid4().hex[:8])))
