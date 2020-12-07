import PySimpleGUI as sg
import serial
# import matplotlib
# import matplotlib.pyplot as plt
# import matplotlib.animation as animation
import json
import os.path
import time
import struct
import threading
from GUIwrapper import * 


def read_serial(serial, stop):
    print(">> Connection active\n")
    while True:
        if stop():
            break
        if serial.inWaiting():
            rx_byte = serial.read(8)
            print(">> ", rx_byte)
    print(">> Disconnected from the driver\n")

# [],
# [Btn("Close")]

class GUI():
    def __init__(self, style = "Dark2"):
        sg.ChangeLookAndFeel(style)

        presets = [["&File", ["Load","Save"]]]

        output_column = sg.Frame("Status", [
            [sg.Output(size=(40, 20))]
        ])

        port_data = [[Txt("Serial Port"), ITxt('COM7', key='_PORT_')], 
            [Txt("Baud Rate"), ITxt(115200, key='_BAUD_')],
            [Btn('Open'), Btn('Close')]
        ]

        pid_settings = [
            [Txt('Kp'), ITxt(1, key = '_KP_')],
            [Txt('Ki'), ITxt(0, key = '_KI_')],
            [Txt('Kd'), ITxt(0, key = '_KD_')]
        ]

        temp_settings = [
            [Txt('Process T [\xb0C]'), ITxt(100, key="_TP_")],
            [Txt('Tolerance [\u00B1\xb0C]'), ITxt(1, key="_T_TOL_")]
        ]

        power_settings = [
            [Txt('frequency [Hz]'), ITxt(1000, key='_FREQ_')],
            [Txt('max duty [%]'), ITxt(10, key = '_MAX_DUTY_')],
            [Txt('initial duty [%]'), ITxt(5, key = '_INIT_DUTY_')]
        ]

        settings = sg.Frame("Settings", [
            [sg.Frame("Serial settings", port_data, size=(10,5)), sg.Frame("PID settings", pid_settings, size=(10,5))], 
            [sg.Frame("Temperature settings", temp_settings, size=(10,5)), sg.Frame("Power settings", power_settings)]
        ])

        control = sg.Frame("control", [
            [sg.B('START', button_color=('#FFFFFF','#008000'), size=(6,1))],
            [sg.B('RESET', button_color=('#FFFFFF','#FFB700'), size=(6,1))],
            [sg.B('STOP', button_color=('#FFFFFF','#FF0000'), size=(6,1))]
        ])

        monitoring = sg.Frame("Monitoring", [
            [sg.Checkbox("Monitor Temperature", key="_MONITOR_T_")],
            [sg.Checkbox("Monitor PWM", key="_MONITOR_PWM_")]
        ])

        # general_options = sg.Col([[sg.SaveAs],
        # [sg.])

        laser_layout = [
            [sg.Menu(presets)],
            [output_column, sg.Column([
                [settings, sg.Submit()],
                [control, monitoring] # sg.Save(), sg.Exit()
             ])
            ]
        ]

        self.name = "Laser power control"
        self.layout = laser_layout
        # self.popup_read = sg.popup_get_file("Load preset:", file_types=(("JSON", "*.json"),("JSON", "*.JSON")))
        # self.popup_save = sg.popup_get_file("Save preset as:",save_as = True,file_types=(("JSON", "*.json"),("JSON", "*.JSON")))
        self.window = sg.Window(self.name, self.layout)




if __name__ == "__main__":
    PID_START = 0xD0D0
    PID_RESET = 0xCEDE
    PID_STOP = 0xDEAD
    PID_SET_TP = 0xAD5E
    PID_SET_KP = 0xADC9
    PID_SET_KD = 0xADCD
    PID_SET_KI = 0xADC1
    PID_SET_PERIOD = 0xADFF
    PID_SET_OI = 0xAD01
    PID_SET_OM = 0xAD03
    window = GUI()
    ser = serial.Serial()
    write_buffer = bytearray(8)
    while True:
        event, values = window.window.read(50)
        
        if event in (None, 'Exit'):
            if ser.is_open:
                ser.close()
                if not ser.is_open:
                    print("Port Closed")
            break
        if event is 'Open':
            ser.port = values["_PORT_"]
            ser.baudrate = values["_BAUD_"]
            ser.open()
            # if ser.is_open:
            # print("Port Opened: " + values["_PORT_"])
            read_task = threading.Thread(target=read_serial, args = (ser, lambda: stop_read, ))
            stop_read = False
            if ser.is_open:
                print(">> Port Opened: " + values["_PORT_"])
                read_task.start()

        if event is 'Close':
            # ser.close()
            stop_read = True
            ser.close() 
            if not ser.is_open:
                print(">> Port Closed")

        if event is 'Load':
            # print(event)
            load_preset = sg.popup_get_file("Load preset:", file_types=(("JSON", "*.json"),("JSON", "*.JSON")))
            # load_preset = window.popup_read()
            if load_preset is not None:
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
                                window.window[key].update(value)
                    print("Loaded preset: " + load_preset)
                    
            
        if event is 'Save':
            save_preset = sg.popup_get_file("Save preset as:",save_as = True,file_types=(("JSON", "*.json"),("JSON", "*.JSON")))
            # save_preset = window.popup_save()
            if save_preset is not None:
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

        if event is 'Submit':
            if ser.is_open:
                # TP and limits 
                write_buffer[:2] = PID_SET_TP.to_bytes(2, byteorder = 'big')
                write_buffer[2:4] = (int(values["_TP_"]) * 10 + 1000).to_bytes(2, byteorder = 'big')
                write_buffer[4:6] = ((int(values["_TP_"]) - int(values["_T_TOL_"])) * 10 + 1000).to_bytes(2, byteorder = 'big')
                write_buffer[6:8] = ((int(values["_TP_"]) + int(values["_T_TOL_"])) * 10 + 1000).to_bytes(2, byteorder = 'big')
                ser.write(write_buffer)
                time.sleep(0.01)
                # PID values
                write_buffer[:2] = PID_SET_KP.to_bytes(2, byteorder = 'big')
                kp_value = struct.pack(">f", float(values["_KP_"]))
                write_buffer[2:6] = kp_value
                ser.write(write_buffer)
                time.sleep(0.01)

                write_buffer[:2] = PID_SET_KI.to_bytes(2, byteorder = 'big')
                ki_value = struct.pack(">f", float(values["_KI_"]))
                write_buffer[2:6] = ki_value
                ser.write(write_buffer)
                time.sleep(0.01)
                
                write_buffer[:2] = PID_SET_KD.to_bytes(2, byteorder = 'big')
                kd_value = struct.pack(">f", float(values["_KD_"]))
                write_buffer[2:6] = kd_value
                ser.write(write_buffer)
                time.sleep(0.01)

                # PWM settings
                write_buffer[:2] = PID_SET_PERIOD.to_bytes(2, byteorder = 'big')
                freq = int(values["_FREQ_"])
                period = 1000000/freq
                if period > 2000:
                    period = 2000
                if period < 100:
                    period = 100
                write_buffer[2:4] = int(period - 1).to_bytes(2, byteorder = 'big')
                # print(write_buffer)
                ser.write(write_buffer)
                time.sleep(0.01)

                write_buffer[:2] = PID_SET_OM.to_bytes(2, byteorder = 'big')
                max_out = float(values["_MAX_DUTY_"])/100
                if max_out > 0.2:
                    max_out = 0.2
                if max_out < 0.02:
                    max_out = 0.2
                # print(max_out)
                # print(int(max_out*period))
                write_buffer[2:4] = int(max_out*period).to_bytes(2, byteorder = 'big')
                ser.write(write_buffer)
                time.sleep(0.01)

                write_buffer[:2] = PID_SET_OI.to_bytes(2, byteorder = 'big')
                initial_out = float(values["_INIT_DUTY_"])/100
                if initial_out > max_out:
                    initial_out = max_out
                # print(initial_out)
                # print(int(initial_out*period))
                write_buffer[2:4] = int(initial_out*period).to_bytes(2, byteorder = 'big')
                ser.write(write_buffer)
                time.sleep(0.01)
            else:
                print("!Serial is closed")

        if event in ("START"):
            if ser.is_open:
                write_buffer[:2] = PID_START.to_bytes(2, byteorder = 'big')
                ser.write(write_buffer)
            else:
                print("!Serial is closed")

        if event in ("RESET"):
            if ser.is_open:
                write_buffer[:2] = PID_RESET.to_bytes(2, byteorder = 'big')
                ser.write(write_buffer)
            else:
                print("!Serial is closed")

        if event in ("STOP"):
            if ser.is_open:
                write_buffer[:2] = PID_STOP.to_bytes(2, byteorder = 'big')
                ser.write(write_buffer)
            else:
                print("!Serial is closed")

