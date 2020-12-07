from serial import Serial
from .driver_frames import *
# if __name__ == "__main__":
#     import driver_frames as frames
# else:
#     import lib.driver_frames as frames
from serial.tools import list_ports
from threading import Thread


class SerialDriver(Serial):
    def __init__(self, port = None, baudrate = 115200, manufacturer = 'STMicroelectronics', idn_check = None, timeout = 0.3, **kwargs):
        self.manufacturer = manufacturer
        self.idn_check = idn_check
        super().__init__(port = port, baudrate = baudrate, timeout = timeout, **kwargs)
    
    def open(self):
        try:
            super().open()
            print("Connection opened.")
        except:
            print("Connection failed.")
            self.close()
    
    def close(self):
        if self.is_open:
            super().close()
            print("Connection closed.")


    def autoconnect(self):
        available_ports = self.scan()
        ports_iter = (port[0] for port in available_ports if port[1] == self.manufacturer)
        if self.is_open:
            print("Port is already opened, close the connection first.")
        else:
            while True:
                port = next(ports_iter, None)
                if port is None:
                    print("Device not found.")
                    break
                else:
                    print(self.manufacturer + " device found on " + port + ".")
                    print("Connection attempt...")
                    self.port = port
                    try:
                        self.open()
                    except:
                        print("Error occured during connection attempt.")
                    finally:
                        if self.is_open:
                            print("Connection successful on " + self.port + " with baudrate: " + str(self.baudrate))
                            if self.idn_check is not None:
                                frame = Frame.general_commands["Identify"]
                                write_buffer = self.pack_frame(frame)
                                self.write(write_buffer)
                                identity = self.read(8)
                                if identity == self.idn_check:
                                    print("Identity confirmed.")
                                    break
                                else:
                                    print("Incorrect device. Dropping connection.")
                                    self.close()
                            else:
                                break
                        else:
                            print("Could not open device. Connection aborted.")
                            self.close()

    @staticmethod
    def scan():
        ports = list_ports.comports()
        return [(port.device, port.manufacturer) for port in ports]
    
    @staticmethod
    def pack_frame(header = 0x0000, value = None, size = 8):
        if size >= 2:
            write_buffer = bytearray(size)
            write_buffer[0:2] = header.to_bytes(2, byteorder='big')
            if value is not None:
                write_buffer[2:-1] = value.to_bytes(2, byteorder = 'big')
        return write_buffer

    @staticmethod
    def parse_frame(frame):
        header = frame[0:2]
        subheader = frame[0]
        command = None
        value = None
        return command, value
    
    
    def execute(self, command):
        pass

                

if __name__ == "__main__":
    # print(SerialDriver.scan())
    test = SerialDriver(manufacturer = "Vyacheslav Frolov")
    test.autoconnect()
    # print(test.pack_frame(driver_frames.platform_commands["Identify"]))
    # write_buffer = bytearray(-1)
    # print(type(write_buffer))
    # test.write(write_buffer)
    # print(test.read(8))

