#to install
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

losses = pd.read_csv('run.losses.csv')

fig = plt.figure()
ax = plt.axes()

x = np.arange(len(losses))
y = np.array(losses)
y = np.clip(y, 0, 5)

ax.scatter(x, y, s=0.1)
plt.show()