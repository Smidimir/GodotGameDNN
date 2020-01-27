import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
from network import Linear_QNet2
import random
import os
import numpy as np
from operator import add
from collections import deque

class DQNAgent_train(object):

    def __init__(self):
        self.gamma = 0.9
        self.epsilon = 0
        self.memory = deque()
        self.lr = 1e-4
        self.model = Linear_QNet2(11, 256, 3)
        self.model.train()
        self.optimizer = optim.Adam(self.model.parameters(), lr=0.001)
        self.loss_fn = nn.MSELoss()

    def remember(self, state, action, reward, next_state, done):
        self.memory.append([state, action, reward, next_state, done])
        if len(self.memory) > 100000:
            self.memory.popleft()

    def train_long_memory(self):
        if len(self.memory) > 1000:
            minibatch = random.sample(self.memory, 1000)
        else:
            minibatch = self.memory

        state, action, reward, next_state, done = zip(*minibatch)
        state = torch.tensor(state, dtype=torch.float) #[1, ... , 0]
        action = torch.tensor(action, dtype=torch.long) # [1, 0, 0]
        reward = torch.tensor(reward, dtype=torch.float) # int
        next_state = torch.tensor(next_state, dtype=torch.float) #[True, ... , False]
        target = reward
        target = reward + self.gamma * torch.max(self.model(next_state), dim=1)[0]
        location = [[x] for x in torch.argmax(action, dim=1).numpy()]
        location = torch.tensor(location)
        pred = self.model(state).gather(1, location)#[action]
        pred = pred.squeeze(1)
        loss = self.loss_fn(target, pred)
        loss.backward()
        self.optimizer.step()

    def train_short_memory(self, state, action, reward, next_state, done):
        state = torch.tensor(state, dtype=torch.float)
        next_state = torch.tensor(next_state, dtype=torch.float)
        action = torch.tensor(action, dtype=torch.long)
        reward = torch.tensor(reward, dtype=torch.float)
        target = reward

        if not done:
            target = reward + self.gamma * torch.max(self.model(next_state))
        pred = self.model(state)
        target_f = pred.clone()
        target_f[torch.argmax(action).item()] = target
        loss = self.loss_fn(target_f, pred)
        self.optimizer.zero_grad()
        loss.backward()
        self.optimizer.step()

    def get_dnn_action(self, state):
        final_move = [0, 0, 0]
        state0 = torch.tensor(state, dtype=torch.float)
        prediction = self.model(state0)
        move = torch.argmax(prediction).item()
        final_move[move] += 1
        return final_move

class DQNAgent_play(object):

    def __init__(self, path):
        self.model = Linear_QNet2(11, 256, 3)
        self.model.load_state_dict(torch.load(path))
        self.model.eval()

    def get_state(self, snake):
        state = [
            # Snake location
            (snake.x_change == 20 and snake.y_change == 0 and ((list(map(add, snake.snake_segments[0], [20, 0])) in snake.snake_segments) or snake.snake_segments[0][0] + 20 >= (snake.display_width - 20))) or 
            (snake.x_change == -20 and snake.y_change == 0 and ((list(map(add, snake.snake_segments[0], [-20, 0])) in snake.snake_segments) or snake.snake_segments[0][0] - 20 < 20)) or 
            (snake.x_change == 0 and snake.y_change == -20 and ((list(map(add, snake.snake_segments[0], [0, -20])) in snake.snake_segments) or snake.snake_segments[0][-1] - 20 < 20)) or 
            (snake.x_change == 0 and snake.y_change == 20 and ((list(map(add, snake.snake_segments[0], [0, 20])) in snake.snake_segments) or snake.snake_segments[0][-1] + 20 >= (snake.display_height-20))),

            (snake.x_change == 0 and snake.y_change == -20 and ((list(map(add,snake.snake_segments[0],[20, 0])) in snake.snake_segments) or snake.snake_segments[0][0] + 20 > (snake.display_width-20))) or 
            (snake.x_change == 0 and snake.y_change == 20 and ((list(map(add,snake.snake_segments[0],[-20,0])) in snake.snake_segments) or snake.snake_segments[0][0] - 20 < 20)) or 
            (snake.x_change == -20 and snake.y_change == 0 and ((list(map(add,snake.snake_segments[0],[0,-20])) in snake.snake_segments) or snake.snake_segments[0][-1] - 20 < 20)) or 
            (snake.x_change == 20 and snake.y_change == 0 and ((list(map(add,snake.snake_segments[0],[0,20])) in snake.snake_segments) or snake.snake_segments[0][-1] + 20 >= (snake.display_height-20))),

            (snake.x_change == 0 and snake.y_change == 20 and ((list(map(add,snake.snake_segments[0],[20,0])) in snake.snake_segments) or snake.snake_segments[0][0] + 20 > (snake.display_width-20))) or 
            (snake.x_change == 0 and snake.y_change == -20 and ((list(map(add, snake.snake_segments[0],[-20,0])) in snake.snake_segments) or snake.snake_segments[0][0] - 20 < 20)) or 
            (snake.x_change == 20 and snake.y_change == 0 and ((list(map(add,snake.snake_segments[0],[0,-20])) in snake.snake_segments) or snake.snake_segments[0][-1] - 20 < 20)) or 
            (snake.x_change == -20 and snake.y_change == 0 and ((list(map(add,snake.snake_segments[0],[0,20])) in snake.snake_segments) or snake.snake_segments[0][-1] + 20 >= (snake.display_height-20))),

            # Move direction
            snake.x_change == -20, 
            snake.x_change == 20,  
            snake.y_change == -20,  
            snake.y_change == 20,
            # Raspberry location 
            snake.raspberry_position[0] < snake.snake_position[0],  # food left
            snake.raspberry_position[0] > snake.snake_position[0],  # food right
            snake.raspberry_position[1] < snake.snake_position[1],  # food up
            snake.raspberry_position[1] > snake.snake_position[1]  # food down
            ]
        for i in range(len(state)):
            if state[i]:
                state[i]=1
            else:
                state[i]=0

        return np.asarray(state)

    def get_action(self, state):
        final_move = [0, 0, 0]
        state0 = torch.tensor(state, dtype=torch.float)
        prediction = self.model(state0)
        move = torch.argmax(prediction).item()
        final_move[move] += 1
        return final_move


def get_model_folder_path():
    return "./model"

def get_model_file_name():
    return "best_model.pth"

def get_model_file_path():
    return "%s/%s" % (get_model_folder_path(), get_model_file_name())

def create_model_folder():
    model_folder_path = get_model_folder_path()

    if not os.path.exists(model_folder_path):
        os.makedirs(model_folder_path)
