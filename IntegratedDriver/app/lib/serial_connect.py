import serial
import serial.tools.list_ports as serial_scanner

Frame_type = {
    "WARNING" : 0
}

class Frame:
    pass 

class SerialDriver:
    def __init__(self, port = None, baudrate = 115200):
        self.handler = serial.Serial(port, baudrate)
        pass

    @staticmethod
    def scan():
        ports = serial_scanner.comports()
        return [(port.device, port.manufacturer) for port in ports]
        
    def autoconnect(self, manufacturer = 'STMicroelectronics', baudrate = None, check = None):
        ports = self.scan()
        driver_port = next((port[0] for port in ports if port[1] == manufacturer), None)
        if driver_port is not None:
            print(manufacturer + " device found on " + driver_port)
            print("Connection attempt...")
            self.handler.port = driver_port
            if baudrate is not None:
                self.baudrate = baudrate
            self.handler.open()
            
            if self.handler.is_open:
                print("Connection successful.")
                if check is not None:
                    print("Identity check:")
            else:
                print("Connection attempt failed. Check device.")
        else:
            print("Device not found. Auto-connect failed.")
        return self.handler
    
    def manual_connect(self, port, baudrate):
        self.handler.port = port
        self.handler.baudrate = baudrate
        print("Connection attempt on " + port)
        self.handler.open()
        if self.handler.is_open:
            print("Connection successful.")
        else:
            print("Connection attempt failed.")

    def open(self):
        if not self.handler.is_open:
            self.handler.open()
            if self.handler.is_open:
                print("Connection ")
        pass
    
    def close(self):
        if self.handler.is_open:
            self.handler.close()
        pass

    


if __name__ == "__main__":
    # print(Serial.scan())
    com_handler = SerialDriver()
    print(com_handler.autoconnect())