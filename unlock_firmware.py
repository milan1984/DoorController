import subprocess
import os
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

def unlock_firmware():

    # Path to the ST-Link command line tool
    stlink_path = find_stlink_cli()
    print("Found:", stlink_path)
    
    # Check if the ST-Link CLI tool exists
    if not os.path.exists(stlink_path):
        print(f"Error: ST-Link CLI tool not found at {stlink_path}")
        return
    
    # Command to unlock the firmware
    unlock_command = [stlink_path, "-c", "SWD", "-OB", "RDP=0"]

    try:
        # Execute the command
        result = subprocess.run(unlock_command, capture_output=True, text=True)
        
        # Check if the command was successful
        if result.returncode == 0:
            print("Firmware unlocked successfully.")
        else:
            print("Failed to unlock firmware.")
            print("Error:", result.stderr)
    except Exception as e:
        print("An error occurred:", str(e))


def before_upload(source, target, env):
    print("Running unlock_firmware.py script...")
    unlock_firmware()

if __name__ == "__main__":
    unlock_firmware()
else:
    from SCons.Script import Import
    Import("env")
    env.AddPreAction("upload", before_upload)