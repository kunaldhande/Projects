class MatchedProbe(object):
	def __init__(self, line):
		self.sampleID	, \
        self.dateTime	, \
        self.sourceCode	, \
        self.latitude	, \
        self.longitude	, \
        self.altitude	, \
        self.speed		, \
        self.heading	, \
        self.linkID		, \
        self.direction	, \
        self.distFromRef, \
        self.distFromLink = line.split(',')

		self.elevation = None
		self.slope = None


	def toString(self):
		'''
        Function to convert data into comma seperated string
        '''
		return '{}, {}, {}, {}, {}, {}, {}, {}, {}, {} ,{}, {}, {}\n' \
			.format(self.sampleID,
					self.dateTime,
					self.sourceCode,
					self.latitude,
					self.longitude,
					self.altitude,
					self.speed,
					self.heading,
					self.linkID,
					self.direction,
					self.distFromRef,
					self.distFromRef,
                    self.slope)