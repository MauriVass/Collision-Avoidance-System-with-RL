#to install
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

losses = pd.read_csv('run.losses.csv')
rewards = pd.read_csv('run.rewards').iloc[:,1]

fig = plt.figure(figsize=(24,8))
ax = plt.axes()

plot_losses = False

if(plot_losses is True):
	x = np.arange(len(losses))
	y = np.array(losses)
	#y = np.clip(y, 0, 0.01)

	#ax.scatter(x, y, s=0.1)
	ax.plot(x,y,linewidth=.05, marker='o', markersize=0.5)
else:
	x = np.arange(len(rewards))
	y = np.array(rewards)

	#ax.scatter(x, y, s=0.1)
	ax.plot(x,y,linewidth=.5, marker='o', markersize=2)
plt.show()