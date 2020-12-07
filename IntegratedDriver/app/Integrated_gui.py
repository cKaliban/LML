import PySimpleGUI as sg 
# import serial
import time
import threading
import json
import os.path
# import lib.driver_frames as frames
import lib.serial_overload as serial
from lib.driver_layout import Layout



def read_serial(serial, stop):
    print(">> Connection active\n")
    while True:
        if stop():
            break
        if serial.in_waiting > 0:
            rx_byte = serial.read(size = 8)
            print(">> ", rx_byte)
    print(">> Disconnected from the driver\n")





setup_keys = ["!CONSTANT_JOB_X!",  "!RXR!", "!RXL!", "!STEPSX!", "!SENDX!",
                "!CONSTANT_JOB_Z!", "!RZD!", "!RZU!", "!STEPSZ!", "!SENDZ!",
                "!_X_RATIO_!", "!_Z_CONSTANT_!"
]


layout = Layout.integrated_layout()

if __name__ == "__main__":
    window = sg.Window("Motor Control", layout)
    ser = serial.SerialDriver(write_timeout = 0)
    

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
                print("Error occured")
        
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
            except:
                print("Error occured during writing, check device.")
                break
            # time.sleep(10)
            write_buffer[:] = bytearray(8)
        
        # print(values)
            
            

