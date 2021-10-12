import matplotlib.pyplot as plt
times = []
with open('output', 'r') as f:
    for line in f:
        times.append(float(line))
print(len(times))
ax1 = plt.subplot(211)
ax2 = plt.subplot(212)
ax1.bar(['small', 'medium'], times[:2])
ax2.bar(['small', 'medium', 'big'], [1,2,3])
plt.show()
