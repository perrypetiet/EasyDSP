import asyncio
from bleak import BleakScanner

# Fucntions
async def scan():
    devices = await BleakScanner.discover()

    return devices

# Main program
async def main():
    devices = await scan()

    for device in devices:
        print(device)

# Run main program
asyncio.run(main())