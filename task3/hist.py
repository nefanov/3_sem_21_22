import matplotlib.pyplot as plt
from math import log10 as lg
times = []
with open('output', 'r') as f:
    for line in f:
        times.append(lg(float(line)))
fig, [ax1, ax2, ax3] = plt.subplots(nrows=3,ncols=1)
fig.tight_layout()
ax1.bar(['small', 'medium', 'big'], times[:3])
ax1.set_title('message queue')
ax1.set_ylabel('lg(seconds)')
ax2.bar(['small', 'medium', 'big'], times[3:6])
ax2.set_title('shared memory')
ax3.bar(['small', 'medium', 'big'], [x for x in times[6:9]])
ax3.set_title('FIFO')
plt.savefig('plot.png')
