class NodeIPAlloc():
	"""docstring for ClassName"""
	def __init__(self):
		self.host = 1
		self.net = 1

	def getHostBlock(self):
		sethost += 1;
		return self.host-1
	
	def getNextIP(self,host):
		if ((self.net & 0xFF) == 0xFF):
			self.net += 1
		s =  str((self.host & (0xFF << 8)) >> 8) + '.' + str(self.host & (0xFF)) + \
				'.' + str((self.net & (0xFF << 8)) >> 8) + '.' + str(self.net & (0xFF))
		self.net += 1
		return s

