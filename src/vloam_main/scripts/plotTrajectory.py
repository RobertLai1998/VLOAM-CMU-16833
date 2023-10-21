import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

lo = np.loadtxt('../results/2011_09_26_drive_0011/LO1.txt')
vo = np.loadtxt('../results/2011_09_26_drive_0011/MO1.txt')
mo = np.loadtxt('../results/2011_09_26_drive_0011/VO1.txt')

fig = plt.figure(1)
ax = fig.add_subplot(111, projection='3d')
ax.scatter(lo[:,0], lo[:,1], lo[:,2], c='r')
ax.scatter(vo[:,0], vo[:,1], vo[:,2], c='g')
ax.scatter(mo[:,0], mo[:,1], mo[:,2], c='b')
plt.show()
