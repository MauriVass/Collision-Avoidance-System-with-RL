import string
from flask import Flask, request, abort, Response, send_file
import json
import numpy as np
import tensorflow as tf
import time
from datetime import datetime

app = Flask(__name__)

RANDOM_SEED = 21
tf.random.set_seed(RANDOM_SEED)
np.random.seed(RANDOM_SEED)

class Network():
	def __init__(self):
		self.batchsize = 64
		self.discount_rate = 0.95
		self.lr = 0.001/10
		self.gamma = 0.99
		self.tau = 0.99
		self.optimizer = tf.optimizers.Adam(self.lr)
		
		self.filepath = 'experiences.csv'
		self.file = '' #initialize after
		self.experiences_raw = []
		self.experiences_prepros = []

		self.is_action_space_descrete = True
		self.readExperiencesFromFile()
		# t1 = time.time()
		# print(f'Reading {time.time()-t1}')

		self.minNumExperiences = self.batchsize * 2
		self.maxNumExperiences = 2*10**4
		self.steps = 0
		self.copyWeightSteps = 64

		self.fit_times = []
		self.losses = []	
		
		self.t1 =0
		self.t2=0
		self.f = open('updates','w')

	def Initialization(self, num_inputs, is_action_space_descrete, num_actions):
		self.num_inputs = num_inputs
		self.is_action_space_descrete = is_action_space_descrete
		self.num_actions = num_actions
		
		#self.readExperiencesFromFile()

		self.policyNetwork = self.ModelTemplate()

		self.targetNetwork = self.ModelTemplate()
		self.copyNN(soft=True)
		print(self.policyNetwork.summary())
		self.t1 = time.time()

	def ModelTemplate(self):
			model = tf.keras.Sequential([
							tf.keras.layers.Flatten(),
							tf.keras.layers.Dense(32, activation='tanh', kernel_initializer='RandomNormal'),
							#tf.keras.layers.Dense(64, activation='relu', kernel_initializer='RandomNormal'),
							tf.keras.layers.Dense(self.num_actions, activation='linear', kernel_initializer='RandomNormal') #softmax
						])
			model.build(input_shape=(1,self.num_inputs))
			return model

	def readExperiencesFromFile(self):
		self.file = open(self.filepath,'r')

		for l in self.file.readlines():
			# self.experiences_raw.append(l.strip())
			self.experiences_prepros.append(self.dictFromExperienceRaw(l.strip()))
		self.closeFile()
	def writeExperienceToFile(self,experience):
		# self.experiences_raw.append(experience)
		self.experiences_prepros.append(self.dictFromExperienceRaw(experience))
		# self.file.write(experience+'\n')
	def writeAllExperiences(self):
		self.file = open(self.filepath,'w')

		diff = len(self.experiences_prepros) - self.maxNumExperiences
		print(diff)
		if(diff>0):
			inds = np.random.randint(len(self.experiences_prepros), size=diff)
			self.experiences_prepros = np.delete(self.experiences_prepros, inds)
		# for e in self.experiences_raw:
		for e in self.experiences_prepros:
			cs = ':'.join( [str(x) for x in e['s'].numpy()[0]] )

			if(self.is_action_space_descrete):
				a = str(e['a'])
			else:
				thro_a = str(e['a'][0])
				steer_a = str(e['a'][1])
				a = thro_a+':'+steer_a
			ns = ':'.join( [str(x) for x in e['ns'].numpy()[0]] )
			r = str(e['r'])
			end = str(int(e['done']))

			self.file.write( ';'.join([cs,a,ns,r,end]) +'\n')
		self.file.close()
	def closeFile(self):
		self.file.close()

	def dictFromExperienceRaw(self,experience_raw):
		elements = experience_raw.split(';')

		currentState = tf.Variable([np.array(elements[0].split(':'), dtype=np.float16)])
		
		if(self.is_action_space_descrete):
			action = float(elements[1])
		else:
			throttle_action = float(elements[1].split(':')[0])
			steer_action = float(elements[1].split(':')[1])
			action = [throttle_action, steer_action]
		nextAction = tf.Variable([np.array(elements[2].split(':'), dtype=np.float16)])
		reward = float(elements[3])
		gameEnded = True if elements[4]==1 else False
		
		# exp = {'s': currentState, 'a': action, 'ns': nextAction, 'r': reward, 'done': gameEnded}
		exp = {'s': currentState, 'a': action, 'ns': nextAction, 'r': reward, 'done': gameEnded}
		return exp

	def getBatchExperiences(self):
		experiences = {'s': [], 'a': [], 'r': [], 'ns': [], 'done': []}

		# t1 = time.time()
		# indices = np.random.randint(len(self.experiences_raw), size=self.batchsize)
		indices = np.random.randint(len(self.experiences_prepros), size=self.batchsize)
		# print(f'\t getBatchExperiences: 1 rand: {(time.time()-t1):.3f}')

		# t_fetch = 0
		# t_append = 0
		for i in indices:
			# t1 = time.time()
			# experience = self.dictFromExperienceRaw(self.experiences_raw[i])
			experience = self.experiences_prepros[i]
			# t_fetch += time.time()-t1
			# if(len(self.experiences_raw)>self.maxNumExperiences):
			# 	experiences.pop()
			# experiences.append(experience)
			# t1 = time.time()
			for k,v in experience.items():
				experiences[k].append(v)
			# t_append += time.time()-t1
		# print(f'\t getBatchExperiences: 2 fetch: {(t_fetch):.3f}')
		# print(f'\t getBatchExperiences: 3 append: {(t_append):.3f}')
		return experiences


	def fit(self, exp):
		# if(len(self.experiences_raw)>=self.minNumExperiences):
		if(len(self.experiences_prepros)>=self.minNumExperiences):

			# t1 = time.time()
			experiences = self.getBatchExperiences()
			# print(f'Fit: 1 getbatch: {(time.time()-t1):.3f}')

			# t1 = time.time()
			states = tf.convert_to_tensor(experiences['s'], dtype=tf.float32) #The current state
			# print('states', states.shape, states)
			actions = np.asarray(experiences['a']) #The action performed (chosen randomly or by net)
			# print('actions', actions.shape, actions)
			rewards = np.asarray(experiences['r']) #The reward got
			# print('rewards', rewards.shape, rewards)
			next_states = tf.convert_to_tensor(experiences['ns']) #The next state
			# print('next_states', next_states.shape, next_states)
			dones = np.asarray(experiences['done']) #State of the game (ended or not)
			# print('dones', dones.shape, dones)
			value_next = np.max(self.targetNetwork(next_states),axis=1) #Max next values according to the Target Network
			# print('value_next', value_next.shape, value_next)
			actual_values = np.where(dones, rewards, rewards+self.gamma*value_next)
			# print('actual_values', actual_values.shape, actual_values)
			# print(f'Fit: 2 generate tensors: {(time.time()-t1):.3f}')

			# t1 = time.time()
			with tf.GradientTape() as tape:
				tape.watch(states)

				if(self.is_action_space_descrete):
					a = self.policyNetwork(states) * tf.one_hot(actions, self.num_actions)
				else:
					a = self.policyNetwork(states)
				selected_action_values = tf.math.reduce_sum(a , axis=1)

				#MES
				# loss = tf.math.reduce_mean(tf.square(actual_values - selected_action_values))
				#Huber Loss
				loss_function = tf.keras.losses.Huber(reduction=tf.keras.losses.Reduction.SUM_OVER_BATCH_SIZE)
				loss = loss_function(actual_values, selected_action_values)

				tape.watch(loss)
				print('#####\tloss\t#####', loss)
				self.losses.append(loss)
			# print(f'Fit: 3 gradient: {(time.time()-t1):.3f}')

			# t1 = time.time()
			variables = self.policyNetwork.trainable_variables
			gradients = tape.gradient(loss, variables)
			#gradients, _ = tf.clip_by_global_norm(gradients, 5.0)
			# print('variables', type(variables), np.array(variables).shape)
			# print('wv', tape.watched_variables())
			# print('gradients', type(gradients), np.array(gradients).shape)
			# print(gradients)
			# time.sleep(3)

			self.optimizer.apply_gradients(zip(gradients, variables))
			# print(f'Fit: 4 backprog: {(time.time()-t1):.3f}')

			self.steps+=1
			if(self.steps>=self.copyWeightSteps):
				# t1 = time.time()
				self.steps=0
				self.copyNN()
				self.t2 = time.time()
				t = self.t2-self.t1
				#print(f'\t\t\tTime from last update: {():.3f}')
				self.f.write(f'{t}\n')
				self.t1 = self.t2
				# print(f'Fit: 5 copy weights: {(time.time()-t1):.3f}')

		
		# t1 = time.time()
		self.writeExperienceToFile(exp)
		# print(f'Fit: 6 write to file: {(time.time()-t1):.3f}')

	def predictPN(self, input):
		return self.policyNetwork(np.atleast_2d(input))

	def predictTN(self, input):
		return self.targetNetwork(np.atleast_2d(input))
	
	def copyNN(self, soft=False):
		if(soft):
			self.targetNetwork.set_weights(self.policyNetwork.get_weights())
		else:
			targetN_weights = self.targetNetwork.get_weights()
			policyN_weights = self.policyNetwork.get_weights()
			new_weights = []
			for t,p in zip(targetN_weights,policyN_weights):
				new_weights.append(self.tau * t + (1-self.tau) * p)
			# c = self.tau * a + (1-self.tau) * b
			self.targetNetwork.set_weights(new_weights)
	
	def saveModel(self,name=''):
		timestamp = datetime.now().strftime("%d_%m_%Y_%H_%M_%S")
		path = f'{timestamp}_{name}_{self.num_inputs}_{self.num_actions}'
		self.policyNetwork.save(f'Models/{path}')
		return path
	def loadModel(self,name):
		self.policyNetwork = tf.keras.models.load_model(f"Models/{name}")
		self.copyNN()
		print(self.policyNetwork.summary())

network = Network()

@app.route('/')
def home():
	print('home')
	return 'server';

@app.route('/initialization', methods=['POST'])
def INITIALIZATION():
	msg = request.data.decode("utf-8")
	input = np.array(msg.split(':'))
	num_sensors = int(input[0])
	is_action_space_descrete = False if input[1]=='False' else True
	num_actions = int(input[2])
	network.Initialization(num_sensors,is_action_space_descrete,num_actions)
	return 'initialization'

@app.route('/fit', methods=['POST'])
def FIT():
	time_start = time.time()
	input = request.data.decode("utf-8") 

	network.fit(input)
	time_elaps = time.time()-time_start
	network.fit_times.append(time_elaps)
	print(f'Fit: elapsed time: {(time_elaps):.3f}\n')

	return 'fitted'

@app.route('/predict', methods=['POST'])
def PREDICT():
	time_start = time.time()
	msg = request.data.decode("utf-8") 
	# print(type(msg),msg)
	input = np.array(msg.split(':')).astype(int)
	# print('PREDICT')
	#print(input)
	output = network.predictPN(input)

	if(network.is_action_space_descrete):
		a = np.argmax(output)
		output = str(a)
	else:
		a = output.numpy()[0] #np.argmax(output)
		output = f'{a[0]};{a[1]}'
	#print('pred net', output, action)
	# # print('target net', self.net.predictTN(input))
	# out = '/'.join([str(o) for o in output.numpy()[0]])
	time_end = time.time()
	print(f'Predict: elapsed time: {(time_end-time_start):.3f}')
	return output

@app.route('/copy', methods=['GET'])
def COPY_WEIGHTS():
	network.copyNN()
	return 'copied'

@app.route('/save', methods=['GET'])
def SAVE_MODEL():
	#http://192.168.1.X:5000/save?file=XX
	file_name = request.args['file']
	path = network.saveModel(file_name)
	return f'saved to {path}'

@app.route('/load', methods=['GET'])
def LOAD_MODEL():
	#http://192.168.1.X:5000/load?file=XX
	path = request.args['file']
	network.loadModel(path)
	return f'loaded {path}'

@app.route("/shutdown", methods=['GET'])
def shutdown():
	shutdown_func = request.environ.get('werkzeug.server.shutdown')
	if shutdown_func is None:
		raise RuntimeError('Not running werkzeug')
	shutdown_func()
	return "Shutting down..."

	# def RESET(self):
	# 	msg = cherrypy.request.body.readline().decode("utf-8")
	# 	input = int(msg) #np.array(msg.split('.')).astype(int)
	# 	print(input)
	# 	self.net = Network(input)
	# 	print('Reset')

app.run(host='0.0.0.0')
# network.closeFile()
network.writeAllExperiences()
network.f.close()

file = open('time_elapsed','w')
for i in network.fit_times:
	file.write(str(i)+'\n')
file.close()

file = open('run.losses.csv','w')
for i in network.losses:
	file.write(str(i.numpy())+'\n')
file.close()

