import torch
# import torchvision
from torchvision import transforms, datasets
import torch.nn as nn
import torch.nn.functional as NNF

train = datasets.MNIST("", train=True, download=True, transform=transforms.Compose([transforms.ToTensor()]))
test = datasets.MNIST("", train=False, download=True, transform=transforms.Compose([transforms.ToTensor()]))

trainset = torch.utils.data.DataLoader(train, batch_size=10, shuffle=True)
testset = torch.utils.data.DataLoader(test, batch_size=10, shuffle=True)


class Net(nn.Module):
    def __init__(self):
        super().__init__()               # Hidden layers of our NN. We have 3 hidden layers with 64 nodes in each layer
        self.fc1 = nn.Linear(28 * 28, 64)   # We start with 28*28 (size of our image. 64 is chosen randomly
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
print(output)
