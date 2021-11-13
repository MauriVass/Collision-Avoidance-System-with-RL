import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

n = 3
colors = ['r', 'g', 'b']
labels = ['DQN', 'DDQN', 'DDDQN']

fig_r, ax_r = plt.subplots(figsize=(24,8))
fig_s, ax_s = plt.subplots(figsize=(24,8))
for i in range(n):
	data = pd.read_csv(f'Runs/Model{i}/statistics_{i}.csv')

	avg_r = data['AvgR']
	lbs_r = data['LbR']
	ubs_r = data['UbR']
	ax_r.plot(range(len(avg_r)),avg_r, markersize=3, marker="o", color=colors[i], label=labels[i])
	ax_r.fill_between(range(len(avg_r)), lbs_r, ubs_r, alpha=0.2, color=colors[i])

	# fig.suptitle(f'Rolling: {rolling}')

	#ax.scatter(x, y, s=0.1)
	avg_s = data['AvgS']
	lbs_s = data['LbS']
	ubs_s = data['UbS']
	ax_s.plot(range(len(avg_s)),avg_s, markersize=3, marker="o", color=colors[i], label=labels[i])
	ax_s.fill_between(range(len(avg_s)), lbs_s, ubs_s, alpha=0.2, color=colors[i])
	# axs[0].set_ylabel('Total Reward')

	# axs[1].plot(x,y2,linewidth=.5, markersize=2)
	# axs[1].scatter(x,y2,c=colors, s=2)
	# axs[1].set_xlabel('Epoch')
	# axs[1].set_ylabel('Avg Velocity')
	# plt.savefig(f'Plots/rewards_{rolling}')
ax_r.set_ylim(0, 2500)
ax_r.legend(loc='upper left')
ax_r.set_xlabel('Episode')
ax_r.set_ylabel('Cumulative Reward')

ax_s.legend(loc='upper left')
ax_s.set_xlabel('Episode')
ax_s.set_ylabel('Average Speed [m/s]')
plt.show()