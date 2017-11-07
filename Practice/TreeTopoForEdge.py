from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import OVSController
from mininet.util import dumpNodeConnections

class NodeIPAlloc():
	"""docstring for ClassName"""
	def __init__(self):
		self.host = 256

	def getHostBlock(self):
		self.host += 1;
		return [self.host-1,1]
	
	def getIPStr(self,host):
		if host[1] & 0xFF == 0xFF:
			host[1] += 2
		s =  str((host[0] & (0xFF << 8)) >> 8) + '.' + str(host[0] & (0xFF)) + '.' +str((host[1] & (0xFF << 8)) >> 8) + '.' + str(host[1] & (0xFF)) + '/16'
		print s
		host[1] += 1
		return s

DEBUG = True

ARIN = NodeIPAlloc();

def display(a):
	if DEBUG == True:
		print a

def int2dpid( dpid ):
        try:
            dpid = hex( dpid )[ 2: ]
            dpid = '0' * ( 16 - len( dpid ) ) + dpid
            return dpid
        except IndexError:
            raise Exception( 'Unable to derive default datapath ID - '
                             'please either specify a dpid or use a '
                             'canonical switch name such as s23.' )

class ThingsEdge(Topo):
	"Tree topology with edge cloud and things"
	def __init__(self,n=2):
		Topo.__init__(self)
		self.things = []
		for i in range(n):
			self.things.append(self.addHost('h'+str(i)))
		self.switch = self.addSwitch('s1')
		for i in range (len(self.things)):
			self.addLink(self.switch,self.things[i])

	def getEdgeNode(self):
		return self.switch

class CloudAndShadowNet(Topo):
	"""Tree topology with edge cloud and things"""
	def __init__(self,numThings=2,numEdges=2,numClouds = 1,RTTRatio = 2):
		Topo.__init__(self)
		#TODO Data rate adjustments
		count = 0;
		# This is the switch that connects the edges to the cloud and the shadowNet
		display ("Adding mainSwitch")
		mainSwitch = self.addSwitch('ms',dpid=int2dpid(count))
		count = count + 1
		# There can be multiple clouds and I only I can be under DDoS attack so provision for multiple cloud
		self.cloud = []

		# There is always only one shadowNet as per our current assumption since layering will add to delay 
		# and we need a fast path. This may change but as of now this is what it is
		display ("Creating shadowNet server")
		self.shadowNet = self.addHost("shadownet")

		display ("Connecting shadownet to mainSwitch")
		self.addLink(self.shadowNet,mainSwitch)

		# Creating clouds and connecting it to the mainSwitch
		for i in range(numClouds):
			display("Creating cloud"+str(i))
			self.cloud.append(self.addHost("cloud" + str(i)))
			self.addLink(self.cloud[i],mainSwitch)

		# This is a master data Structure for all edges switches and things
		self.edge = []
		for i in range(numEdges):
			display("Creating edgeSwitch"+str(i))
			p = self.addSwitch('es'+str(i),dpid=int2dpid(count))
			count = count + 1
			self.edge.append([self.addHost("edge" + str(i)),p,[]])
			#Connecting edge to master switch which will connect it to the cloud and shadow net
			self.addLink(self.edge[i][0],mainSwitch)
			#Connecting edge to switch which will in turn connect it to the things
			self.addLink(p,self.edge[i][0])
			
		display("Creating things for edges")
		for i in range (numEdges):
			for j in range(numThings):
				self.edge[i][2].append(self.addHost('thing'+str(i)+str(j)))
				self.addLink(self.edge[i][1],self.edge[i][2][j])
		display("Done with creating")

class TwoNode(Topo):
 	"""Accepts a list of thing and edge topos and connects them to one cloud and shadownet server"""
 	def __init__(self):
		Topo.__init__(self)
		self.iphostNum = ARIN.getHostBlock()
		self.iphostNum1 = ARIN.getHostBlock()
		self.h3 = self.addHost('h3',ip=ARIN.getIPStr(self.iphostNum1))
		self.h1 = self.addHost('h1',ip=ARIN.getIPStr(self.iphostNum))
		self.h2 = self.addHost('h2',ip=ARIN.getIPStr(self.iphostNum)) 
		self.s1 = self.addSwitch('s1')

		self.addLink(self.h1,self.s1)
		self.addLink(self.h2,self.s1)
		self.addLink(self.h2,self.h3)


topos = { 'mytopo': ( lambda: TwoNode() ) }

#topo = TwoNode()
#net = Mininet(topo,controller=OVSController)

#net.start()
#dumpNodeConnections(net.hosts)
#net.pingAll()
#net.stop()
