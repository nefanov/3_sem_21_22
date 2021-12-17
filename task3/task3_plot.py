import numpy as np
import matplotlib.pyplot as plt

y = np.array([3.3, 3.4, 12, 3.3, 3.5, 13, 7.6, 74, 1021.3])
y = np.log(y)
fig, [ax1, ax2, ax3] = plt.subplots(nrows=3, ncols=1)
fig.tight_layout()
ax1.bar(['small', 'medium', 'big'], y[:3])
ax1.set_title('message queue')
ax1.set_ylabel('lg(seconds)')
ax2.bar(['small', 'medium', 'big'], y[3:6])
ax2.set_title('FIFO')
ax3.bar(['small', 'medium', 'big'], y[6:9])
ax3.set_title('shared memory')
plt.savefig('t3_plot.png')
