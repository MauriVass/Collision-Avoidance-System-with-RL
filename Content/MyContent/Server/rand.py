import numpy as np
import pandas as pd 
f = pd.read_csv('time_elapsed')

print(np.array(f).mode())