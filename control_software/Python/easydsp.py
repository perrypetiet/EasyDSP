from bleak import BleakScanner
from bleak import BleakClient
import asyncio
import cmd

devices = None
scanned = False
client  = None

async def exit():
    await disconnect()

async def scan():
    global devices 
    global scanned
    devices = await BleakScanner.discover(timeout = 1.5, return_adv = True)
    scanned = True
    print("Found ", len(devices), " devices.")

def printDevices():
    if scanned == True:
        print("\n")
        for device in devices.values():
            bleDevice = device[0]
            advData   = device[1]
            print("****** BLE DEVICE ******")
            print(advData.local_name)
            print("RSSI    ", advData.rssi)
            print("address ", bleDevice.address)
        print("\n")

async def connect(addr_or_name):
    global client
    global scanned
    connected = False
    if scanned == True:
        for device in devices.values():
            bleDevice = device[0]
            advData  = device[1]
            if advData.local_name == addr_or_name:
                client = BleakClient(bleDevice)
                await client.connect()
                connected = True
            elif bleDevice.address == addr_or_name:
                client = BleakClient(bleDevice)
                await client.connect()
                connected = True
    
    if connected == True:
        services = client.services
        
        #todo: check services and characteristics.

        print(len(services.characteristics))
        print("Connected!\n")

async def disconnect():
    if client != None:
        await client.disconnect()
        print("Disconnected!\n")
