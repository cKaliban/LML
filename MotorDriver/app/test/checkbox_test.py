import PySimpleGUI as sg

r_keys = ['-R1-', '-R2-', '-R3-']

layout = [  [sg.Text('Radio Test')],
            [sg.Checkbox('Radio 1', key=r_keys[0], default=True), 
             sg.Checkbox('Radio 2', key=r_keys[1], enable_events=True), 
             sg.Checkbox('Radio 3', key=r_keys[2])],
            [sg.T('The key of selected radio buitton is'), sg.Text('', size=(5,1), key='-OUT-')],
            [sg.Button('Do Something'), sg.Button('Exit')]  ]

window = sg.Window('Window Title', layout)

while True:             # Event Loop
    event, values = window.read()
    print(event, values)
    if event in (None, 'Exit'):
        break
    window['-OUT-']([ key for key in r_keys if values[key]][0])
window.close()