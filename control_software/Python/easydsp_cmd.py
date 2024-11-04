# Perry Petiet
# 23-10-2024
# EasyDsp command line control.

import asyncio
import cmd
from bleak import BleakScanner
import easydsp

async def parseCommand(command):
    commandFound = False
    words = command.split()

    #Only commands
    if len(words) == 1:
        cmd = words[0]

        if cmd == "list":
            easydsp.printDevices()
            commandFound = True

        elif cmd == "scan":
            await easydsp.scan()
            commandFound = True

        elif cmd == "disconnect":
            commandFound = True
            result = await easydsp.disconnect()
            if result == True:
                print("Disconnected")

        elif cmd == "eqgetgain":
            commandFound = True
            gain = await easydsp.eqGetGain()
            print(gain)
            
        elif cmd == "eqgetstate":
            commandFound = True
            state = await easydsp.eqGetState()
            print(state)

        elif cmd == "eqgets":
            commandFound = True
            s = await easydsp.eqGetS()
            print(s)

        elif cmd == "eqgetq":
            commandFound = True
            q = await easydsp.eqGetQ()
            print(q)

        elif cmd == "eqgetbandwith":
            commandFound = True
            bandwith = await easydsp.eqGetBandwith()
            print(bandwith)

        elif cmd == "eqgetboost":
            commandFound = True
            boost = await easydsp.eqGetBoost()
            print(boost)

        elif cmd == "eqgetfreq":
            commandFound = True
            freq = await easydsp.eqGetFreq()
            print(freq)

        elif cmd == "eqgetfiltertype":
            commandFound = True
            filterType = await easydsp.eqGetFilterType()
            print(filterType)

        elif cmd == "eqgetphase":
            commandFound = True
            phase = await easydsp.eqGetPhase()
            print(phase)

        elif cmd == "getmux":
            commandFound = True
            mux = await easydsp.getMux()
            print(mux)

    #Commands + argument
    elif len(words) == 2:
        cmd = words[0]
        arg = words[1]

        if cmd == "connect":
            commandFound = True
            result = await easydsp.connect(arg)
            if result == True:
                print("Connected")
            else:
                print("Connection failed")

        elif cmd == "selectinput":
            commandFound = True
            result = await easydsp.selectinput(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "selectoutput":
            commandFound = True
            result = await easydsp.selectOutput(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "selecteq":
            commandFound = True
            result = await easydsp.selectEq(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "setmux":
            commandFound = True
            result = await easydsp.setMux(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsetgain":
            commandFound = True
            result = await easydsp.eqSetGain(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsetstate":
            commandFound = True
            result = await easydsp.eqSetState(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsets":
            commandFound = True
            result = await easydsp.eqSetS(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsetq":
            commandFound = True
            result = await easydsp.eqSetQ(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsetstate":
            commandFound = True
            result = await easydsp.eqSetBandwith(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsetstate":
            commandFound = True
            result = await easydsp.eqSetBoost(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsetfreq":
            commandFound = True
            result = await easydsp.eqSetFreq(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsetfiltertype":
            commandFound = True
            result = await easydsp.eqSetFilterType(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "eqsetphase":
            commandFound = True
            result = await easydsp.eqSetPhase(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

        elif cmd == "setmux":
            commandFound = True
            result = await easydsp.setMux(int(arg))
            if result == True:
                print("Success")
            else:
                print("Failed")

    if commandFound == False:
        print("Invalid command!")
    


# Main program
async def main():
    await easydsp.scan()
    
    easydsp.printDevices()
    
    print("\ntype \"exit\" to leave")

    running = True
    while(running):
        try:
            command = input()
        except:
            await easydsp.exit()
            break
        if command == "exit":
            running = False
            await easydsp.exit()
            break
        await parseCommand(command)

# Run main program
asyncio.run(main())