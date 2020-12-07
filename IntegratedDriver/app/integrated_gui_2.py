import PySimpleGUI as sg 
import serial
import time
import threading
import json
import os.path
from lib.GUIwrapper import *
import lib.serial_connect

print(lib.serial_connect.Frame_type["WARNING"])
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

placeholder = [[sg.T("Placeholder")]]

# Menus: File Management, Internal Configuration, System Info
file_menu = [['&File',['Load::_LOAD_PRESET_', 'Save::_SAVE_PRESET_','---' ,'Exit']]]
config_menu = [[]]
info_menu = [[]]

# Buld Platform Settings 
x_settings = sg.Frame("X axis settings",[
    [sTxt("Full Speed [Hz]"), sITxt("1000", key = "_SPEED_")]
])

y_settings = sg.Frame("Z axis settings",[
    # [Txt("dy [um]"), ITxt("5")],
    [sTxt("1st layer [um]"), sITxt("5", key = "_FIRST_LAYER_")],
    [sTxt("default depth [um]"), sITxt("5", key = "_DEFAULT_LAYER_")]
])

platform_settings = sg.Frame("Build Platform Control settings", [
    [x_settings],
    [y_settings]
], border_width=4)


# Heating Laser Settings
monitoring = sg.Frame("Monitoring", [
    [sg.Checkbox("Monitor Temperature", key="_MONITOR_T_")],
    [sg.Checkbox("Monitor PWM", key="_MONITOR_PWM_")]
], size=(10,5))

pid_settings = sg.Frame("PID coefficients", [
    [sTxt('Kp'), sITxt(1, key = '_KP_')],
    [sTxt('Ki'), sITxt(0, key = '_KI_')],
    [sTxt('Kd'), sITxt(0, key = '_KD_')]
], size=(10,5))

temperature_settings = sg.Frame("Temperature settings",[
    [sTxt('Process T [\xb0C]'), sITxt(100, key="_TP_")],
    [sTxt('Tolerance [\u00B1\xb0C]'), sITxt(1, key="_T_TOL_")]
])

power_settings = sg.Frame("Power settings", [
    [sTxt('frequency [Hz]'), sITxt(1000, key='_FREQ_')],
    [sTxt('max duty [%]'), sITxt(10, key = '_MAX_DUTY_')],
    [sTxt('initial duty [%]'), sITxt(5, key = '_INIT_DUTY_')]
])

laser_settings = sg.Frame("Heating Laser Control settings", [
    [monitoring],
    [pid_settings],
    [power_settings],
    [temperature_settings]
], border_width=4)

# Main Control
auto_serial = sg.Frame("Auto connect", [
    [Btn("Scan"), Btn("Connect")]
])

manual_serial = sg.Frame("Manual connect", [
            [sTxt("Serial Port"), sITxt('COM5', key='_PORT_')], 
            [sTxt("Baud Rate"), sITxt('115200', key='_BAUD_')],
            [Btn('Open'), Btn('Close')]
])

serial_settings = sg.Frame("Driver connection",[
    [auto_serial],
    [manual_serial]
], border_width=4)


# GUI integration
setting_column = sg.Col([
    [laser_settings],
    [platform_settings],
    [sg.Button("Update Settings", key="_UPDATE_", size=(20,2), font='Default 14')]
])

# debug_column = sg.Col()
# control_column = sg.Frame("Main Control", [
control_column = sg.Col([
    [sg.Output(size=(31, 20))],
    [serial_settings],
    [sg.Button('AUTO', size=(10,2), font='Default 14'), sg.Button('STOP', button_color=('#FFFFFF','#FF0000'), size=(10,2), font='Default 14')]
])
# ], border_width=4)




###############################################################
layout = [
    [sg.Col(placeholder), setting_column, control_column]
]


if __name__ == "__main__":
    window = sg.Window("Motor Control", layout)
    # ser = serial.Serial(write_timeout = 0)

    while True:
        event, values = window.read()
        if event in (None, 'Exit'):
            break