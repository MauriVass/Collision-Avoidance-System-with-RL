from scipy.stats import t
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

def evaluate_conf_interval_(x,runs,confidence_level):
	t_sh = t.ppf((confidence_level + 1) / 2, df=runs - 1) # threshold for t_student

	ave = x.mean() # average
	stddev = x.std(ddof=1) # std dev
	ci = t_sh * stddev / np.sqrt(runs) # confidence interval half width

	#print(ave, stddev, t_sh, runs, ci, ave)
	rel_err = ci / ave if ave>0 else 0
	return ave, ci, rel_err

def evaluate_conf_interval(data,confidence_level):
	runs = len(data)
	t_sh = t.ppf((confidence_level + 1) / 2, df=runs - 1) # threshold for t_student
	avgs = []
	lbs = []
	ubs = []
	for i in range(len(data[0])):
		x = []
		for j in range(runs):
			x.append(data[j][i])
		x = np.array(x)
		ave = x.mean() # average
		stddev = x.std(ddof=1) # std dev
		ci = t_sh * stddev / np.sqrt(runs) # confidence interval half width
		# rel_err = ci / ave # relative error
		avgs.append(ave)
		lbs.append(ave-ci)
		ubs.append(ave+ci)
	return avgs, lbs, ubs, ci

models = [0, 1, 2]
colors = ['r', 'g', 'b']
labels = ['DQN', 'DDQN', 'DDDQN']

train = True

if(train):
	for m in models:

		confidence_level = 0.95
		model = m
		n_runs = 5
		runs_reward = []
		runs_speed = []
		for i in range(n_runs):
			data = pd.read_csv(f'Runs/Model{model}/{i}/run.rewards_{model}_{i}.csv')
			rolling = 9
			data['rolling_avg_reward'] = data['totalReward'].rolling(rolling).mean()
			data['rolling_avg_speed'] = data['averageSpeed'].rolling(rolling).mean()
			runs_reward.append(data['rolling_avg_reward'])#totalReward
			runs_speed.append(data['rolling_avg_speed'] / 50) #averageSpeed [m/s]
		avg_r, lbs_r, ubs_r, _ = evaluate_conf_interval(runs_reward,confidence_level)
		avg_s, lbs_s, ubs_s, _ = evaluate_conf_interval(runs_speed,confidence_level)

		f = open(f'Runs/Model{model}/statistics_train_{model}.csv','w')
		f.write('AvgR,LbR,UbR,AvgS,LbS,UbS\n')
		for i in range(len(avg_r)):
			f.write(f'{avg_r[i]},{lbs_r[i]},{ubs_r[i]},{avg_s[i]},{lbs_s[i]},{ubs_s[i]}\n')
		f.close()

		for i in range(2):
			fig, ax = plt.subplots(figsize=(12, 6))
			ax.set_xlabel('Episode')
			c = colors[m]
			l = labels[m]
			if(i==0):
				ax.plot(range(len(avg_r)),avg_r, markersize=3, marker="o",color=c, label=l)
				ax.fill_between(range(len(avg_r)), lbs_r, ubs_r, alpha=0.2,color=c)
				plt.ylim([0, 2500])
				ax.legend(loc='upper left')
				ax.set_ylabel('Cumulative Reward')
				plt.savefig(f'Runs/Model{model}/plot{model}_reward.png')
				plt.show()
			else:
				ax.plot(range(len(avg_s)),avg_s, markersize=3, marker="o",color=c, label=l)
				ax.fill_between(range(len(avg_s)), lbs_s, ubs_s, alpha=0.2,color=c)
				ax.set_ylabel('Average Speed [m/s]')
				ax.legend(loc='upper left')
				plt.savefig(f'Runs/Model{model}/plot{model}_speed.png')
				plt.show()
else:
	f = open(f'Runs/statistics_test.csv','w')
	f.write('Model,AvgR,CIR,AvgS,CIS\n')
	for m in models:	
		confidence_level = 0.95
		model = m
		n_runs = 50

		data = pd.read_csv(f'Runs/Model{model}/run_test.rewards_{model}.csv')
		avg_r, ci_r, _ = evaluate_conf_interval_(data['totalReward'],n_runs,confidence_level)
		avg_s, ci_s, _ = evaluate_conf_interval_(data['averageSpeed'] / 50,n_runs,confidence_level)

		f.write(f'{m},{avg_r},{ci_r},{avg_s},{ci_s}\n')
	f.close()
