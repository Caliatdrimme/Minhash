import numpy as np


#open files

f_test = open("test.label", "r")

#f_train = open("train.label", "r")


#count number of examples in each of the 4 subgroups 

#train = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
test = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

#start indeces for document ids for groups 2,9,15,20
start = [0, 0, 0, 0]

line = f_test.readline()

while line:
	for i in range(20):
		if int(line) == i+1:
			test[i] += 1

	line = f_test.readline()

#print("Test data")
#print(test)


#line = f_train.readline()

#while line:
#	for i in range(20):

#		if int(line) == i+1:
#			train[i] += 1

#	line = f_train.readline()


f_test.close()
#f_train.close()
	
#print("Train data")
#print(train)

#print(sum(test))
#print(sum(train))



#collect document ids for mini dataset
start[0] = sum(test[0:1])
start[1] = sum(test[0:8])
start[2] = sum(test[0:14])
start[3] = sum(test[0:19])


print(start)


#how long is vocabulary
#f_vocab = open("vocabulary.txt", "r")

#line = f_vocab.readline()

#size_vocab = 0

#while line:
#	size_vocab += 1
#	line = f_vocab.readline()

#print(size_vocab)

#f_vocab.close()

#initialize data matrix
data = np.zeros((200,200))

f_test_data = open("test.data","r")

line = f_test_data.readline()
#print(line)
#print(type(line))

while line:

	dt = line.split(' ')	
	#print(dt)
	id = int(dt[0])

	if ((id >= (start[0]+1)) and (id <= (start[0]+50))):
		#store this line's data
		word = int(dt[1])
		if (word <= 200):
			row = id - start[0] - 1
			data[row,word-1] = 1

	if ((id >= (start[1]+1)) and (id <= (start[1]+50))):
		#store this line's data
		word = int(dt[1])
		if (word <= 200):
			row = id - start[1] + 48
			data[row,word-1] = 1

	if ((id >= (start[2]+1)) and (id <= (start[2]+50))):
		#store this line's data
		word = int(dt[1])
		if (word <= 200):
			row = id - start[2] + 99
			data[row,word-1] = 1

	if ((id >= (start[3]+1)) and (id <= (start[3]+50))):
		#store this line's data
		word = int(dt[1])
		if (word <= 200):
			row = id - start[3] + 149
			data[row,word-1] = 1

	line = f_test_data.readline()

f_test_data.close()

print(data)
print(data[1,:])
#print data matrix into file 
#each row as line
#no symbols

f_out = open("result.txt","w+")

for i in range(200):
	for j in range(200):
		f_out.write(data[i,j])
	f_out.write("/n")

f_out.close()
