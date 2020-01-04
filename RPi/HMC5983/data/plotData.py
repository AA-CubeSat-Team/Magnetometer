import matplotlib.pyplot as plt
import numpy as np
import csv
from mpl_toolkits.mplot3d import Axes3D

#calibrated_file = "best_cal2.csv"
#uncalibrated_file = "best_uncal2.csv"
calibrated_file = "calibrated.csv"
uncalibrated_file = "uncalibrated.csv"

xcal_list = []
ycal_list = []
zcal_list = []
xuncal_list = []
yuncal_list = []
zuncal_list = []

with open(uncalibrated_file, newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='|')
    for row in reader:
        xuncal_list.append(float(row[0]))
        yuncal_list.append(float(row[1]))
        zuncal_list.append(float(row[2]))
        
with open(calibrated_file, newline='') as csvfile:
    reader = csv.reader(csvfile, delimiter=',', quotechar='|')
    for row in reader:
        xcal_list.append(float(row[0]))
        ycal_list.append(float(row[1]))
        zcal_list.append(float(row[2]))

maxVal = float("-inf")
minVal = float("inf")
for x in xcal_list:
    if(x > maxVal):
        maxVal = x
    if( x < minVal):
        minVal = x

for y in ycal_list:
    if(y > maxVal):
        maxVal = y
    if( y < minVal):
        minVal = y
for z in zcal_list:
    if(z > maxVal):
        maxVal = z
    if( z < minVal):
        minVal = z
        
fig = plt.figure()
ax = fig.gca(projection='3d')

dotSize = 3;

ax.scatter(xuncal_list, yuncal_list, zuncal_list, label='Uncalibrated', s=dotSize)
ax.scatter(xcal_list, ycal_list, zcal_list, label='Calibrated', s=dotSize)
ax.set(xlabel='X mag', ylabel='Y mag', zlabel='Z mag')
ax.legend()
ax.set_aspect('equal')
axis_range = (maxVal-minVal)/2.0
ax.set_xlim(-axis_range, axis_range)
ax.set_ylim(-axis_range, axis_range)
ax.set_zlim(-axis_range, axis_range)


fig2 = plt.figure(2, figsize =(6,6))
ax1 = fig2.add_subplot(221);
ax2 = fig2.add_subplot(222);
ax3 = fig2.add_subplot(223);


ax1.scatter(xuncal_list, yuncal_list,label='Uncalibrated', s=dotSize)
ax1.scatter(xcal_list, ycal_list,label='Calibrated', s=dotSize)
ax1.set(xlabel='X (Gauss)', ylabel='Y (Gauss)')
ax1.legend()
ax1.axis('equal')


ax2.scatter(zuncal_list, yuncal_list, label='Uncalibrated', s=dotSize);
ax2.scatter(zcal_list, ycal_list, label='Calibrated', s=dotSize);
ax2.set(xlabel='Z (Gauss)', ylabel='Y (Gauss)')
ax2.legend()
ax2.axis('equal')

ax3.scatter(xuncal_list, zuncal_list, label='Uncalibrated', s=dotSize);
ax3.scatter(xcal_list, zcal_list, label='Calibrated', s=dotSize);
ax3.set(xlabel='X (Gauss)', ylabel='Z (Gauss)')
ax3.legend()
ax3.axis('equal')


fig3 = plt.figure(3)
ax31 = fig3.add_subplot(121);
ax32 = fig3.add_subplot(122);

ax31.scatter(xuncal_list, yuncal_list,label='X vs Y', s=dotSize)
ax31.scatter(yuncal_list, zuncal_list,label='Y vs Z', s=dotSize)
ax31.scatter(zuncal_list, xuncal_list,label='Z vs X', s=dotSize)
ax31.set(xlabel='Gauss', ylabel='Gauss')
ax31.legend()
ax31.set_title("Uncalibrated")
ax31.axis('equal')

ax32.scatter(xcal_list, ycal_list,label='X vs Y', s=dotSize)
ax32.scatter(ycal_list, zcal_list,label='Y vs Z', s=dotSize)
ax32.scatter(zcal_list, xcal_list,label='Z vs X', s=dotSize)
ax32.set(xlabel='Gauss', ylabel='Gauss')
ax32.legend()
ax32.set_title("Calibrated")
ax32.axis('equal')

plt.show()
