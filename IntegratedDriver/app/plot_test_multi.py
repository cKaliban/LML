import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import random
import multiprocessing
from time import sleep
# multiprocessing.freeze_support() # <- may be required on windows


def plot():
    # Parameters
    x_len = 200         # Number of points to display
    y_range = [0, 40]  # Range of possible Y values to display

    # Create figure for plotting
    fig = plt.figure()
    ax = fig.add_subplot(1, 1, 1)
    fig, ax1 = plt.subplots()


    xs = list(range(0, 200))
    ys1 = [0] * x_len
    ys2 = [0] * x_len
    ax1.set_ylim(y_range)
    line, = ax1.plot(xs, ys1)
    plt.title('Temperature over time')
    plt.xlabel('Samples')
    plt.ylabel('Temperature (deg C)')
    plt.show(block=False)
    sleep(3)

    ani = animation.FuncAnimation(fig,
        update_plot,
        fargs=(line, ys1, ys2, x_len),
        interval=50,
        blit=True)
    plt.show(block=True)  

def init_plot():
    pass

def update_plot(i, lines, ys1, ys2, x_len):
    temp_c = random.randint(10,40)
    pwm_prc = random.randint(0,20)
    ys1.append(temp_c)
    ys1 = ys1[-x_len:]
    ys1.append(pwm_prc)
    # ys2 = ys2[-x_len:]
    lines.set_ydata(ys1)
    # lines[1].set_ydata(ys2)
    return lines


if __name__ == "__main__": 
    # input('Value: ') 
    
    # plot()
    # p = multiprocessing.Process(target=plot, args=())
    # p.start()
    while 1:
        mes = input(">>")
        print("<<" + mes)
        if mes == "START":
            p = multiprocessing.Process(target=plot, args=())
            p.start()
        if mes == "END":
            p.terminate()
            break
    