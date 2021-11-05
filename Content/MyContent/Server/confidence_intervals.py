from scipy.stats import t
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

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

confidence_level = 0.9
model = 2
n = 1
runs_reward = []
runs_speed = []
for i in range(n):
	data = pd.read_csv(f'Runs/Model{model}/{i}/run.rewards_{model}_{i}.csv')
	rolling = 9
	data['rolling_avg_reward'] = data['totalReward'].rolling(rolling).mean()
	data['rolling_avg_speed'] = data['averageSpeed'].rolling(rolling).mean()
	runs_reward.append(data['rolling_avg_reward'])#totalReward
	runs_speed.append(data['rolling_avg_speed'])#averageSpeed
avg_r, lbs_r, ubs_r, _ = evaluate_conf_interval(runs_reward,confidence_level)
avg_s, lbs_s, ubs_s, _ = evaluate_conf_interval(runs_speed,confidence_level)

f = open(f'Runs/Model{model}/statistics_{model}.csv','w')
f.write('AvgR,LbR,UbR,AvgS,LbS,UbS\n')
for i in range(len(avg_r)):
	f.write(f'{avg_r[i]},{lbs_r[i]},{ubs_r[i]},{avg_s[i]},{lbs_s[i]},{ubs_s[i]}\n')
f.close()

fig, ax = plt.subplots(figsize=(12, 6))

ax.plot(range(len(avg_r)),avg_r, markersize=3, marker="o")
ax.fill_between(range(len(avg_r)), lbs_r, ubs_r, alpha=0.2)
plt.ylim([0, 2500])
plt.show()

