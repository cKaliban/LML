import PySimpleGUI as sg


def Btn(*args, **kwargs):
    return(sg.Button(*args, size = (9,1), **kwargs))

def sTxt(*args, **kwargs):
    return(sg.Text(*args, size = (11,1), justification='right', **kwargs))

def bTxt(*args, **kwargs):
    return(sg.Text(*args, size = (18,1), justification='right', **kwargs))

def sITxt(*args, **kwargs):
    return(sg.InputText(*args, size = (5,1), justification='right', **kwargs))

def bITxt(*args, **kwargs):
    return(sg.InputText(*args, size = (7,1), justification='right', **kwargs))

