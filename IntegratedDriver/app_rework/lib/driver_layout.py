import PySimpleGUI as sg
from .driver_frames import Frame

sg.ChangeLookAndFeel("Dark2")

class Layout:

    _menu = [['&File',['Load::_LOAD_PRESET_', 'Save::_SAVE_PRESET_','---' ,'Exit']]]

    _heating_laser_settings = None

