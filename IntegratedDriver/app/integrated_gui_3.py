import PySimpleGUI as sg 
# import serial
import time
import threading
from multiprocessing import Process, Pipe, freeze_support
import json
import os.path
# import lib.driver_frames as frames
import lib.serial_overload as serial
from lib.driver_layout import Layout
from lib.driver_frames import Frame
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import random
import struct
import datetime
global l_num
reader_pipe, plotter_pipe = Pipe()
Z_constant = 0.833
# global l_num
l_num = 0

def read_serial(serial, pipe, stop):
    global l_num
    print(">> Connection active\n")
    # time.sleep(1)
    serial.flushInput()
    series = 1
    output_time = time.time()
    
    try:
        filename = str(series) + ".csv"
        f = open(filename, "w")
    except:
        print("Could not create log file.")
    

    while True:
        if stop():
            f.close()
            break
        if serial.in_waiting > 0:
            rx_byte = serial.read(size = 8)
            # print(rx_byte)
            header = int.from_bytes(rx_byte[0:2], byteorder='little')
            # header = int.from_bytes(rx_byte[0:2], byteorder='big')
            if header == 0xF00D:
                # value = int.from_bytes(rx_byte[4:], byteorder='little')
                # print(value)
                value = rx_byte[2:]
                y1 = (int.from_bytes(value[2:], byteorder='little') - 1000) / 10
                y2 = int.from_bytes(value[:2], byteorder='little') / 2
                print("MEAS:TEMP = " + str(y1))
                print("MEAS:PWMD = " + str(y2))
                line_time = str(time.time() - output_time)
                temp = str(y1)
                pwm = str(y2)
                line = line_time + ", " + temp + ", " + pwm + "\n"
                f.write(line)
                pipe.send(value)    
            else:
                timestamp = time.strftime('[%H:%M:%S]')
                if(str(rx_byte) == "b'LAYREADY'"):
                    l_num +=1
                    message = timestamp + " " + str(rx_byte) + " " + str(l_num) + "/" + str(50)
                elif(str(rx_byte) == "b'INTDRIVE'"):
                    output_time = time.time()
                    series += 1
                    f.close()
                    try:
                        filename = str(series) + ".csv"
                        f = open(filename, "w")
                        print("Log file updated")
                    except:
                        print("Could not create log file.")
                    message = timestamp + " " + str(rx_byte)
                else:
                    message = timestamp + " " + str(rx_byte)
                print(message)
    # pipe.send("END")
    print(">> Disconnected from the driver\n")


def plotter(pipe):
    while 1:
        limit = 1000
        fig, (ax1, ax2) = plt.subplots(2,1)
        line1, = ax1.plot([], [], lw=2)
        line2, = ax2.plot([], [], lw=2, color='r', )
        lines = [line1, line2]
        ax1.set_ylim(0, 250)
        ax2.set_ylim(0, 20) 
        ax1.set_xlim(0, 60)
        ax2.set_xlim(0, 60)

        ax1.grid()
        ax2.grid()

        xdata, y1data, y2data = [],[],[]
        start_time = time.time()
        def run(i, xdata, y1data, y2data):
            data = pipe.recv()
            t = time.time() - start_time
            y2 = int.from_bytes(data[:2], byteorder='little')
            y1 = int.from_bytes(data[2:], byteorder='little')
            y1 = (y1 - 1000) / 10
            y2 = y2 / 2
            # print(type(y1))
            # print(y1)
            # t, y1, y2 = data
            # y2 = y1
            # y1 = int(y1) 
            # y2 = int(y2) % 20
            xdata.append(t)
            y1data.append(y1)
            y2data.append(y2)
            # xdata = xdata[-limit:]
            # y1data = y1data[-limit:]
            # y2data = y2data[-limit:]
            for ax in [ax1, ax2]:
                xmin, xmax = ax.get_xlim()
                if t >= xmax:
                    ax.set_xlim(xmin, 2*t)
                    ax.figure.canvas.draw()
                elif xmax < t:
                    ax.set_xlim(0, limit)
                    ax.figure.canvas.draw()
            lines[0].set_data(xdata, y1data)
            lines[1].set_data(xdata, y2data)
            return lines
        ani = animation.FuncAnimation(fig, run, fargs=(xdata, y1data,y2data),blit = True, interval=10)
        plt.show()
        plt.pause(0.01)

        # try:
        #     packet = pipe.recv()
        #     if packet == "END":
        #         print("Ending")
        #         break
        #     # print(packet)
        # except:
        #     break



# TODO: Move to frames or other layout packet
setup_keys = ["!CONSTANT_JOB_X!",  "!RXR!", "!RXL!", "!STEPSX!", "!SENDX!",
                "!CONSTANT_JOB_Z!", "!RZD!", "!RZU!", "!STEPSZ!", "!SENDZ!",
                "!_X_RATIO_!", "!_Z_CONSTANT_!"
]


layout = Layout.integrated_layout()

if __name__ == "__main__":
    freeze_support()
    window = sg.Window("Motor Control", layout)
    ser = serial.SerialDriver(write_timeout = 0)
    write_buffer = bytearray(8)
    running = Process(target=plotter, args=(plotter_pipe,))
    while True:
        event, values = window.read()
        if event in ('Exit', None):
            if running.is_alive():
                running.terminate()
            if ser.is_open:
                stop_read = True
                ser.close()
            break
        # Serial control. TODO: Find a way to streamline. Maybe dictionary with those values?
        if event in ("_AUTO_CON_", "_SCAN_", "_OPEN_", "_CLOSE_"): 
            # TODO: change to dictionary call using getattr or class to dict 
            # For now, this must suffice
            if event == "_AUTO_CON_":
                if not ser.is_open:
                    ser.autoconnect()
                    window["_PORT_"].update(ser.port)
                    window["_BAUD_"].update(ser.baudrate)
                    if ser.is_open:
                        stop_read = False
                        read_task = threading.Thread(target=read_serial, args = (ser, reader_pipe, lambda: stop_read, ))
                        read_task.start()
                        running = Process(target=plotter, args=(plotter_pipe,))
                        running.start()
            elif event == "_SCAN_":
                print(ser.scan())
            elif event == "_OPEN_":
                if not ser.is_open:
                    print(values["_PORT_"])
                    print(values["_BAUD_"])
                    ser.port = values["_PORT_"]
                    ser.baud = values["_BAUD_"]
                    ser.open()
                    if ser.is_open:
                        read_task = threading.Thread(target=read_serial, args = (ser, reader_pipe, lambda: stop_read))
                        stop_read = False
                        read_task.start()
                        running = Process(target=plotter, args=(plotter_pipe,))
                        running.start()
            elif event == "_CLOSE_":
                stop_read = True
                if running.is_alive():
                    running.terminate()
                if ser.is_open:
                    ser.close()
        
        # Preset control
        if event in ('Load::_LOAD_PRESET_', 'Save::_SAVE_PRESET_'):
            if event == 'Load::_LOAD_PRESET_':
                load_preset = sg.popup_get_file("Load preset:", file_types=(("JSON", "*.json"),("JSON", "*.JSON")))
                if load_preset is not None:
                    filename, file_extension = os.path.splitext(load_preset)
                    if file_extension not in (".JSON", ".json"):
                        load_preset = filename + ".json"
                    if not os.path.isabs(load_preset):
                        working_directory = os.getcwd()
                        load_preset = working_directory + "\\" + load_preset
                    if os.path.isabs(load_preset):
                        with open(load_preset, 'r') as preset:
                            values = json.load(preset)
                            for (key,value) in values.items():
                                # if key[0] == "_":
                                window[key].update(value)
                            print("Settings loaded from:\n" + load_preset)
            if event == 'Save::_SAVE_PRESET_':
                save_preset = sg.popup_get_file("Save preset as:",save_as = True,file_types=(("JSON", "*.json"),("JSON", "*.JSON")))
                if save_preset is not None:
                    filename, file_extension = os.path.splitext(save_preset)
                    if file_extension not in (".JSON", ".json"):
                        save_preset = filename + ".json"
                    if not os.path.isabs(save_preset):
                        working_directory = os.getcwd()
                        save_preset = working_directory + "\\" + save_preset
                    if os.path.isabs(save_preset):
                        with open(save_preset, 'w') as preset:
                            values.pop(0)
                            json.dump(values, preset, indent=4)
                            print("Saved as:\n" + save_preset)

        # Debug modes
        if event in ('_DEBUG_'):
            if values['_DEBUG_'] == True:
                print("Debug Mode")
                for i in range(1,7):
                    window[str(i)].update(disabled=False)
                    window[str(i*11)].update(disabled=False)
            else:
                print("Normal Mode")
                for i in range(1,7):
                    window[str(i)].update(disabled=True)
                    window[str(i*11)].update(disabled=True)
            pass        
        
        # Actions performed on driver
        if ser.is_open:
            if event in Frame.general_commands.keys():
                if event == "Submit settings":
                    # Temperature settings
                    header = Frame.all_settings["PID: Temperature [\u2103]"]
                    temperature = int(values["PID: Temperature [\u2103]"])
                    tolerance = int(values["PID: Tolerance [\u2103]"])
                    temperature_hex = (temperature*10 + 1000)
                    tolerance_p = ((temperature+tolerance)*10 + 1000)
                    tolerance_n = ((temperature-tolerance)*10 + 1000)
                    write_buffer[0:2] = header.to_bytes(2, byteorder='little')
                    write_buffer[2:4] = temperature_hex.to_bytes(2, byteorder='little')
                    write_buffer[4:6] = tolerance_p.to_bytes(2, byteorder='little')
                    write_buffer[6:8] = tolerance_n.to_bytes(2, byteorder='little')
                    ser.write(write_buffer)
                    print("Temperature settings changed.")
                    time.sleep(0.01)

                    # PID settings
                    header = Frame.all_settings["PID: Kp"].to_bytes(2, 'little')
                    kp_value = struct.pack("<f", float(values["PID: Kp"]))
                    write_buffer[0:2] = header
                    write_buffer[4:8] = kp_value
                    ser.write(write_buffer)
                    time.sleep(0.01)

                    header = Frame.all_settings["PID: Kd"].to_bytes(2, 'little')
                    kd_value = struct.pack("<f", float(values["PID: Kd"]))
                    write_buffer[0:2] = header
                    write_buffer[4:8] = kd_value
                    ser.write(write_buffer)
                    time.sleep(0.01)

                    header = Frame.all_settings["PID: Ki"].to_bytes(2, 'little')
                    ki_value = struct.pack("<f", float(values["PID: Ki"]))
                    write_buffer[0:2] = header
                    write_buffer[4:8] = ki_value
                    ser.write(write_buffer)
                    print("PID settings changed.")
                    time.sleep(0.01)

                    # PWM settings
                    header = Frame.all_settings["PWM: Frequency [Hz]"].to_bytes(2, 'little')
                    frequency = int(values["PWM: Frequency [Hz]"])
                    if frequency > 10000:
                        frequency = 10000
                    elif frequency < 1000:
                        frequency = 1000
                    period = int(1000000/frequency - 1)

                    max_power = float(values["PWM: Preheat power [%]"]) / 100
                    if max_power > 0.95: # max 20% output power
                        max_power = 0.95
                    elif max_power < 0.01: # min 1% output power
                        max_power = 0.01
                    max_power_hex = int(max_power * period) + 1

                    initial_power = float(values["PWM: Main power [%]"]) / 100
                    
                    # if initial_power > max_power: # max 20% output power
                        # initial_power = max_power
                        # values["PWM: Initial power [%]"] = max_power * 100
                    if initial_power < 0.01: # min 1% output power
                        initial_power = 0.01
                    initial_power = int(initial_power * period) + 1

                    write_buffer[0:2] = header
                    write_buffer[2:4] = period.to_bytes(2, 'little')
                    write_buffer[4:6] = max_power_hex.to_bytes(2, 'little')
                    write_buffer[6:8] = initial_power.to_bytes(2, 'little')
                    ser.write(write_buffer)
                    print("PWM settings changed.")
                    time.sleep(0.01)

                    # Platform settings
                    header = Frame.all_settings["Platform speed [Hz]"]
                    speed = int(values["Platform speed [Hz]"])
                    depth_initial = int(int(values["First layer depth [um]"]) / Z_constant)
                    depth_default = int(int(values["Default layer depth [um]"]) / Z_constant)
                    write_buffer[0:2] = header.to_bytes(2, 'little')
                    write_buffer[2:4] = speed.to_bytes(2, 'little')
                    write_buffer[4:6] = depth_initial.to_bytes(2, 'little')
                    write_buffer[6:8] = depth_default.to_bytes(2, 'little')
                    ser.write(write_buffer)
                    print("Platform settings changed.")
                    time.sleep(0.01)



                    
                else:
                    header = Frame.general_commands[event].to_bytes(2, byteorder='little')
                    write_buffer[0:2] = header
                    ser.write(write_buffer)
                    write_buffer = bytearray(8)

            if event in Frame.quick_commands.keys():
                if event == "Start processing":
                    l_num = 0 
                    header = Frame.quick_commands[event].to_bytes(2, byteorder='little')
                    write_buffer[0:2] = header
                    layer_num = int(values["_LAYERS_"])
                    if layer_num < 1:
                        layer_num = 1
                    write_buffer[2:4] = layer_num.to_bytes(2, byteorder='little')
                    ser.write(write_buffer)
                    write_buffer = bytearray(8)
                else:
                    header = Frame.quick_commands[event].to_bytes(2, byteorder='little')
                    write_buffer[0:2] = header
                    ser.write(write_buffer)
                    write_buffer = bytearray(8)

            if event in Frame.z_control_commands.keys():
                header = Frame.z_control_commands[event].to_bytes(2, byteorder='little')
                write_buffer[0:2] = header
                ser.write(write_buffer)
                write_buffer = bytearray(8)
            if event in Frame.x_control_commands.keys():
                header = Frame.x_control_commands[event].to_bytes(2, byteorder='little')
                write_buffer[0:2] = header
                ser.write(write_buffer)
                write_buffer = bytearray(8)

