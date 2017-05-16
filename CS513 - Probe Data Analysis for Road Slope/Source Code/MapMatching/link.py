
import math

##All the orders of longitude and latitude are wrong
class GeoPoint():
    def __init__(self, latlong):
        self.ID = latlong
        split_attrs = latlong.split("/")
        self.long, self.lat = map(float, (split_attrs[0], split_attrs[1]))



class Link():
    def __init__(self, ID, start_point, end_point):
        self.id = ID
        self.ref_point, self.non_ref_point = GeoPoint(start_point), GeoPoint(end_point)

        # Vector components
        self.vector_long, self.vector_lat = self.non_ref_point.long - self.ref_point.long, self.non_ref_point.lat - self.ref_point.lat

        # The modular of the vector
        self.length = math.sqrt(self.vector_long ** 2 + self.vector_lat ** 2)

        # Calculating radian of the link
        if self.vector_lat != 0:
            self.radian = math.atan(self.vector_long / self.vector_lat)
        elif self.vector_long > 0:
            self.radian = math.pi / 2
        else:
            self.radian = math.pi * 3 / 2



    def calculateDistance(self, point):
        target_long, target_lat = point.long - self.ref_point.long, point.lat - self.ref_point.lat
        dist_point_refnode = (target_long ** 2) + (target_lat ** 2)

        # Calculating projection
        projection = (target_long * self.vector_long + target_lat * self.vector_lat) / self.length

        # For negative projection, return the square of length from point to ref node
        if projection < 0:
            return dist_point_refnode

        pro_squre = projection ** 2

        # For the projection length which is longer than the link length, return the square of length from point to end node
        if pro_squre > self.length ** 2:
            return (point.long - self.non_ref_point.long) ** 2 + (point.lat - self.non_ref_point.lat) ** 2

        # otherwise return the vertical distance
        return (target_long**2 + target_lat**2) - projection**2


    def calculateDistanceFromLink(self, point):
        target_long, target_lat = point.long - self.ref_point.long, point.lat - self.ref_point.lat
        return math.sqrt(target_long**2 + target_lat**2)