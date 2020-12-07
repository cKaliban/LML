import PySimpleGUI as sg 
import serial
import time
import threading
import json
import os.path
from lib.GUIwrapper import *



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
        if serial.in_waiting > 0:
            rx_byte = serial.read(size = 8)
            print(">> ", rx_byte)
    print(">> Disconnected from the driver\n")

sg.ChangeLookAndFeel("Dark2")

serial_settings = sg.Frame("Serial", [
            [sTxt("Serial Port"), bITxt('COM5', key='_PORT_')], 
            [sTxt("Baud Rate"), bITxt('115200', key='_BAUD_')],
            [Btn('Open'), Btn('Close')]
])

presets = [['&File',['Load::_LOAD_PRESET_', 'Save::_SAVE_PRESET_','---' ,'Exit']]]
# presets = [['File', ['Open', 'Save', 'Exit',]],
#                 ['Edit', ['Paste', ['Special', 'Normal',], 'Undo'],],
#                 ['Help', 'About...'],]

quick_control = sg.Col([
    # [presets],
    [sg.Button("Next Layer", disabled=True, key = "_NXT_LAYER_", size = (10,2))],
    [sg.Button("Finish Work", disabled=True, key = "_END_WORK_", size = (10,1))],
    [sg.Button('STOP', button_color=('#FFFFFF','#FF0000'), size=(10,2))]
])

out = sg.Column([
    [sg.Output(size=(40, 20))],
    [sg.Checkbox("Enable Control", key="ENABLE_CONTROL", enable_events=True)],
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
    [Btn('Center', key = "_CENTER_"), Btn('Calibrate X', key = "_CAL_X_")],
    [Btn('Home X', key = "_HOME_X_"), Btn('Test X', key = "_TEST_X_")],
])

x_settings = sg.Frame("X axis settings",[
    [sTxt("Full Speed [Hz]"), bITxt("1000", key = "_SPEED_")]
])


xaxis = sg.Column([
    [x_queries],
    [x_settings]
    ])

y_queries = sg.Frame("Z axis control", [
    [Btn('Clean', key = "_CLEAN_"), Btn('Calibrate Z', key = "_CAL_Y_")],
    [Btn('Home Z', key = "_HOME_Y_"), Btn('Test Z', key = "_TEST_Y_")],
])

y_settings = sg.Frame("Z axis settings",[
    # [Txt("dy [um]"), ITxt("5")],
    [sTxt("1st layer [um]"), bITxt("5", key = "_FIRST_LAYER_")],
    [sTxt("layers [um]"), bITxt("5", key = "_LAYERS_")]
])

yaxis = sg.Column([
    [y_queries],
    [y_settings],
    [sTxt("  "), sg.Submit(size = (10,2))]
])

axes = sg.Column([
    [general],
    [xaxis],
    [yaxis]
])

tests = sg.Frame("Platform Tests",[
    [sg.Button("X Constant Run", key = "!CONSTANT_JOB_X!")],
    [sg.Button("Z Constant Run", key = "!CONSTANT_JOB_Z!")]
])

manual = sg.Frame("Manual Control",[
    [sg.Text("X Control"), sg.Radio("L", "X", default=True, disabled=True, key="!RXL!"), sg.Radio("R", "X", disabled=True, key="!RXR!"),
    sg.Text("Steps"), bITxt("50", key = "!STEPSX!", disabled=True), sg.Button("Send", key = "!SENDX!", disabled = True)],
    [sg.Text("Z Control"), sg.Radio("D", "Z", default=True, disabled=True, key="!RZD!"), sg.Radio("U", "Z", disabled=True, key="!RZU!"),
    sg.Text("Steps"), bITxt("100", key = "!STEPSZ!", disabled = True), sg.Button("Send", key = "!SENDZ!", disabled = True)],
])

calibration = sg.Frame("Calibration Constants",[
    [sg.Text("Z calibration constant [um/step]",size = (29,1)), bITxt("0.833", key = "!_Z_CONSTANT_!", disabled=True)],
    [sg.Text("Ramp to full distance ratio [%]", size = (29,1)), bITxt("33", key = "!_X_RATIO_!", disabled=True)]
])

setup = sg.Col([
    [sg.Checkbox("Platform Setup", key="SETUP", enable_events=True)],
    [calibration],
    [manual],
    [tests]
])

setup_keys = ["!CONSTANT_JOB_X!",  "!RXR!", "!RXL!", "!STEPSX!", "!SENDX!",
                "!CONSTANT_JOB_Z!", "!RZD!", "!RZU!", "!STEPSZ!", "!SENDZ!",
                "!_X_RATIO_!", "!_Z_CONSTANT_!"
]

layout = [
    [sg.Menu(presets), out, axes, setup]
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
    CENTER = 0xAA66
    
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
        
        if event not in ("Submit", "__TIMEOUT__", 'Load::_LOAD_PRESET_', 'Save::_SAVE_PRESET_'):
            print("<< ", event, '\n')

        if event is 'Load::_LOAD_PRESET_':
            load_preset = sg.popup_get_file("Load preset:", file_types=(("JSON", "*.json"),("JSON", "*.JSON")))
            filename, file_extension = os.path.splitext(load_preset)

            if load_preset is not None:
                if file_extension not in (".JSON", ".json"):
                    load_preset = filename + ".json"

                if not os.path.isabs(load_preset):
                    working_directory = os.getcwd()
                    load_preset = working_directory + "\\" + load_preset
                
                if os.path.isabs(load_preset):
                    with open(load_preset, 'r') as preset:
                        values = json.load(preset)
                        for (key,value) in values.items():
                            if key[0] == "_":
                                window[key].update(value)
                        # print(preset.read())
                        #TODO Update window 
            

        if event is 'Save::_SAVE_PRESET_':
            save_preset = sg.popup_get_file("Save preset as:",save_as = True,file_types=(("JSON", "*.json"),("JSON", "*.JSON")))
            filename, file_extension = os.path.splitext(save_preset)
            if save_preset is not None:
                if file_extension not in (".JSON", ".json"):
                    save_preset = filename + ".json"

                if not os.path.isabs(save_preset):
                    working_directory = os.getcwd()
                    save_preset = working_directory + "\\" + save_preset

                if os.path.isabs(save_preset):
                    with open(save_preset, 'w') as preset:
                        json.dump(values, preset, indent=4)
                        print("Saved as: " + save_preset)               

        if event in (None, 'Exit'):
            if ser.is_open:
                stop_read = True
                ser.close()
                if not ser.is_open:
                    print(">> Port Closed")
            break
        
        elif event is "ENABLE_CONTROL":
            if values["ENABLE_CONTROL"] == True:
                window["_NXT_LAYER_"].update(disabled=False)
                window["_END_WORK_"].update(disabled=False)
            else:
                window["_NXT_LAYER_"].update(disabled=True)
                window["_END_WORK_"].update(disabled=True)

        elif event is 'SETUP':
            if values["SETUP"] == True:
                for key in setup_keys:
                    window[key].update(disabled=False)
            else:
                for key in setup_keys:
                    window[key].update(disabled=True)

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
            print("Default layer depth [um] = ", values["_LAYERS_"])

            write_buffer[:2] = SUBMIT.to_bytes(2, byteorder = 'big')
            freq = int(values["_SPEED_"])
            # period = 1000000/freq
            # if period > 65536:
            #     period = 2000
            # if period < 100:
            #     period = 100
            # write_buffer[2:4] = int(period - 1).to_bytes(2, byteorder = 'big')
            if freq > 4000:
                freq = 4000
            if freq < 500:
                freq = 500
            write_buffer[2:4] = int(freq).to_bytes(2, byteorder = 'big')

            # print("Speed[Hz] = ", 1000000/period, "\n")
            # calibration_value = 100 # assume 100 steps per um
            calibration_value = float(values["!_Z_CONSTANT_!"]) # assume 100 steps per um
            first_layer = int(int(values['_FIRST_LAYER_']) / calibration_value)
            other_layers = int(int(values['_LAYERS_']) / calibration_value)
            print("1st layer depth [steps] = ", first_layer)
            print("Default layer depth [steps] = ", other_layers)
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
            option = 0
            write_buffer[2:4] = option.to_bytes(2, byteorder = 'big')
            
        elif event is "_TEST_Y_":
            write_buffer[:2] = TEST_Y.to_bytes(2, byteorder = 'big')
        
        elif event is "!CONSTANT_JOB_X!":
            write_buffer[:2] = TEST_X.to_bytes(2, byteorder = 'big')
            option = TEST_X
            write_buffer[2:4] = option.to_bytes(2, byteorder = 'big')
        # Other
        elif event is "_CLEAN_":
            write_buffer[:2] = CLEAN.to_bytes(2, byteorder = 'big')

        elif event is "_CENTER_":
            write_buffer[:2] = CENTER.to_bytes(2, byteorder = 'big')
        
        # Control
        elif event is "_NXT_LAYER_":
            write_buffer[:2] = NEXT_LAYER.to_bytes(2, byteorder = 'big')
        elif event is "_END_WORK_":
            write_buffer[:2] = FINISH_WORK .to_bytes(2, byteorder = 'big')
        
        if write_buffer[:] != bytearray(8) and ser.is_open:
            try:
                # print(write_buffer)
                ser.write(write_buffer)
            except serial.SerialTimeoutException as handler:
                print(handler)
                break
            # time.sleep(10)
            write_buffer[:] = bytearray(8)
            
            

