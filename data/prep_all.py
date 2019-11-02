import numpy as np


#open files

f_test_label = open("test.label", "r")

line = f_test_label.readline()
cnt = 0

while line:
	cnt += 1
	line = f_test.readline()
  
print(cnt)

f_test_label.close()

#initialize data matrix
data = np.zeros((cnt,61188))

f_test_data = open("test.data","r")

line = f_test_data.readline()

while line:

	dt = line.split(' ')
	#store this line's data
	word = int(dt[1])
	row = int(dt[0])
	data[row,word-1] = 1

	line = f_test_data.readline()

f_test_data.close()

data = data.astype(int)

f_out = open("data-valid-all.txt","w+")

for i in range(cnt):
	for j in range(61188):
		element = str(data[i,j])
		f_out.write(element)
	f_out.write('\n')

f_out.close()
