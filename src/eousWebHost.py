'''
WEBHOST CODE

Code written around the following hardware:
- Jetson Nano 4GB (NOT THE ORIN) (PYTHON 3.6)
- The Jetbot addon
- Plug Mainboard into USB port and use Serial

Code by Klef Heavy Industries 2026

THIS CODE IS PROVIDED AS-IS WITH NO WARRANTY

LICENCE AS REQUIRED
'''

# import network
import asyncio
import socket
import time
import random
import serial

# import sensors
import ina219 as ina # THIS IS REQUIRED FOR BATTERY

# declare sensor
wah = ina.INA219(addr=0x41)

# Declare mainboard serial
# main_b = serial.Serial(port='COM4', baudrate=115200, timeout=.1)
main_b = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=.1)

# Variables
state = "OFF"
random_value = 0

# HTML template for the webpage
def webpage(random_value, state):
    html = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>eous-OS Web Server</title>
            <meta name="viewport" content="width=device-width, initial-scale=1">
        </head>
        <body>
            <h1>eous-OS Web Server</h1>
            <h2>Modes</h2>
            <form action="./m_normal">
                <input type="submit" value="Normal Mode" />
            </form>
            <br>
            <form action="./m_wave">
                <input type="submit" value="Wave Mode" />
            </form>
            <br>
            <form action="./m_aaaa">
                <input type="submit" value="AAAA Mode" />
            </form>
            <br>
            <form action="./m_reboot">
                <input type="submit" value="Reboot Mode" />
            </form>
            <br>
            <form action="./m_loading">
                <input type="submit" value="Loading Mode" />
            </form>
            <br>
            <form action="./m_sleep">
                <input type="submit" value="Sleep Mode" />
            </form>
            <br>
            <form action="./m_look">
            <input type="submit" value="Look Mode" />
            </form>
            <br>
            <form action="./m_auto_look">
            <input type="submit" value="Auto Look Mode" />
            </form>
            <p>Bangboo state: {state}</p>
            <h2>Fetch New Value</h2>
            <form action="./value">
                <input type="submit" value="Fetch value" />
            </form>
            <p>Fetched value: {random_value}</p>
        </body>
        </html>
        """
    return str(html)

# Async function for client requests
async def handle_client(reader, writer):
    global state

    print("Client connected")
    request_line = await reader.readline()
    print('Request:', request_line)

    # Skip HTTP headers
    while await reader.readline() != b"\r\n":
        pass

    request = str(request_line, 'utf-8').split()[1]
    print('Request:', request)

    # Process request, update variables
    if request == '/m_normal?':
        print('Normal Mode Received')
        state = '0,0,0'
    elif request == '/m_wave?':
        print('Wave Mode')
        state = '0,0,1'
    elif request == '/m_aaaa?':
        print('AAAA Mode')
        state = '0,0,2'
    elif request == '/m_reboot?':
        print('Reboot Mode')
        state = '0,0,3'
    elif request == '/m_loading?':
        print('Loading Mode')
        state = '0,0,4'
    elif request == '/m_sleep?':
        print('Sleep Mode')
        state = '0,0,5'
    elif request == '/m_look?':
        print('Lock Mode')
        state = '0,0,6'
    elif request == '/m_auto_look?':
        print('Auto Look Mode')
        state = '0,0,7'
    elif request == '/value?':
        global random_value
        #random_value = random.randint(0,20)
        random_value = wah.getBusVoltage_V()
    else: 
        print('Unknown Operation')
        state = '9,9,9'

    if state == '9,9,9':
        print("No command")
    else:
        main_b.write(bytes(state, 'utf-8'))
        print("Command:" , state)

    # Generate HTML response
    response = webpage(random_value, state)

    # Send the response and close the connection
    writer.write('HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n'.encode("utf-8"))
    writer.write(response.encode("utf-8"))
    await writer.drain()
    ## await writer.wait_closed() # 3.14
    writer.close() # 3.6
    await asyncio.sleep(0.01) # 3.6
    print('Client Disconnected')

# Main
async def main():
    #Start server
    print('Setting up server')
    server = asyncio.start_server(handle_client, "0.0.0.0", 80)

    loop = asyncio.get_event_loop()
    task_server = loop.create_task(server)
    await task_server
    while True:

        #Loop

        await asyncio.sleep(5)
        print('Heartbeat (very 5 secs)')

# Event Loop (3.6)
## Deprecated for 3.14, check if jetson can handle
loop = asyncio.get_event_loop()

## Create task for main function
loop.create_task(main())

# Use this for testing:

try:
    # asyncio.run(main()) #3.14
    loop.run_forever() #3.6
except Exception as e:
    print('An Error Occurred: ', e)
except KeyboardInterrupt:
    print('Program Terminated by User')