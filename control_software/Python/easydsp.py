# Perry Petiet
# 23-10-2024
# EasyDsp python module. Uses bleak for BLE python functionality.
# This module contains functions to connect to and control EasyDsp.

from bleak import BleakScanner
from bleak import BleakClient
import serv_char
import asyncio
import cmd

devices = None
scanned = False
client  = None

intSize = 4

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
        return True
    return False

async def disconnect():
    if client != None:
        await client.disconnect()
        return True
    return False

async def selectinput(inputNum):
    if client != None:
        dataNum    = bytes([inputNum])
        dataOutput = bytes([int(0)])
        await client.write_gatt_char(char_specifier=serv_char.char_chan_index, data=dataNum,    response= True)
        checkInput    = await client.read_gatt_char(char_specifier=serv_char.char_chan_index)
        await client.write_gatt_char(char_specifier=serv_char.char_is_output,  data=dataOutput, response= True)
        checkIsOutput = await client.read_gatt_char(char_specifier=serv_char.char_is_output)
        if checkIsOutput == dataOutput:
            if checkInput == dataNum:
                return True
    return False

async def selectOutput(outputNum):
    if client != None:
        dataNum    = bytes([outputNum])
        dataOutput = bytes([int(1)])
        await client.write_gatt_char(char_specifier=serv_char.char_is_output,  data=dataOutput, response= True)
        checkIsOutput = await client.read_gatt_char(char_specifier=serv_char.char_is_output)
        await client.write_gatt_char(char_specifier=serv_char.char_chan_index, data=dataNum,    response= True)
        checkNum    = await client.read_gatt_char(char_specifier=serv_char.char_chan_index)
        if checkIsOutput == dataOutput:
            if checkNum == dataNum:
                return True
    return False

async def selectEq(eqNum):
    if client != None:
        data = bytes([eqNum])
        await client.write_gatt_char(char_specifier=serv_char.char_eq_index,  data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_eq_index)
        if checkData == data:
            return True
    return False

#
# EQ GET AND SET FUNCTIONS:
#

async def eqSetState(state):
    if client != None:
        data = bytes([state])
        await client.write_gatt_char(char_specifier=serv_char.char_state, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_state)
        print(checkData)
        if checkData == data:
            return True             
    return False

async def eqGetState():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_state)
    return int.from_bytes(rx, "little")

async def eqSetGain(gain):
    if client != None:
        data = gain.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_gain, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_gain)
        if checkData == data:
            return True             
    return False

async def eqGetGain():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_gain)
    return int.from_bytes(rx, "little")

async def eqSetS(s):
    if client != None:
        data = s.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_s, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_s)
        if checkData == data:
            return True             
    return False

async def eqGetS():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_s)
    return int.from_bytes(rx, "little")

async def eqSetQ(q):
    if client != None:
        data = q.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_q, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_q)
        print(checkData)
        if checkData == data:
            return True             
    return False

async def eqGetQ():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_q)
    return int.from_bytes(rx, "little")

async def eqSetBandwith(bandwith):
    if client != None:
        data = bandwith.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_bandwith, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_bandwith)
        if checkData == data:
            return True             
    return False

async def eqGetBandwith():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_bandwith)
    return int.from_bytes(rx, "little")

async def eqSetBoost(boost):
    if client != None:
        data = boost.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_boost, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_boost)
        if checkData == data:
            return True             
    return False

async def eqGetBoost():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_boost)
    return int.from_bytes(rx, "little")

async def eqSetFreq(freq):
    if client != None:
        data = freq.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_freq, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_freq)
        if checkData == data:
            return True             
    return False

async def eqGetFreq():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_freq)
    return int.from_bytes(rx, "little")

async def eqSetFilterType(filter_type):
    if client != None:
        data = filter_type.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_filt_type, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_filt_type)
        if checkData == data:
            return True             
    return False

async def eqGetFilterType():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_filt_type)
    return int.from_bytes(rx, "little")

async def eqSetPhase(phase):
    if client != None:
        data = phase.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_phase, data=data, response= True)
        checkData = await client.read_gatt_char(char_specifier=serv_char.char_phase)
        if checkData == data:
            return True             
    return False

async def eqGetPhase():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_phase)
    return int.from_bytes(rx, "little")

async def setMux(muxVal):
    if client != None:
        dataMux = muxVal.to_bytes(intSize, "little")
        await client.write_gatt_char(char_specifier=serv_char.char_mux_val, data=dataMux, response= True)
        checkMux = await client.read_gatt_char(char_specifier=serv_char.char_mux_val)
        if checkMux == dataMux:
            return True
    return False

async def getMux():
    if client != None:
        rx = await client.read_gatt_char(char_specifier=serv_char.char_mux_val)
    return int.from_bytes(rx, "little")