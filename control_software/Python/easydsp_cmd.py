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

        if cmd == "scan":
            await easydsp.scan()
            commandFound = True

        if cmd == "disconnect":
            await easydsp.disconnect()
            commandFound = True

    #Commands + argument
    if len(words) == 2:
        cmd = words[0]
        arg = words[1]

        if cmd == "connect":
            await easydsp.connect(arg)
            commandFound = True     
    
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