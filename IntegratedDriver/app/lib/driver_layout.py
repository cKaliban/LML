import PySimpleGUI as sg 
from .driver_frames import Frame
from .GUIwrapper import *

sg.ChangeLookAndFeel("Dark2")

class Layout:
    # Layout constants
    _system_options = [
        ["Debug mode", "Laser control", "Laser mode", "PID delay", "Platform speed", "Subsystems"],
        ["Off", "Auto", "PID", "On", "Ramp", "Coupled"],
        ["On", "Manual", "Constant", "Off", "Constant", "Independent"]
    ]

############## NEW BEAUTIFUL LAYOUTS
    _menu = [['&File',['Load::_LOAD_PRESET_', 'Save::_SAVE_PRESET_','---' ,'Exit']]]

    # Settings
    _PID_settings = sg.Frame("", [
        [sg.T(key, key=("_CONST_" + key), size = (18,1)), 
        sg.In("0", justification='right', key=key, size = (5,1))] for key in Frame.PID_settings.keys()
    ])

    _PWM_settings = sg.Frame("", [
        [sg.T(key, key=("_CONST_" + key), size = (18,1)),
        sg.In("0", justification='right', key=key, size = (5,1))] for key in Frame.PWM_settings.keys()
    ])

    _platform_settings = sg.Frame("",[
        [sg.T(key, key=("_CONST_" + key), size = (18,1)),
        sg.In("0", justification='right', key=key, size = (5,1))] for key in Frame.platform_settings.keys()
    ])

    settings_layout = [
        [sg.T(key, key=("_CONST_" + key), size = (18,1)),
        sg.In("0", justification='right', key=key, size = (5,1))] for key in Frame.all_settings.keys()
    ]

    settings_layout.insert(9, [sg.T("_"*28)])
    settings_layout.insert(6, [sg.T("_"*28)])
    settings_layout.insert(0, [sg.T("_"*28)])

    _all_settings = sg.Frame("Settings", settings_layout)

    _serial_settings = sg.Frame("Serial", [
        [sg.T("Auto connect", size=(12,1)), sg.B("Connect", size=(10,2), key='_AUTO_CON_')],
        [sg.T("Search ports", size=(12,1)), sg.B("Scan", size=(10,2), key='_SCAN_')],
        [sg.T("Serial Port", size=(12,1)), sg.In('COM5', size=(12,1), justification='right', key='_PORT_')], 
        [sg.T("Baud Rate", size=(12,1)), sg.In('115200', size=(12,1), justification='right', key='_BAUD_')],
        [sg.B("Open", key='_OPEN_', size=(11,1)), sg.B("Close", key='_CLOSE_', size=(11,1))]
    ])

    # Checkboxes and radio
    _laser_monitoring = sg.Frame("Laser Monitoring", [
        [sg.Checkbox("Monitor Temperature", size=(20,1), key="_MONITOR_T_")],
        [sg.Checkbox("Monitor PWM", size=(20,1), key="_MONITOR_PWM_")],
        [sg.Checkbox("Save to file", size=(20,1), key="_SAVE_STATS_")]
    ])

    _system_debug = sg.Checkbox("Activate advanced mode", size=(20,1), key="_DEBUG_", enable_events=True)

    _system_modes_description = sg.Col([
        [sg.T(desc, size=(12,1), pad=(1,4))] for desc in _system_options[0]
    ])

    _system_modes_default = sg.Col([
        [sg.R(desc, ind, default=True, key=str(ind+1), disabled=True)] for ind, desc in enumerate(_system_options[1])
    ])

    _system_modes_alternative = sg.Col([
        [sg.R(desc, ind, key=str((ind+1)*11), disabled=True)] for ind, desc in enumerate(_system_options[2])
    ])

    _system_modes = sg.Frame("",[
        [_system_modes_description, _system_modes_default, _system_modes_alternative]
    ])

    # Control
    _layers_num = [sg.T("Layers", size = (6,1)), sg.I('1', key= "_LAYERS_",size=(8,1), justification='right')]
    _preheat_num = [sg.T("Preheat", size = (6,1)), sg.I('0', key= "_PREHEAT_",size=(8,1), justification='right')]
    _qc = [[sg.B(key, size=(14,2))] for key in Frame.quick_commands.keys()]
    _qc.insert(0,_layers_num)
    _qc.insert(1,_preheat_num)
    # print(_qc)
    _quick_control = sg.Frame("Quick control",
        _qc
    )

    _PID_control = sg.Frame("PID control",[
        [sg.B(key, size=(10,1))] for key in Frame.PID_commands.keys()
    ])

    _laser_control = sg.Frame("Laser control",[
        [sg.B(key, size=(10,1))] for key in Frame.laser_commands.keys()
    ])

    _general_control = sg.Frame("Main control",[
        [sg.B(key, size=(14,2))] for key in Frame.general_commands.keys()
    ])

    _x_axis_control = sg.Frame("X axis control", [
        [sg.B(key, size=(10,1))] for key in Frame.x_control_commands.keys()
    ])

    _z_axis_control = sg.Frame("Z axis control", [
        [sg.B(key, size=(10,1))] for key in Frame.z_control_commands.keys()
    ])

############## OLD UGLY LAYOUTS
    # [sg.Checkbox("Enable Control", key="ENABLE_CONTROL", enable_events=True)],


    control = sg.Frame("control", [
        [sg.B('START', button_color=('#FFFFFF','#008000'), size=(6,1))],
        [sg.B('RESET', button_color=('#FFFFFF','#FFB700'), size=(6,1))],
        [sg.B('STOP', button_color=('#FFFFFF','#FF0000'), size=(6,1))]
    ])


    

    
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
        [sg.Output(size=(38, 15))]
        
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

    


    xaxis = sg.Column([
        [x_queries]
        # [x_settings]
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

    # _laser_layout = sg.Column([
    #     [settings, sg.Submit()],
    #     [control, monitoring] # sg.Save(), sg.Exit()
    # ])


    @staticmethod
    def motor_layout():
        layout = [[]]
        return layout

    @staticmethod
    def laser_layout():
        layout = [[]]
        return layout
    
    @staticmethod
    def integrated_layout():
        control_column = sg.Col([
            [Layout._laser_control],
            [Layout._PID_control],
            [Layout._x_axis_control],
            [Layout._z_axis_control]
        ])

        # settings_column = sg.Col([
        #     [Layout._PID_settings],
        #     [Layout._PWM_settings],
        #     [Layout._platform_settings],
        #     [Layout._laser_monitoring]
        # ])

        settings_column = sg.Col([
            [Layout._serial_settings],
            [Layout._all_settings]
        ])

        options_column = sg.Col([
            [Layout._laser_monitoring],
            [Layout._system_debug],
            [Layout._system_modes],
            [Layout.manual]
        ])

        general_column = sg.Col([
            [Layout.out],            
            [Layout._general_control, Layout._quick_control]
        ])

        layout = [
            [sg.Menu(Layout._menu)],
            [settings_column, options_column, control_column, general_column]
            # [settings_column, control_column, general_column]
        ]

        return layout
