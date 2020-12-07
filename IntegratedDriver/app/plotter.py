import multiprocessing as mp
import lib.serial_overload as serial
import time
import matplotlib.pyplot as plt

import random

def f(pipe):
    key = ""
    while key != "END":
        key = pipe.recv()
        print(key)
    pipe.close()

def reader(serial, pipe):


    pipe.close()

if __name__ == '__main__':
    reader_pipe, plotter_pipe = mp.Pipe()

    # p = mp.Process(target=f, args=(plotter_pipe,))

    # p.start()
    key = ""
    while key != "END":
        # try:
        #     print(reader_pipe.recv())
        # except:
        #     break
        key = input(">>")
        # reader_pipe.send(key)
    # p.join()

    reader_process = mp.Process(target=reader, args=())


# class a
