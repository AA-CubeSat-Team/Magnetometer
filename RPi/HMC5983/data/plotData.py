import matplotlib.pyplot as plt
import numpy as np
import csv
from mpl_toolkits.mplot3d import Axes3D

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


ax.scatter(xuncal_list, yuncal_list, zuncal_list, label='Uncalibrated')
ax.scatter(xcal_list, ycal_list, zcal_list, label='Calibrated')
ax.set(xlabel='X mag', ylabel='Y mag', zlabel='Z mag')
ax.legend()
ax.set_aspect('equal')
axis_range = (maxVal-minVal)/2.0
ax.set_xlim(-axis_range, axis_range)
ax.set_ylim(-axis_range, axis_range)
ax.set_zlim(-axis_range, axis_range)


fig2 = plt.figure(2, figsize =(6,6))
ax1 = fig2.add_subplot(221);

#fig3 = plt.figure(3, figsize=(6,6))
ax2 = fig2.add_subplot(222);

#fig4 = plt.figure(4, figsize= (6,6))
ax3 = fig2.add_subplot(223);


ax1.scatter(xuncal_list, yuncal_list,label='Uncalibrated')
ax1.scatter(xcal_list, ycal_list,label='Calibrated')
ax1.set(xlabel='X', ylabel='Y')
ax1.legend()
ax1.axis('equal')


ax2.scatter(zuncal_list, yuncal_list, label='Uncalibrated');
ax2.scatter(zcal_list, ycal_list, label='Calibrated');
ax2.set(xlabel='Z', ylabel='Y')
ax2.legend()
ax2.axis('equal')

ax3.scatter(xuncal_list, zuncal_list, label='Uncalibrated');
ax3.scatter(xcal_list, zcal_list, label='Calibrated');
ax3.set(xlabel='X', ylabel='Z')
ax3.legend()
ax3.axis('equal')

plt.show()
