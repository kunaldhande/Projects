import math

class Probe(object):
    def __init__(self, line):
        '''
        ProbePoints Record Format:
            sampleID	is a unique identifier for the set of probe points that were collected from a particular phone.
            dateTime	is the date and time that the probe point was collected.
            sourceCode	is a unique identifier for the data supplier (13 = COMPANY).
            latitude	is the latitude in decimal degrees.
            longitude	is the longitude in decimal degrees.
            altitude	is the altitude in meters.
            speed		is the speed in KPH.
            heading		is the heading in degrees.
        '''
        self.sampleID,      \
        self.dateTime,      \
        self.sourceCode,    \
        self.latitude,      \
        self.longitude,     \
        self.altitude,      \
        self.speed,         \
        self.heading = line.strip().split(',')

        self.direction = ""
        self.linkID = None
        self.distFromRef = None
        self.distFromLink = None
        self.slope = None

    # Get direction of the link
    def getDirection(self, A, B):
        self.direction = "F" if ((math.cos(A) * math.cos(B) + math.sin(A) * math.sin(B)) > 0) else "T"

    def toString(self):
        '''
        Function to convert data into comma seperated string
        '''

        return '{}, {}, {}, {}, {}, {}, {}, {}, {}, {} ,{}, {}\n' \
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
                    self.distFromRef)