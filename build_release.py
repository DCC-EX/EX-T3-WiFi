import sys
import os
from os.path import join

env = DefaultEnvironment()
platformIO = env.PioPlatform()
sys.path.append(join(platformIO.get_package_dir('tool-esptoolpy')))
import esptool

def buildRelease(*args, **kwargs):
  frameworkDir = platformIO.get_package_dir('framework-arduinoespressif32')
  projectDir = env.subst('$PROJECT_DIR')
  buildDir = env.subst('$BUILD_DIR')

  # Build firmware
  os.system('pio run -e dfrobot_firebeetle2_esp32e')
  # Build SPIFFS
  os.system('pio run -t buildfs -e dfrobot_firebeetle2_esp32e')

  cmd = [
    '--chip', 'esp32',
    'merge_bin',
    '-o', join(projectDir, 'firmware.bin'),
    '--flash_mode', 'dio',
    '--flash_size', '4MB',
    '--flash_freq', '40m',
    '0x1000', join(buildDir, 'bootloader.bin'),
    '0x8000', join(buildDir, 'partitions.bin'),
    '0xe000', join(frameworkDir, 'tools', 'partitions', 'boot_app0.bin'),
    '0x10000', join(buildDir, 'firmware.bin'),
    '0x210000', join(buildDir, 'spiffs.bin')
  ]
  # Merge firmware and SPIFFS into single binary
  esptool.main(cmd)

env.AddCustomTarget('buildrelease', None, buildRelease, 'Build Release')
