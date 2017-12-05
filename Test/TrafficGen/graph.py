import matplotlib.pyplot as plt
import numpy as np
from astropy.stats import median_absolute_deviation
import statistics

pktNum, pktSize, var  = np.genfromtxt('log_random.csv', unpack = True)
plt.plot(pktNum,var,'b.')
print statistics.mean(var)
print median_absolute_deviation(var)
plt.show()
