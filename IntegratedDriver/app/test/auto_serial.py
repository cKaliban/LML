import serial.tools.list_ports as scanner

ports = scanner.comports()
for port in ports:
    print("--------------[")
    print(port.device)
    print(port.manufacturer)
    print("]--------------")

# serial.tools
