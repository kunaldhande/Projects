from sys import argv
import math
import urllib
from PIL import Image
import io

EARTH_RADIUS = 6378137
MAX_LAT, MIN_LAT = 85.05112878, -85.05112878
MAX_LONG, MIN_LONG = 180, -180

MAX_RESOLUTION = 23
TILE_SIZE = 2 ** 8  # The origial tile image is 256 * 256, for rendering images
IMG_SIZE = 2 ** 14


def encode(lat, long):
    return lat_long_to_quadkey(lat, long, 1)


def getURL(quad_key):
    return "http://h0.ortho.tiles.virtualearth.net/tiles/h%s.jpeg?g=131&key=AhHPXuVNzco40xVgSzCMz6TmC589Zlp-F2_Wj065PQW1Cr0SyOLC_EoyMEnJZ6Lx" % (quad_key)


# If the input exceeds the min/max value of lat/long, clip it into min/max value
def clip(input, min_val, max_val):
    """
    :param inputVal: 
    :param minVal: 
    :param maxVal: 
    :return: min/max value
    """
    return min(max(input, min_val), max_val)

# Converts pixel XY coordinates into tile XY coordinates of the tile.
def pixel_xy_to_tile_xy(pixel_x, pixel_y):
    """
    :param pixleX: 
    :param pixelY: 
    :return: tileXY co-ordinates
    """
    return int(pixel_x / TILE_SIZE), int(pixel_y / TILE_SIZE)


# Converts a point from latitude/longitude WGS-84 coordinates (in degrees) into pixel XY coordinates at
# a specified level of detail.
def lat_long_to_pixel(lat, long, level):
    """
    :param lat: 
    :param long: 
    :param level: 
    :return: lat, long to pixel value
    """
    lat, long = clip(float(lat), MIN_LAT, MAX_LAT), clip(float(long), MIN_LONG, MAX_LONG)

    sin_lat = math.sin(lat * math.pi / 180)

    x, y = (long + 180) / 360, 0.5 - math.log((1 + sin_lat) / (1 - sin_lat))/(4 * math.pi)

    mapSize = 256 * (math.pow(2, level))
    return round(clip(x * mapSize + 0.5, 0, mapSize - 1)), int(clip(y * mapSize + 0.5, 0, mapSize - 1))


# Converts tile XY coordinates into a QuadKey at a specified level of detail.
def tile_to_qaudkey(tileX, tileY, level):
    """
    :param tileX: 
    :param tileY: 
    :param level: 
    :return: quad key
    """
    quad_key = str()
    for i in range(level, 0, -1):
        d = '0'
        m = (1<<(i-1)) & 0xffffffff
        if (tileX & m) != 0:   d = chr(ord(d) + 1)

        if (tileY & m) != 0:
            d = chr(ord(d) + 1)
            d = chr(ord(d) + 1)
        quad_key += d
    return quad_key

# Convert latitude/longitude into tileXY coordinates of tile containing the specified point
def lat_long_to_tile(lat, long, level):
    """
    :param lat: 
    :param long: 
    :param level: 
    :return: lat long to tile XY cordinates
    """
    pixelX,pixelY = lat_long_to_pixel(lat, long, level)
    return pixel_xy_to_tile_xy(pixelX, pixelY)


# Calculating quad key from lat-long
def lat_long_to_quadkey(lat, long, level):
    """
    :param lat: 
    :param long: 
    :param level: 
    :return: quad key
    """
    lat = clip(lat, MIN_LAT, MAX_LAT)
    long = clip(long, MIN_LONG, MAX_LONG)

    tileX, tileY = lat_long_to_tile(lat, long, level)
    return tile_to_qaudkey(tileX, tileY, level)

# Downlaod image
def download_image(x, y, level):
    """
    :param x: 
    :param y: 
    :param level: 
    :return: downloaded image
    """
    img = urllib.urlopen(getURL(tile_to_qaudkey(x, y, level)))
    return Image.open(io.BytesIO(img.read()))


# Calculate no of tile are gonna be there, given current detail level
def calculate_no_of_tiles(lat1, lon1, lat2, lon2, currLv):
    x1, y1 = lat_long_to_tile(lat1, lon1, currLv)
    x2, y2 = lat_long_to_tile(lat2, lon2, currLv)

    if x1 > x2: x1, x2 = x2, x1
    if y1 > y2: y1, y2 = y2, y1

    return (x2 - x1 + 1) * (y2 - y1 + 1)


def isImageNone(img):
    return img != Image.open('null.jpeg')


# Calculate the most common level shared by rectangle, we will start our work from here.
def calculate_common_level(lat1, lon1, lat2, lon2):
    for level in xrange(MAX_RESOLUTION, -1, -1):
        x1, y1 = lat_long_to_tile(lat1, lon1, level)
        x2, y2 = lat_long_to_tile(lat2, lon2, level)
        if abs(y2 - y1) <= 1 and abs(x1 - x2) <= 1: return level


def main():
    try:
        lat1, lon1, lat2, lon2 = [float(argv[i]) for i in xrange(1, 5)]
        # lat1, lon1, lat2, lon2= 39.646058, -123.285856, 39.637485, -123.267924
        print 'Point 1: %s, %s\nPoint 2: %s, %s' % (lat1, lon1, lat2, lon2)

        image_file = "image.jpeg"

        print 'Checking common level -'
        level = calculate_common_level(lat1, lon1, lat2, lon2)
        print 'Level: %d' % level

        image = Image.new('RGB', (IMG_SIZE, IMG_SIZE))

        a0, b0 = map(min, (zip(lat_long_to_tile(lat1, lon1, level), lat_long_to_tile(lat2, lon2, level))))
        tile_size = IMG_SIZE / 2
        a1, b1 = 0, 0
        a2, b2 = 0, 0

        while tile_size >= TILE_SIZE and level <= MAX_RESOLUTION:
            print "Level: %d\tNo of tiles: %d" % (level, calculate_no_of_tiles(lat1, lon1, lat2, lon2, level))

            a1, b1 = lat_long_to_tile(lat1, lon1, level)
            a2, b2 = lat_long_to_tile(lat2, lon2, level)

            # swaping the index, if the index is not proper format
            if a1 > a2:
                a1, a2 = a2, a1
            if b1 > b2:
                b1, b2 = b2, b1

            for X in range(a1, a2 + 1):
                for Y in range(b1, b2 + 1):
                    try:
                        img = download_image(X, Y, level)
                    except:
                        # Handling some exceptions
                        continue

                    if isImageNone(img):
                        img = img.resize((tile_size, tile_size))
                        pixelX, pixelY = (X - a0) * tile_size, (Y - b0) * tile_size

                        # Paste the downloaded image at correct place
                        image.paste(img, (pixelX, pixelY, pixelX + tile_size, pixelY + tile_size))

            a0, b0 = a0 * 2, b0 * 2
            level += 1
            tile_size /= 2

        # Croping the image to a proper place
        a0, b0 = a0 / 2, b0 / 2
        tile_size *= 2

        pixel_a1, pixel_b1 = (a1 - a0) * tile_size, (b1 - b0) * tile_size
        pixel_a2, pixel_b2 = (a2 - a0 + 1) * tile_size, (b2 - b0 + 1) * tile_size

        image = image.crop((pixel_a1, pixel_b1, pixel_a2, pixel_b2))
        image.show()
        print 'Image saved : %s' % image_file
        image.save(image_file)

    except:
        print 'Required input:\n~ python index.py <lat1> <lon1> <lat2> <lon2>\nE.g.\n~ python index.py 41.884423 -87.627599 41.880142 -87.617342'



if __name__ == '__main__':
    main()