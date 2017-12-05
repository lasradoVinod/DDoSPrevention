
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.node import Node
from mininet.log import setLogLevel, info
from mininet.cli import CLI

class LinuxRouter( Node ):
    "A Node with IP forwarding enabled."

def config( self, **params ):
    super( LinuxRouter, self).config( **params )
    # Enable forwarding on the router
    self.cmd( 'sysctl net.ipv4.ip_forward=1' )

def terminate( self ):
    self.cmd( 'sysctl net.ipv4.ip_forward=0' )
    super( LinuxRouter, self ).terminate()


class NetworkTopo( Topo ):
    "A LinuxRouter connecting three IP subnets"

def build( self, **_opts ):

    robIP = '10.1.1.14/24'  # IP address for r0-eth1
    richIP = '10.4.4.46/24'
    robert = self.addNode( 'r0', cls=LinuxRouter, ip=robIP, defaultRoute='via 10.4.4.14' )
    richard = self.addNode( 'r1', cls=LinuxRouter, ip=richIP, defaultRoute='via 10.6.6.46' )

    alice = self.addHost( 'h1', ip='10.1.1.17/24', defaultRoute='via 10.1.1.14' )
    bob = self.addHost( 'h2', ip='10.4.4.48/24', defaultRoute='via 10.4.4.14' )

    self.addLink( alice, robert, intfName2='r0-eth1', params2={ 'ip' : '10.1.1.14/24' } )
    self.addLink( bob, robert, intfName2='r1-eth1', params2={ 'ip' : '10.4.4.14/24' } )
    self.addLink( bob, richard, intfName2='r0-eth2', params2={ 'ip' : '10.4.4.46/24' } )



def run():
    "Test linux router"
    topo = NetworkTopo()
    net = Mininet( topo=topo )  # controller is used by s1-s3
    net.start()
    info( '*** Routing Table on Router:\n' )
    print net[ 'r0' ].cmd( 'route' )
    CLI( net )
    net.stop()

if __name__ == '__main__':
    setLogLevel( 'info' )
    run()
