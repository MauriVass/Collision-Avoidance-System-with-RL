#to install
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

losses = pd.read_csv('run.losses.csv')
rewards = pd.read_csv('run.rewards.csv')

fig = plt.figure(figsize=(24,8))
ax = plt.axes()

plot_losses = True

if(plot_losses is True):
	x = np.arange(len(losses))
	y = np.array(losses)
	#y = np.clip(y, 0, 0.01)

	#ax.scatter(x, y, s=0.1)
	ax.plot(x,y,linewidth=.05, marker='o', markersize=0.5)
else:
	rolling = 7
	rewards['rolling_avg_reward'] = rewards['totalReward'].rolling(rolling).mean()
	rewards['rolling_avg_speed'] = rewards['averageSpeed'].rolling(rolling).mean()
	x = np.arange(len(rewards))
	y1 = np.array(rewards['rolling_avg_reward'])
	y2 = np.array(rewards['rolling_avg_speed'])

	#ax.scatter(x, y, s=0.1)
	ax.plot(x,y1,linewidth=.5, marker='o', markersize=2)
	ax.plot(x,y2,linewidth=.5, marker='o', markersize=2)
plt.show()