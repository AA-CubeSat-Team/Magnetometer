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
        
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.scatter(xuncal_list, yuncal_list, zuncal_list)
ax.scatter(xcal_list, ycal_list, zcal_list)
ax.set(xlabel='X mag', ylabel='Y mag', zlabel='Z mag')

fig2 = plt.figure(2)
ax1 = fig2.add_subplot(221);
ax2 = fig2.add_subplot(222);
ax3 = fig2.add_subplot(223);

ax1.scatter(xuncal_list, yuncal_list);
ax1.scatter(xcal_list, ycal_list);

ax2.scatter(zuncal_list, yuncal_list);
ax2.scatter(zcal_list, ycal_list);

ax3.scatter(xuncal_list, zuncal_list);
ax3.scatter(xcal_list, zcal_list);


plt.show()
