#WARNING !!! Not all comments in this file might make sense to you. It might not make sense to me too!!
#####################################################################################################
# For API documentation refer 
#http://mininet.org/api/classmininet_1_1link_1_1Link.html#ade37d7cffeff73830bec0129ac754622
##################################INCLUDE SECTION ###################################################
#Our Topology extends the base Topo class
from mininet.topo import Topo
# Create a mininet instance
from mininet.net import Mininet
import time
#This is the default controller used.
#We have connected it to a remote ODL controller
#TODO connect it to this code
from mininet.node import OVSController
#You cannot set link bw and delay without using this type of link 
#for command line sudo mn --custom custom.py --topo customtopo --link tc
#For more details 
#https://stackoverflow.com/questions/24130398/typeerror-init-got-an-unexpected-keyword-argument-delay#25298989
#Also using the default mn from linux you mighe get RETLINK Error: on output screen it is not important
#To get rid of this refer 
#https://github.com/mininet/mininet/issues/629
from mininet.link import TCLink
#Debbugging function
from mininet.util import dumpNodeConnections
from Utils import int2dpid
from Utils import NodeIPAlloc
from Utils import CSNHelper

CLOUD_PORT = 10000
SNPORT	= 12000
EDGE_PORT =	14000

class CloudAndShadowNet(Topo):
	"""Tree topology with edge cloud and things"""
	def __init__(self,numThings=2,numEdges=2,numClouds = 1):
		ARIN = NodeIPAlloc.Instance()
		Topo.__init__(self)
		cshInst = CSNHelper.Instance()
		self.numThings = numThings
		self.numEdges = numEdges
		self.numClouds = numClouds

		#Putting all Infra things on one IP subnet for simplicity 
		self.IPInfra = ARIN.getHostBlock()

		#TODO Data rate adjustments

		# There was an error with the dpid therefore I had to generate my own
		count = 0;
		# This is the switch that connects the edges to the cloud and the shadowNet
		display ("Adding mainSwitch")
		mainSwitch = self.addSwitch('ms',dpid=int2dpid(count))
		count = count + 1
		# There can be multiple clouds and 1 only 1 can be under DDoS attack so provision for multiple cloud
		self.cloud = []

		# There is always only one shadowNet as per our current assumption since layering will add to delay 
		# and we need a fast path. This may change but as of now this is what it is
		display ("Creating shadowNet server")
		sip=ARIN.getIPStr(self.IPInfra)
		self.shadowNet = [self.addHost("shadownet",ip=sip), sip]

		display ("Connecting shadownet to mainSwitch")
		self.addLink(self.shadowNet[0],mainSwitch,delay=cshInst.GetShadownetDelay(),bw=cshInst.GetShadownetBandwidth())

		# Creating clouds and connecting it to the mainSwitch
		for i in range(numClouds):
			display("Creating cloud"+str(i))
			ipAddr = ARIN.getIPStr(self.IPInfra)
			self.cloud.append([self.addHost("cloud" + str(i),ip=ipAddr), ipAddr])
			self.addLink(self.cloud[i][0],mainSwitch,delay=cshInst.GetCloudDelay(),bw=cshInst.GetCloudBandwidth())

		# This is a master data Structure for all edges switches and things
		# The edges will have two interfaces, one for communication with the things 
		# second for communication with the infra
		self.edge = []
		for i in range(numEdges):
			#saving the ipblock for things creation
			eip = ARIN.getIPStr(self.IPInfra)
			self.edge.append([self.addHost("edge" + str(i) ,ip = eip),[], eip])
			#Connecting edge to master switch which will connect it to the cloud and shadow net
			#If in the future you want to customise this delay and bandwidth the add functions in
			#CSNHelper 
			self.addLink(self.edge[i][0],mainSwitch,delay=cshInst.GetCloudDelay(),bw=cshInst.GetCloudBandwidth())

		#Here we will use the ip block created before for the things
		display("Creating things for edges")
		for i in range (numEdges):
			for j in range(numThings):
				tip = ARIN.getIPStr(self.IPInfra)
				self.edge[i][1].append([self.addHost('thing'+str(i)+str(j),ip=tip),tip])
				self.addLink(self.edge[i][1][j][0],mainSwitch,bw=1,delay=100)
		display("Done with creating")

	def getEdges(self):
		return self.edge

	def getClouds(self):
		return self.cloud

	def getShadowNet(self):
		return self.shadowNet

############################################CONSTANTS#########################################################

DEBUG = True
def display(a):
	if DEBUG == True:
		print a

def getIp(ip):
	return ip.split('/')[0]

mainNumClouds = 1
mainNumEdge = 2
mainNumThings = 2

cshInst = CSNHelper.Instance()

cshInst.SetCloudDelay(200)
cshInst.SetShadownetDelay(25)

#Creation of topology
topo = CloudAndShadowNet(mainNumThings,mainNumEdge,mainNumClouds)

net = Mininet(topo,controller=OVSController,link=TCLink)

net.start()
dumpNodeConnections(net.hosts)
mainEdges = topo.getEdges()
mainClouds = topo.getClouds()

packetSize  = 100
for i in range(len(mainClouds)):
	node = net['cloud' + str(i)]
	command = ("xterm -hold -e ./cloud "+ getIp(mainClouds[i][1]) + " " + str(CLOUD_PORT) + " " \
	+ str(packetSize * 60 * len(mainEdges) * len(mainEdges[0][1]))  +" &")
	print command 
	node.cmd(command)

mainShadownet = topo.getShadowNet()
node = net['shadownet']
node.cmd("xterm -hold -e ./shadownet "+ getIp(mainShadownet[1]) + " " + str(SNPORT) + " ShadownetLog.txt &")

for i in range(len(mainEdges)):
	node = net['edge' + str(i)]
	command = "xterm -hold -e ./edge " + getIp(mainEdges[i][2]) +  " " + str(EDGE_PORT) + " " + getIp(mainClouds[0][1]) + " " + str(CLOUD_PORT) + " " + getIp(mainShadownet[1]) + " " + str(SNPORT) + " Pune" + " &"
	print command
	node.cmd(command)
	time.sleep(1)

for i in range(len(mainEdges)):
	for j in range(len(mainEdges[i][1])):
		node = net['thing' + str(i) + str(j)]
		command = "xterm -hold -e ./randomTest " + getIp(mainEdges[i][2]) +  " " + str(EDGE_PORT) + " " + str(packetSize)+ " 1 2000 1 &" 
		print command
		time.sleep(0.2)
		node.cmd(command)
while(True):
     continue
