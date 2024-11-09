import numpy as np

reps = 500000

# Create two lists of 1000 random numbers between 0 and 2**15
d1 = np.random.rand(reps) * 2**15
d2 = np.random.rand(reps) * 2**15

fp_d1 = np.floor(d1 * 2**10)
fp_d2 = np.floor(d2 * 2**10)

fp_m  = np.mod(fp_d1 * fp_d2 / 2**10, 2**25)
fp_m2 = np.mod(fp_d1 * fp_d2, 2**25)


err = np.abs(m - fp_m)
print(err.mean())
print(err.max())
print(fp_m.max())
