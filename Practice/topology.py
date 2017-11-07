#WARNING !!! Not all comments in this file might make sense to you. It might not make sense to me too!!
#####################################################################################################
# For API documentation refer 
#http://mininet.org/api/classmininet_1_1link_1_1Link.html#ade37d7cffeff73830bec0129ac754622
##################################INCLUDE SECTION ###################################################
#Our Topology extends the base Topo class
from mininet.topo import Topo
# Create a mininet instance
from mininet.net import Mininet
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

class CloudAndShadowNet(Topo):
	"""Tree topology with edge cloud and things"""
	def __init__(self,numThings=2,numEdges=2,numClouds = 2):
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
		self.shadowNet = self.addHost("shadownet",ip=ARIN.getIPStr(self.IPInfra))

		display ("Connecting shadownet to mainSwitch")
		self.addLink(self.shadowNet,mainSwitch,delay=cshInst.GetShadownetDelay(),bw=cshInst.GetShadownetBandwidth())

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
			self.edge.append([self.addHost("edge" + str(i) ,ip = ARIN.getIPStr(self.IPInfra)),[]])
			#Connecting edge to master switch which will connect it to the cloud and shadow net
			#If in the future you want to customise this delay and bandwidth the add functions in
			#CSNHelper 
			self.addLink(self.edge[i][0],mainSwitch,delay=cshInst.GetCloudDelay(),bw=cshInst.GetCloudBandwidth())

		#Here we will use the ip block created before for the things
		display("Creating things for edges")
		for i in range (numEdges):
			for j in range(numThings):
				self.edge[i][1].append(self.addHost('thing'+str(i)+str(j),ip=ARIN.getIPStr(self.IPInfra)))
				self.addLink(self.edge[i][1][j],mainSwitch,bw=1,delay=100)
		display("Done with creating")

	def getEdges(self):
		return self.edge
############################################CONSTANTS#########################################################

DEBUG = True
def display(a):
	if DEBUG == True:
		print a

topo = CloudAndShadowNet()
net = Mininet(topo,controller=OVSController,link=TCLink)

net.start()
dumpNodeConnections(net.hosts)
net.pingAll()
net.stop()

