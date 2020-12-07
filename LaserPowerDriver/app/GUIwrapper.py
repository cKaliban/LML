import PySimpleGUI as sg


def Btn(*args, **kwargs):
    return(sg.Button(*args, size = (7,1), **kwargs))

def Txt(*args, **kwargs):
    return(sg.Text(*args, size = (11,1), justification='right', **kwargs))

def ITxt(*args, **kwargs):
    return(sg.InputText(*args, size = (7,1), justification='right', **kwargs))

def Col(*args, **kwargs):
    return(sg.Column(*args, background_color="#000000", **kwargs))


