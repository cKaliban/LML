import PySimpleGUI as sg

r_keys = ['-R1-', '-R2-', '-R3-']

layout = [  [sg.Text('Radio Test')],
            [sg.Rad('Radio 1', 1, key=r_keys[0], default=True), 
             sg.Rad('Radio 2', 1,key=r_keys[1]), 
             sg.Rad('Radio 3', 1, key=r_keys[2])],
            [sg.T('The key of selected radio buitton is'), sg.Text('', size=(5,1), key='-OUT-')],
            [sg.Button('Do Something'), sg.Button('Exit')]  ]

window = sg.Window('Window Title', layout)

while True:             # Event Loop
    event, values = window.read(50)
    print(event, values)
    if event in (None, 'Exit'):
        break
    window['-OUT-']([ key for key in r_keys if values[key]][0])
window.close()