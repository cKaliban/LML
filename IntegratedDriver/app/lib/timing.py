# import matplotlib.pyplot as plt
# import matplotlib.animation as animation
# import time

# fig = plt.figure()
# ax1 = fig.add_subplot(1, 1, 1)

# cnt=0
# xComponent=[]

# line,  = ax1.plot([0], [0])
# text = ax1.text(0.97,0.97, "", transform=ax1.transAxes, ha="right", va="top")

# plt.ylim(0,25)
# plt.xlim(0,100)
# last_time = {0: time.time()}
# def animate(i):

#     if len(xComponent)>100:
#         xComponent.pop(0)
#     y = i % 25
#     xComponent.append(y)

#     line.set_data(range( len(xComponent) ) ,xComponent)
#     new_time = time.time()
#     if new_time == last_time[0]:
#         new_time = last_time[0] + 0.0001
#     text.set_text("{0:.2f} fps".format(1./(new_time-last_time[0])))
#     last_time.update({0:new_time})


# ani = animation.FuncAnimation(fig, animate, interval=0)
# plt.show()

import matplotlib.pyplot as plt
from matplotlib import animation
# from numpy import random 
import random

limit = 1000
fig, (ax1, ax2) = plt.subplots(2,1)
line1, = ax1.plot([], [], lw=2)
line2, = ax2.plot([], [], lw=2, color='r', )
lines = [line1, line2]

# for ax in [ax1, ax2]:
ax1.set_ylim(0, 200)
ax2.set_ylim(0,20)

ax1.grid()
ax2.grid()

xdata, y1data, y2data = [],[],[]

xlen = 200
def run(i, xdata, y1data, y2data):
    t = i
    y1 = random.randint(0, 200)
    # t, y1, y2 = data
    y2 = random.randint(0,20)
    xdata.append(t)
    y1data.append(y1)
    y2data.append(y2)
    xdata = xdata[-limit:]
    y1data = y1data[-limit:]
    y2data = y2data[-limit:]

    for ax in [ax1, ax2]:
        xmin, xmax = ax.get_xlim()
        if t >= limit:
            ax.set_xlim(t-limit, t)
            ax.figure.canvas.draw()
        elif xmax < t:
            ax.set_xlim(0, limit)
            ax.figure.canvas.draw()
        
    lines[0].set_data(xdata, y1data)
    lines[1].set_data(xdata, y2data)

    return lines

ani = animation.FuncAnimation(fig, run, fargs=(xdata, y1data,y2data),blit = True, interval = 10)
plt.show()



# fig = plt.figure()
# ax1 = plt.axes(xlim=(-108, -104), ylim=(31,34))
# line, = ax1.plot([], [], lw=2)
# plt.xlabel('Longitude')
# plt.ylabel('Latitude')

# plotlays, plotcols = [2], ["black","red"]
# lines = []
# for index in range(2):
#     lobj = ax1.plot([],[],lw=2,color=plotcols[index])[0]
#     lines.append(lobj)


# def init():
#     for line in lines:
#         line.set_data([],[])
#     return lines

# x1,y1 = [],[]
# x2,y2 = [],[]

# # fake data
# frame_num = 100
# gps_data = [-104 - (4 * random.rand(2, frame_num)), 31 + (3 * random.rand(2, frame_num))]


# def animate(i):

#     x = gps_data[0][0, i]
#     y = gps_data[1][0, i]
#     x1.append(x)
#     y1.append(y)

#     x = gps_data[0][1,i]
#     y = gps_data[1][1,i]
#     x2.append(x)
#     y2.append(y)

#     xlist = [x1, x2]
#     ylist = [y1, y2]

#     #for index in range(0,1):
#     for lnum,line in enumerate(lines):
#         line.set_data(xlist[lnum], ylist[lnum]) # set data for each line separately. 

#     return lines

# # call the animator.  blit=True means only re-draw the parts that have changed.
# anim = animation.FuncAnimation(fig, animate, init_func=init,
#                                frames=frame_num, interval=10, blit=True)


# plt.show()