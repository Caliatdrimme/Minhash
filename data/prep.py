import numpy as np


#open files

f_test = open("test.label", "r")

f_train = open("train.label", "r")


#count number of examples in each of the 4 subgroups i chose

train = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
test = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

#start indeces for document ids for groups 2,9,15,20
start = [0, 0, 0, 0]

line = f_test.readline()

while line:
	for i in range(20):
		if int(line) == i+1:
			test[i] += 1

	line = f_test.readline()

print("Test data")
print(test)


line = f_train.readline()

while line:
	for i in range(20):

		if int(line) == i+1:
			train[i] += 1

	line = f_train.readline()


f_test.close()
f_train.close()
	
print("Train data")
print(train)

print(sum(test))
print(sum(train))



#collect document ids for mini dataset
start[0] = sum(test[0:1])
start[1] = sum(test[0:8])
start[2] = sum(test[0:14])
start[3] = sum(test[0:19])


print(start)


#how long is vocabulary
f_vocab = open("vocabulary.txt", "r")

line = f_vocab.readline()

size_vocab = 0

while line:
	size_vocab += 1
	line = f_vocab.readline()

print(size_vocab)

f_test_data = open("test.data","r")

line = f_test_data.readline()
print(line)
print(type(line))

data = line.split(' ')	
print(data)
print(int(data[2]))


f_vocab.close()


	
	


