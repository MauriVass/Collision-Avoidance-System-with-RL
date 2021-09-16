import string
from flask import Flask, request, abort, Response, send_file
import json
import numpy as np
import tensorflow as tf
import time

app = Flask(__name__)

class Network():
	def __init__(self):
		self.num_actions = 3
		self.num_inputs = 16
		self.batchsize = 16
		self.discount_rate = 0.75
		self.lr = 0.01
		self.gamma = 0.99
		self.optimizer = tf.optimizers.Adam(self.lr)
		
		self.filepath = 'experiences.csv'
		self.file = '' #initialize after
		self.experiences_raw = []
		self.readExperiencesFromFile()

		self.minNumExperiences = 2000
		self.maxNumExperiences = 20000
		self.steps = 0
		self.copyWeightStepsteps = 24

		self.policyNetwork = self.ModelTemplate()

		self.targetNetwork = self.ModelTemplate()
		self.copyNN()
		print(self.policyNetwork.summary())

	def ModelTemplate(self):
			model = tf.keras.Sequential([
							tf.keras.layers.Flatten(),
							# tf.keras.layers.Dense(64, activation='relu'),
							# tf.keras.layers.Dense(64, activation='relu'),
							tf.keras.layers.Dense(128, activation='tanh', kernel_initializer='RandomNormal'),
							tf.keras.layers.Dense(self.num_actions, activation='softmax', kernel_initializer='RandomNormal')
						])
			model.build(input_shape=(1,self.num_inputs))
			return model

	def readExperiencesFromFile(self):
		self.file = open(self.filepath,'a+')

		self.file.seek(0)
		for l in self.file.readlines():
			self.experiences_raw.append(l)
	def writeExperienceToFile(self,experience):
		self.experiences_raw.append(experience)
		self.file.write(experience+'\n')
	def closeFile(self):
		self.file.close()

	def dictFromExperienceRaw(self,experience_raw):
		elements = experience_raw.split(';')

		currentState = tf.Variable([np.array(elements[0].split(':'), dtype=np.int8)])
		action = int(elements[1])
		reward = float(elements[2])
		nextAction = tf.Variable([np.array(elements[3].split(':'), dtype=np.int8)])
		gameEnded = True if elements[4]==1 else False
		exp = {'s': currentState, 'a': action, 'ns': nextAction, 'r': reward, 'done': gameEnded}
		return exp

	def getBatchExperiences(self):
		experiences = {'s': [], 'a': [], 'r': [], 'ns': [], 'done': []}

		indices = np.random.randint(len(self.experiences_raw), size=self.batchsize)

		for i in indices:
			experience = self.dictFromExperienceRaw(self.experiences_raw[i])
			if(len(self.experiences_raw)>self.maxNumExperiences):
				experiences.pop()
			experiences.append(experience)
		return experiences


	def fit(self, exp):
		if(len(self.experiences_raw)>=self.minNumExperiences):

			experiences = self.getBatchExperiences()
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

			with tf.GradientTape() as tape:
				tape.watch(states)
				
				a = self.policyNetwork(states) * tf.one_hot(actions, self.num_actions)
				selected_action_values = tf.math.reduce_sum(a , axis=1)

				loss = tf.math.reduce_mean(tf.square(actual_values - selected_action_values))
				tape.watch(loss)



			variables = self.policyNetwork.trainable_variables
			gradients = tape.gradient(loss, variables)
			print('\n#####\tloss\t#####', loss)
			# print('variables', type(variables), np.array(variables).shape)
			# print('wv', tape.watched_variables())
			# print('gradients', type(gradients), np.array(gradients).shape)
			# print(gradients)
			# time.sleep(3)

			self.optimizer.apply_gradients(zip(gradients, variables))

			self.steps+=1
			if(self.steps>=self.copyWeightSteps):
				self.copyNN()
				self.counter=0
		
		self.writeExperienceToFile(exp)

	def predictPN(self, input):
		return self.policyNetwork(np.atleast_2d(input))

	def predictTN(self, input):
		return self.targetNetwork(np.atleast_2d(input))
	
	def copyNN(self):
		self.targetNetwork.set_weights(self.policyNetwork.get_weights())


network = Network()

@app.route('/')
def home():
	print('home')
	return 'bella';

@app.route('/fit', methods=['POST'])
def FIT():
	input = request.data.decode("utf-8") 

	network.fit(input)

	return ''

@app.route('/predict', methods=['POST'])
def PREDICT():
	msg = request.data.decode("utf-8") 
	print(type(msg),msg)
	input = np.array(msg.split(':')).astype(int)
	# print('PREDICT')
	print(input)
	# output = self.net.predictPN(input)
	# # print('pred net', output)
	# # print('target net', self.net.predictTN(input))
	# out = '/'.join([str(o) for o in output.numpy()[0]])
	return '1;0.4'

	# def PREDICT_TN(self):
	# 	output = self.net.predictTN(values[0])
	# 	out = '.'.join([str(o) for o in output])
	# 	#print('tn', out, np.shape(out))
	# 	return out

	# def RESET(self):
	# 	msg = cherrypy.request.body.readline().decode("utf-8")
	# 	input = int(msg) #np.array(msg.split('.')).astype(int)
	# 	print(input)
	# 	self.net = Network(input)
	# 	print('Reset')

app.run(host='0.0.0.0')
network.closeFile()
