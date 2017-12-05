
class Singleton:
    """
    A non-thread-safe helper class to ease implementing singletons.
    This should be used as a decorator -- not a metaclass -- to the
    class that should be a singleton.

    The decorated class can define one `__init__` function that
    takes only the `self` argument. Also, the decorated class cannot be
    inherited from. Other than that, there are no restrictions that apply
    to the decorated class.

    To get the singleton instance, use the `Instance` method. Trying
    to use `__call__` will result in a `TypeError` being raised.

    """

    def __init__(self, decorated):
        self._decorated = decorated

    def Instance(self):
        """
        Returns the singleton instance. Upon its first call, it creates a
        new instance of the decorated class and calls its `__init__` method.
        On all subsequent calls, the already created instance is returned.

        """
        try:
            return self._instance
        except AttributeError:
            self._instance = self._decorated()
            return self._instance

    def __call__(self):
        raise TypeError('Singletons must be accessed through `Instance()`.')

    def __instancecheck__(self, inst):
        return isinstance(inst, self._decorated)

@Singleton
class CSNHelper():
	def __init__(self):
		self.cbw = 10 #Mbps
		self.sbw = 10 #Mbps
		self.cdly = 50 #ms
		self.sdly = 25 #ms

	def SetCloudBandwidth(self,val):
		self.cbw = val

	def SetShadownetBandwidth(self,val):
		self.sbw = val

	def SetCloudDelay(self,val):
		self.cdly = val

	def SetShadownetDelay(self,val):
		self.sdly = val

	def GetCloudBandwidth(self):
		return self.cbw

	def GetShadownetBandwidth(self):
		return self.sbw

	def GetCloudDelay(self):
		return self.cdly

	def GetShadownetDelay(self):
		return self.sdly
        

# This class is a helper to give the nodes IP addresses
@Singleton
class NodeIPAlloc():
	"""IP allocation helper"""
	def __init__(self):
		self.host = 256

	# Allocates a different subnet
	def getHostBlock(self):
		self.host += 1;
		return [self.host-1,1]
	
	#Returns next address in subnet
	def getIPStr(self,host):
		if host[1] & 0xFF == 0xFF:
			host[1] += 2
		s =  str((host[0] & (0xFF << 8)) >> 8) + '.' + str(host[0] & (0xFF)) + '.' +str((host[1] & (0xFF << 8)) >> 8) + '.' + str(host[1] & (0xFF)) + '/16'
		host[1] += 1
		return s

#This function was included because there was a error in creating switches with
#default dpid 
def int2dpid( dpid ):
        try:
            dpid = hex( dpid )[ 2: ]
            dpid = '0' * ( 16 - len( dpid ) ) + dpid
            return dpid
        except IndexError:
            raise Exception( 'Unable to derive default datapath ID - '
                             'please either specify a dpid or use a '
                             'canonical switch name such as s23.' )
