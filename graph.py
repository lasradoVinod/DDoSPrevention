A = [2, 4, 6, 8, 10, 12]	
B = [0.62, 0.70, 0.49, 0.82,0.91,1.46]

C = [2, 4, 6, 8, 10, 12]
D = [0.624224, 0.8943045, 0.6439005, 0.8840924, 0.945934, 1.140227]

#A = [10,25,50,75,100,125]
#B = [455.0769230769,442.5625,155.9230769231,121.6153846154,80.2682926829,0.0625]
import matplotlib.pyplot as plt

plt.plot(A,B,'b',C,D,'r--')

#plt.xlabel('Cloud Latency/ShadowNet Latency')
#plt.ylabel('Difference in detection time')
#plt.xlabel('Threshold')
#plt.ylabel('Number of packets at shodownet per second')
plt.show()
