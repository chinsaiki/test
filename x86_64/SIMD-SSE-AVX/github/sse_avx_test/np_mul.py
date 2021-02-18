import numpy as np
import datetime

def assign_matrix(n,m):
	mat = []
	for i in range(m):
		mat.append(list(range(i+1, n+1+i)))
	return mat
if __name__ == '__main__':
	a = assign_matrix(100,100)
	b = assign_matrix(100,100)
	start_t = datetime.datetime.now()
	c = np.dot(a,b)
	end_t = datetime.datetime.now()
	diff = end_t - start_t
	print "Matrix multiplication complete, time taken : ", diff.microseconds