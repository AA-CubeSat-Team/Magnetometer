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


ax.scatter(xcal_list, ycal_list, zcal_list, label='Cal')
ax.scatter(xuncal_list, yuncal_list, zuncal_list, label='Uncal')
ax.set(xlabel='X mag', ylabel='Y mag', zlabel='Z mag')
ax.legend()
ax.axis('equal')

fig2 = plt.figure(2, figsize =(6,6))
ax1 = fig2.add_subplot(221);

#fig3 = plt.figure(3, figsize=(6,6))
ax2 = fig2.add_subplot(222);

#fig4 = plt.figure(4, figsize= (6,6))
ax3 = fig2.add_subplot(223);

ax1.scatter(xcal_list, ycal_list,label='Cal')
ax1.scatter(xuncal_list, yuncal_list,label='Uncal')
ax1.set(xlabel='X', ylabel='Y')
ax1.legend()
ax1.axis('equal')


ax2.scatter(zcal_list, ycal_list, label='Cal');
ax2.scatter(zuncal_list, yuncal_list, label='Uncal');
ax2.set(xlabel='Z', ylabel='Y')
ax2.legend()
ax2.axis('equal')

ax3.scatter(xcal_list, zcal_list, label='Cal');
ax3.scatter(xuncal_list, zuncal_list, label='Uncal');
ax3.set(xlabel='X', ylabel='Z')
ax3.legend()
ax3.axis('equal')

plt.show()
