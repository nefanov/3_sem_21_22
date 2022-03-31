import numpy as np
import matplotlib.pyplot as plt

y = np.array([116.7, 22, 20.5, 138.1, 26, 24, 1375, 2656, 1711])
fig, [ax1, ax2, ax3] = plt.subplots(nrows=3, ncols=1)
fig.tight_layout()
ax1.bar(['FIFO', 'shared memory', 'MSGQ'], y[:3])
ax1.set_title('200 b')
ax1.set_ylabel('10^(-6) seconds')
ax2.bar(['FIFO', 'shared memory', 'MSGQ'], y[3:6])
ax2.set_title('4 kb')
ax2.set_ylabel('10^(-6) seconds')
ax3.bar(['FIFO', 'shared memory', 'MSGQ'], y[6:9])
ax3.set_title('1 Mb')
ax3.set_ylabel('10^(-6) seconds')
plt.savefig('gist.png')
plt.show()