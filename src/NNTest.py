import torch
# import torchvision
from torchvision import transforms, datasets
import torch.nn as nn
import torch.nn.functional as NNF
import torch.optim as optim
import matplotlib.pyplot as plt

train = datasets.MNIST("", train=True, download=True, transform=transforms.Compose([transforms.ToTensor()]))
test = datasets.MNIST("", train=False, download=True, transform=transforms.Compose([transforms.ToTensor()]))

trainset = torch.utils.data.DataLoader(train, batch_size=10, shuffle=True)
testset = torch.utils.data.DataLoader(test, batch_size=10, shuffle=True)


class Net(nn.Module):
    def __init__(self):
        super().__init__()               # Hidden layers of our NN. We have 3 hidden layers with 64 nodes in each layer
        self.fc1 = nn.Linear(28 * 28, 64)   # We start with 28*28 (size of our image). 64 is chosen randomly
        self.fc2 = nn.Linear(64, 64)
        self.fc3 = nn.Linear(64, 64)
        self.fc4 = nn.Linear(64, 10)        # We have 10 as output because we have 10 possible outputs (classes) 0-9

    def forward(self, x):
        x = NNF.relu(self.fc1(x))             # We apply the activation function on each hidden layer. Here we use ReLU
        x = NNF.relu(self.fc2(x))
        x = NNF.relu(self.fc3(x))
        x = self.fc4(x)
        return NNF.log_softmax(x, dim=1)


net = Net()
print(net)

X = torch.rand((28, 28))
X = X.view(-1, 28*28)
output = net(X)
print("Test run of feed-forward (no training has been done):")
print(output)

optimizer = optim.Adam(net.parameters(), lr=0.001)  # Define optimizer that will try to minimize loss with learning rate (lr)

# Now we add the part of our program that will train the network:
EPOCHS = 3
for epoch in range(EPOCHS):
    for data in trainset:
        # data is a batch of featuresets and labels
        X, y = data
        net.zero_grad()
        output = net(X.view(-1, 28*28))
        loss = NNF.nll_loss(output, y)
        loss.backward()
        optimizer.step()
    print(loss)

# Now we can calculate the accuracy of the guesses of the NN, we define the number of correct answers and find the
correct = 0
total = 0

with torch.no_grad():
    for data in trainset:
        X, y = data
        output = net(X.view(-1, 28*8))
        for idx, i in enumerate(output):
            if torch.argmax(i) == y[idx]:
                correct += 1
            total += 1

print("Accuracy: ", round(correct/total, 3))

# We display the image of the handwritten digit
plt.imshow(X[2].view(28, 28))
plt.show()

# The network figures out which digit it is seeing and prints it
print(torch.argmax(net(X[2].view(-1, 28*28))[0]))
