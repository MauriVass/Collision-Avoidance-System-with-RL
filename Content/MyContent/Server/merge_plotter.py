import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

n = 3
colors = ['r', 'g', 'b']
labels = ['DQN', 'DDQN', 'DDDQN']#

train = True
x = 12
y = 6
fig_r, ax_r = plt.subplots(figsize=(x,y))
fig_s, ax_s = plt.subplots(figsize=(x,y))

if(train):
	for i in range(n):
		data = pd.read_csv(f'Runs/Model{i}/statistics_train_{i}.csv')

		avg_r = data['AvgR']
		lbs_r = data['LbR']
		ubs_r = data['UbR']
		print(i, np.max(avg_r))
		ax_r.plot(range(len(avg_r)),avg_r, markersize=3, marker="o", color=colors[i], label=labels[i])
		#ax_r.fill_between(range(len(avg_r)), lbs_r, ubs_r, alpha=0.2, color=colors[i])

		avg_s = data['AvgS']
		lbs_s = data['LbS']
		ubs_s = data['UbS']
		ax_s.plot(range(len(avg_s)),avg_s, markersize=3, marker="o", color=colors[i], label=labels[i])
		#ax_s.fill_between(range(len(avg_s)), lbs_s, ubs_s, alpha=0.2, color=colors[i])
else:
	data = pd.read_csv(f'Runs/statistics_test.csv')

	avg_r = data['AvgR']
	print(avg_r)
	ci_r = data['CIR']
	#ax_r.bar(range(len(labels)), avg_r, yerr=ci_r, align='center', color=colors, alpha=0.5, ecolor='black', capsize=5, bottom=4000)
	for avg,ci,x,c in zip(avg_r,ci_r,range(len(labels)),colors):
		ax_r.plot((x,x,x),(avg-ci/2,avg,avg+ci/2),'ro-',color=c)
	ax_r.set_xticks(range(len(labels)))
	ax_r.set_xticklabels(labels)
	
	avg_s = data['AvgS']
	ci_s = data['CIS']
	#ax_s.bar(range(len(labels)), avg_s, yerr=ci_s, align='center', color=colors, alpha=0.5, ecolor='black', capsize=5)
	for avg,ci,x,c in zip(avg_s,ci_s,range(len(labels)),colors):
		ax_s.plot((x,x,x),(avg-ci/2,avg,avg+ci/2),'ro-',color=c)
	ax_s.set_xticks(range(len(labels)))
	ax_s.set_xticklabels(labels)
	

if(train):
	ax_r.set_ylim(0, 2300)
	ax_r.legend(loc='upper left')
	ax_r.set_xlabel('Episode')
ax_r.set_ylabel('Cumulative Reward')
fig_r.savefig(f'Plots/rewards')

if(train):
	ax_s.legend(loc='upper left')
	ax_s.set_xlabel('Episode')
ax_s.set_ylabel('Average Speed [m/s]')
fig_s.savefig(f'Plots/speeds')
plt.show()