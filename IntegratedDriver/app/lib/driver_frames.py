
class Frame:
    driver_types = {
        b'INTDRIVE' : "Integrated driver",
        b'BPLDRIVE' : "Platform control driver",
        b'LPWDRIVE' : "Laser power driver"
    }

    PID_settings = {
        "PID: Temperature [\u2103]" : 0xAD5E,
        "PID: Tolerance [\u2103]" : 0x0000,
        "PID: Loop delay [ms]" : 0x0000,
        "PID: Kp" : 0xADC9,
        "PID: Kd" : 0xADCD,
        "PID: Ki" : 0xADC1,
    }

    PWM_settings = {
        "PWM: Frequency [Hz]" : 0xADFF,
        "PWM: Main power [%]" : 0xAD01,
        "PWM: Preheat power [%]" : 0xAD03
    }

    platform_settings = {
        "Platform max speed [Hz]" : 0xADAA,
        "First layer depth [um]" : 0xADB0,
        "Default layer depth [um]" : 0xADBB
    }

    all_settings = {
        "PID: Temperature [\u2103]" : 0xAD5E,
        "PID: Tolerance [\u2103]" : 0x0000, #TODO!
        "PID: Loop delay [ms]" : 0x0000, #TODO!
        "PID: Kp" : 0xADC9,
        "PID: Kd" : 0xADCD,
        "PID: Ki" : 0xADC1,

        "PWM: Frequency [Hz]" : 0xADFF,
        "PWM: Preheat power [%]" : 0xAD03,
        "PWM: Main power [%]" : 0xAD01,

        "Platform speed [Hz]" : 0xADAA,
        "First layer depth [um]" : 0xADB0,
        "Default layer depth [um]" : 0xADBB
    }

    general_commands = {
        "Identify" : 0x1D1D,
        "Read settings" : 0x5EED, # TODO CHANGE!
        "Submit settings" : 0xFFFF,
        "Test system" : 0xCEAB,
        "Home" : 0x00AB,
    }

    x_control_commands = {
        "Calibrate X" : 0xAA5E,
        "Test X axis" : 0xAACE,
        "Home X" : 0x00AA,
        "Right" : 0xAA00,
        "Center" : 0xAACC,
        "Left" :  0xAAFF,
    }

    z_control_commands = {
        "Calibrate Z" : 0xBB5E,
        "Test Z axis" : 0xBBCE,
        "Home Z" : 0x00BB,
        "Clean" : 0xBB66,
    }

    PID_commands = {
        "Start PID" : 0xD0D0,
        "Reset PID" : 0xCEDE,
        "Stop PID" : 0x0000 # TODO: change this packet
    }  

    laser_commands = {
        "Start Laser" : 0x0000,
        "Stop Laser" : 0x0000
    }

    quick_commands = {
        "Calibrate" : 0x5EAB,
        "Start processing" : 0x6060,
        # "Next layer" : 0x6060,
        "Finish processing" : 0xF1FF,
        "STOP" : 0xDEAD
    }

    system_modes = {
        "Debug mode" : 0x0000, #TODO!
        "Laser mode" : 0x0000, #TODO!
        "System coupling" : 0x0000, #TODO!
    }


    write_headers = [quick_commands, laser_commands, PID_commands,
        z_control_commands, x_control_commands, general_commands,
        platform_settings, PWM_settings, PID_settings]

    read_headers = []




    # "Calibrate X" : 0xAA5E
    # TEST = 0xCECC







# IDN = 0x1D1D
# SUBMIT = 0x5EED
# CALIBRATE = 0x5EAB
# HOME = 0xAB00
# TEST = 0xCECC
# STOP = 0xDEAD

# X control
# CALIBRATE_X = 0xAA5E
# HOME_X = 0xAA00
# TEST_X = 0xAACE
# CENTER = 0xAA66

# Y control
# CALIBRATE_Y = 0xBB5E
# HOME_Y = 0xBB00
TEST_Y = 0xBBCE
CLEAN = 0xBB66

# 
# SUBMIT = 0xF00D
NEXT_LAYER = 0x6060
FINISH_WORK = 0xF1FF


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