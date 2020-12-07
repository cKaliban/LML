import PySimpleGUI as sg 
import serial
import time
import threading
from GUIwrapper import *



class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

def read_serial(serial, stop):
    print(">> Connection active\n")
    while True:
        if stop():
            break
        if serial.inWaiting():
            rx_byte = serial.read(8)
            print(">> ", rx_byte)
    print(">> Disconnected from the driver\n")

sg.ChangeLookAndFeel("Dark2")

serial_settings = sg.Frame("Serial", [
            [Txt("Serial Port"), ITxt('COM5', key='_PORT_')], 
            [Txt("Baud Rate"), ITxt('115200', key='_BAUD_')],
            [Btn('Open'), Btn('Close')]
])

quick_control = sg.Col([
    [sg.Button("Next Layer", disabled=True, key = "_NXT_LAYER_", size = (10,2))],
    [sg.Button("Finish Work", disabled=True, key = "_END_WORK_", size = (10,1))],
    [sg.Button('STOP', button_color=('#FFFFFF','#FF0000'), size=(10,2))]
])

out = sg.Column([
    [sg.Output(size=(40, 20))],
    [sg.Checkbox("Enable Control", key="_ENABLE_CONTROL_")],
    [serial_settings, quick_control]
])

queries = sg.Frame("General", [
    [Btn('IDN?', key = "_IDN_"), Btn('Calibrate', key = "_CAL_")],
    [Btn('Home', key = "_HOME_"), Btn('Test', key = "_TEST_")],
])

general = sg.Column([
    [queries]
])

x_queries  = sg.Frame("X axis control", [
    [Btn('Toggle', key = "_TOGGLE_"), Btn('Calibrate X', key = "_CAL_X_")],
    [Btn('Home X', key = "_HOME_X_"), Btn('Test X', key = "_TEST_X_")],
])

x_settings = sg.Frame("X axis settings",[
    [Txt("Full Speed [Hz]"), ITxt("1000", key = "_SPEED_")]
])


xaxis = sg.Column([
    [x_queries],
    [x_settings]
    ])

y_queries = sg.Frame("Y axis control", [
    [Btn('Clean', key = "_CLEAN_"), Btn('Calibrate Y', key = "_CAL_Y_")],
    [Btn('Home Y', key = "_HOME_Y_"), Btn('Test Y', key = "_TEST_Y_")],
])

y_settings = sg.Frame("Y axis settings",[
    # [Txt("dy [um]"), ITxt("5")],
    [Txt("1st layer [um]"), ITxt("5", key = "_FIRST_LAYER_")],
    [Txt("layers [um]"), ITxt("5", key = "_LAYERS_")]
])

yaxis = sg.Column([
    [y_queries],
    [y_settings],
    [Txt("  "), sg.Submit(size = (10,2))]
])

axes = sg.Column([
    [general],
    [xaxis],
    [yaxis]
])

layout = [
    [out, axes]
]

if __name__ == "__main__":
    window = sg.Window("Motor Control", layout)
    ser = serial.Serial(write_timeout = 0)
    

    # Frames 
    # General
    IDN = 0x1D1D
    SUBMIT = 0x5EED
    CALIBRATE = 0x5EAB
    HOME = 0xAB00
    TEST = 0xCECC
    STOP = 0xDEAD
    
    # X control
    CALIBRATE_X = 0xAA5E
    HOME_X = 0xAA00
    TEST_X = 0xAACE
    TOGGLE = 0xAA66
    
    # Y control
    CALIBRATE_Y = 0xBB5E
    HOME_Y = 0xBB00
    TEST_Y = 0xBBCE
    CLEAN = 0xBB66
    
    # 
    # SUBMIT = 0xF00D
    NEXT_LAYER = 0x6060
    FINISH_WORK = 0xF1FF


    write_buffer = bytearray(8)
    while True:
        event, values = window.read(50)
        
        if event not in ("Submit", "__TIMEOUT__"):
            print("<< ", event, '\n')

        if values is not None:
            if values["_ENABLE_CONTROL_"] == True:
                window["_NXT_LAYER_"].update(disabled=False)
                window["_END_WORK_"].update(disabled=False)
            else:
                window["_NXT_LAYER_"].update(disabled=True)
                window["_END_WORK_"].update(disabled=True)

        if event in (None, 'Exit'):
            if ser.is_open:
                stop_read = True
                ser.close()
                if not ser.is_open:
                    print(">> Port Closed")
            break
        # Serial port configuration
        elif event is 'Open':
            try:
                if not ser.is_open:
                    ser.port = values["_PORT_"]
                    ser.baudrate = values["_BAUD_"]
                    ser.open()
                    read_task = threading.Thread(target=read_serial, args = (ser, lambda: stop_read, ))
                    stop_read = False
                    if ser.is_open:
                        print(">> Port Opened: " + values["_PORT_"])
                        read_task.start()
            except:
                break
        
        elif event is 'Close':
            if ser.is_open:
                stop_read = True
                ser.close() 
                if not ser.is_open:
                    print(">> Port Closed")
        
        # General
        elif event is "Submit":
            print("<< New Settings:")
            print("1st layer depth [um] = ", values["_FIRST_LAYER_"])
            print("Other layers depth [um] = ", values["_LAYERS_"])

            write_buffer[:2] = SUBMIT.to_bytes(2, byteorder = 'big')
            freq = int(values["_SPEED_"])
            # period = 1000000/freq
            # if period > 65536:
            #     period = 2000
            # if period < 100:
            #     period = 100
            # write_buffer[2:4] = int(period - 1).to_bytes(2, byteorder = 'big')
            if freq > 2000:
                freq = 2000
            if freq < 500:
                freq = 500
            write_buffer[2:4] = int(freq).to_bytes(2, byteorder = 'big')

            # print("Speed[Hz] = ", 1000000/period, "\n")
            # TODO Calculate steps for single layer, set direct number of steps per layer
            calibration_value = 100 # assume 100 steps per um
            first_layer = int(values['_FIRST_LAYER_']) * calibration_value
            other_layers = int(values['_LAYERS_']) * calibration_value
            write_buffer[4:6] = int(first_layer).to_bytes(2, byteorder = 'big')
            write_buffer[6:8] = int(other_layers).to_bytes(2, byteorder = 'big')

        elif event is "STOP":
            write_buffer[:2] = STOP.to_bytes(2, byteorder = 'big')
            
        elif event is "_IDN_":
            write_buffer[:2] = IDN.to_bytes(2, byteorder = 'big')
            

        # Calibration procedures
        elif event is "_CAL_":
            write_buffer[:2] = CALIBRATE.to_bytes(2, byteorder = 'big')
            
        elif event is "_CAL_X_":
            write_buffer[:2] = CALIBRATE_X.to_bytes(2, byteorder = 'big')
            
        elif event is "_CAL_Y_":
            write_buffer[:2] = CALIBRATE_Y.to_bytes(2, byteorder = 'big')

            
        
        # Homing procedures
        elif event is "_HOME_":
            write_buffer[:2] = HOME.to_bytes(2, byteorder = 'big')
            
        elif event is "_HOME_X_":
            write_buffer[:2] = HOME_X.to_bytes(2, byteorder = 'big')
            
        elif event is "_HOME_Y_":
            write_buffer[:2] = HOME_Y.to_bytes(2, byteorder = 'big')
            

        # Testing procedures
        elif event is "_TEST_":
            write_buffer[:2] = TEST.to_bytes(2, byteorder = 'big')
            
        elif event is "_TEST_X_":
            write_buffer[:2] = TEST_X.to_bytes(2, byteorder = 'big')
            
        elif event is "_TEST_Y_":
            write_buffer[:2] = TEST_Y.to_bytes(2, byteorder = 'big')
            
        
        # Other
        elif event is "_CLEAN_":
            write_buffer[:2] = CLEAN.to_bytes(2, byteorder = 'big')

        elif event is "_TOGGLE_":
            write_buffer[:2] = TOGGLE.to_bytes(2, byteorder = 'big')
        
        # Control
        elif event is "_NXT_LAYER_":
            write_buffer[:2] = NEXT_LAYER.to_bytes(2, byteorder = 'big')
        elif event is "_END_WORK_":
            write_buffer[:2] = FINISH_WORK .to_bytes(2, byteorder = 'big')
        
        if write_buffer[:] != bytearray(8) and ser.is_open:
            try:
                ser.write(write_buffer)
            except serial.SerialTimeoutException as handler:
                print(handler)
                break

            # time.sleep(10)
            write_buffer[:] = bytearray(8)
            
            

