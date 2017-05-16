import math
from collections import defaultdict
from link import Link, GeoPoint
from probe import Probe


AVG_EARTH_RADIUS_MLT = math.pi / 180 * 6371000
points = defaultdict(list)
links = defaultdict(list)

probeData = "../data/Partition6467ProbePoints.csv"
linkData = "../data/Partition6467LinkData.csv"
matchedData = "../data/MatchedPointsOutput.csv"

#v Reading link data from Partition6467LinkData
def readLinkData():
    for line in open(linkData).readlines():
        attr = line.strip().split(",")

        # Splitting attribute shapeInfo with '|'
        nodes = attr[14].split("|")

        # Iterate len(nodes)-1 times to group lat-long links
        for i in range(len(nodes)-1):
            temp = Link(attr[0], nodes[i], nodes[i+1])
            links[attr[0]].append(temp)

            points[nodes[i]].append(temp)
            points[nodes[i + 1]].append(temp)

    print "Link loaded"



def matchData():
    target = open(matchedData, "w")
    recentID = None
    candidate = []

    # Iterate Probe data to match it against links
    for cnt, line in enumerate(open(probeData).readlines()):
        probe = Probe(line)

        # Just for status
        if cnt % 500 == 0:
            print(cnt)

        #   Small   - current smallest distance
        #   linkID  - holds the smallest distance
        #   dist_point_refnode  - the distance from the point to ref node of the link
        latlong = GeoPoint(probe.latitude + "/" + probe.longitude)

        # Check if probe sampleID is already matched or not
        if probe.sampleID != recentID:
            recentID = probe.sampleID

            # Iterate through each link to match with current probe
            for key in links.keys():
                for link in links[key]:
                    # Calculate distance of probe lat-long from link
                    distance = link.calculateDistance(latlong)

                    if not probe.distFromRef or distance < probe.distFromRef:
                        probe.distFromRef, probe.linkID = distance, link.id
                        # Calculating distance of current probe from Link
                        probe.distFromLink = links[probe.linkID][0].calculateDistanceFromLink(latlong)

                        # Determine direction
                        probe.getDirection(float(probe.heading), link.radian)

                        # saving candidate with reference point and its respective
                        candidate = [link.ref_point, link.non_ref_point]

        else:
            for candidate_point in candidate:
                for link in points[candidate_point.ID]:
                    distance = link.calculateDistance(latlong)

                    if not probe.distFromRef or distance < probe.distFromRef:
                        probe.distFromRef, probe.linkID = distance, link.id
                        # Calculating distance of current probe from Link
                        probe.distFromLink = links[probe.linkID][0].calculateDistanceFromLink(latlong)

                        # Determine direction
                        probe.getDirection(float(probe.heading), link.radian)



        # distance from the reference node to the map-matched probe point location on the link in decimal meters.
        probe.distFromRef = math.sqrt(probe.distFromRef) * AVG_EARTH_RADIUS_MLT

        # perpendicular distance from the map-matched probe point location on the link to the probe point in decimal meters.
        probe.distFromLink = probe.distFromLink * AVG_EARTH_RADIUS_MLT

        # Writing comma seperated string into file
        target.write(probe.toString())

    target.close()



readLinkData()
matchData()