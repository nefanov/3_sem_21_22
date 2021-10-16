import matplotlib.pyplot as plt
times = []
with open('output', 'r') as f:
    for line in f:
        times.append(float(line))
print(len(times))
ax1 = plt.subplot(311)
ax2 = plt.subplot(312)
ax3 = plt.subplot(313)
ax1.bar(['small', 'medium', 'big'], times[:3])
ax2.bar(['small', 'medium', 'big'], times[3:6])
ax3.bar(['small', 'medium', 'big'], [x/10 for x in times[6:9]])
plt.savefig('plot.png')
