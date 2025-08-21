import subprocess
import os
import time
import shutil
import winreg

def find_stlink_cli():
    # 1. Try PATH
    exe = shutil.which("ST-LINK_CLI.exe")
    if exe and os.path.exists(exe):
        return exe

    # 2. Try Registry
    reg_paths = [
        r"SOFTWARE\WOW6432Node\STMicroelectronics\STM32 ST-LINK Utility",
        r"SOFTWARE\STMicroelectronics\STM32 ST-LINK Utility"
    ]
    for reg_path in reg_paths:
        try:
            key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, reg_path)
            value, _ = winreg.QueryValueEx(key, "InstallPath")
            exe = os.path.join(value, "ST-LINK_CLI.exe")
            if os.path.exists(exe):
                return exe
        except FileNotFoundError:
            continue

    # 3. Try default install locations
    common_paths = [
        r"C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe",
        r"C:\Program Files\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility\ST-LINK_CLI.exe",
    ]
    for path in common_paths:
        if os.path.exists(path):
            return path

    return None

def lock_firmware():
    # Path to the ST-Link command line tool
    stlink_path = find_stlink_cli()
    print("Found:", stlink_path)
    
    # Check if the ST-Link CLI tool exists
    if not os.path.exists(stlink_path):
        print(f"Error: ST-Link CLI tool not found at {stlink_path}")
        return
    
    # Command to lock the firmware
    lock_command = [stlink_path, "-c", "SWD", "-OB", "RDP=1"]
    reset_command = [stlink_path, "-c", "SWD", "-Rst"]

    try:
        # Execute the lock command
        result = subprocess.run(lock_command, capture_output=True, text=True)
        
        # Check if the command was successful
        if result.returncode == 0:
            print("Firmware locked successfully.")
            # Add a delay to ensure the MCU has time to reset
            time.sleep(1)
            # Execute the reset command
            result = subprocess.run(reset_command, capture_output=True, text=True)
            # Check if the command was successful
            if result.returncode == 0:
                print("MCU reset successfully.")
            else:
                print("Failed to reset MCU.")
                print("Error:", result.stderr)

        else:
            print("Failed to lock firmware.")
            print("Error:", result.stderr)
    except Exception as e:
        print("An error occurred:", str(e))


def after_upload(source, target, env):
    print("Running lock_firmware.py script...")
    lock_firmware()

if __name__ == "__main__":
    lock_firmware()
else:
    from SCons.Script import Import
    Import("env")
    env.AddPostAction("upload", after_upload)