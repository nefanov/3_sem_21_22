import matplotlib.pyplot as plt
import numpy as np

y = []
with open('res.txt', 'r') as file:
    y = [line.rstrip('\n') for line in file]
    y = [float(string) for string in y]

x = np.arange(1, 21)
plt.figure(figsize=(10, 10))
plt.grid()
plt.plot(x[0:14], y[0:14])
plt.show()
