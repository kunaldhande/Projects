class Link(object):
	def __init__(self, line):
		self.linkID		  ,\
		self.refNodeID		  ,\
		self.nrefNodeID		  ,\
		self.length			  ,\
		self.functionalClass  ,\
		self.directionOfTravel,\
		self.speedCategory	  ,\
		self.fromRefSpeedLimit,\
		self.toRefSpeedLimit  ,\
		self.fromRefNumLanes  ,\
		self.toRefNumLanes	  ,\
		self.multiDigitized	  ,\
		self.urban			  ,\
		self.timeZone		  ,\
		self.shapeInfo		  ,\
		self.curvatureInfo	  ,\
		self.slopeInfo		  = line.strip().split(',')

		self.ReferenceNodeLat,self.ReferenceNodeLong,_  = self.shapeInfo.split('|')[0].split('/')
		self.ReferenceNode = map(float, (self.ReferenceNodeLat,self.ReferenceNodeLong))
		self.ProbePoints   = []
