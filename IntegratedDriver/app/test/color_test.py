import PySimpleGUI as sg
import tkinter as tk
from tkinter.font import Font


class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

class out_colors:
    WARNING = 'yellow'
    ALARM = 'red'
    DEFAULT = 'black'
    SETTINGS = 'blue'
    CONFIRMATION = 'green'


def print_to_out(window, string, key = "-OUTPUT-", color = "black"):
    window[key].update(value = (string + "\n"), append = True, text_color_for_value = color)


sg.theme('BluePurple')

layout = [[sg.Text('Your typed chars appear here:'), sg.Multiline(size=(15,1), key='-OUTPUT-', autoscroll = True, disabled = True)],
          [sg.Input(key='-IN-')],
          [sg.Button('Show'), sg.Button('Exit')]]

window = sg.Window('Pattern 2B', layout)

while True:  # Event Loop
    event, values = window.read()
    if event in  (None, 'Exit'):
        break
    if event == 'Show':
        # Update the "output" text element to be the value of "input" element
        # window['-OUTPUT-'].update(values['-IN-'])
        print_to_out(window, values['-IN-'], color = 'red')
        print_to_out(window, values['-IN-'], color = 'black')

window.close()
