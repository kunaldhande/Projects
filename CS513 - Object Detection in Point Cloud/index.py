
# coding: utf-8
import os
import math
import numpy as np
import pandas as pd
import cv2
import matplotlib.pyplot as plt



intensity_thresold = 60

# Gaussian smoothing
kernel_size = 3

# Canny Edge Detector
low_threshold = 50
high_threshold = 150


'''read data and obtain point cloud and camera center.'''
def preprocessing():
    filename = os.path.join('final_project_data', 'final_project_point_cloud.fuse')
    point_clouds = pd.read_csv(filename, sep=',', header=None, names=['combined'])

    # split the combined column into separate columns
    point_clouds = point_clouds['combined'].str.split(' ', expand=True).astype(np.float64)
    point_clouds.columns = ['latitude', 'longitude', 'altitude', 'intensity']

    filename = os.path.join('final_project_data', 'image', 'camera.config')
    with open(filename, 'r') as f:
        f.readline()
        camera_file = f.readline()

    # Reading camera configuration
    camera_center = [float(camera) for camera in camera_file.split(', ')]
    camera_df = pd.DataFrame([camera_center[:3]], columns=['latitude', 'longitude', 'altitude'])

    camera_df['latitude'] = camera_df['latitude'].apply(convert_degree_to_radian)
    camera_df['longitude'] = camera_df['longitude'].apply(convert_degree_to_radian)
    point_clouds['latitude'] = point_clouds['latitude'].apply(convert_degree_to_radian)
    point_clouds['longitude'] = point_clouds['longitude'].apply(convert_degree_to_radian)

    transform(point_clouds, camera_df)


def transform(point_clouds, camera_df):
    # Sliced point cloud wih intensity greater than intensity_thresold (60)
    point_clouds = point_clouds.loc[point_clouds['intensity'] >= intensity_thresold]
    point_clouds = point_clouds.reset_index(drop=True)

    # Transform LLA to ECEF coordinate
    x0, y0, z0 = lla_to_ecef(camera_df)
    x, y, z = lla_to_ecef(point_clouds)

    ecef = pd.DataFrame(np.column_stack((x, y, z)), columns=['x', 'y', 'z'])
    ecef[['latitude', 'longitude']] = point_clouds[['latitude', 'longitude']]

    # rotate coordinates wrt camera configuration
    R = create_rotation_matrix(camera_df)
    ecef['enu'] = ecef.apply(ecef_to_enu, axis=1, args=(R, x0, y0, z0))

    # create 3d point cloud obj file
    create_obj_file(ecef['enu'], 'output/pointcloud.obj', point_clouds['intensity'])


'''convert degree to radian'''
def convert_degree_to_radian(degree):

    radian = degree * math.pi / 180
    return radian


'''transform LLA to ECEF coordinate'''
def lla_to_ecef(df):
    altitude = df['altitude'].values
    latitude = df['latitude'].values
    longitude = df['longitude'].values
    
    a = 6378137
    b = 6356752.3
    f = (a - b) / a
    e = math.sqrt(f * (2 - f))

    # compute N, distance from surface to z axis
    N = a / np.sqrt(1 - e ** 2 * np.sin(latitude) ** 2)

    # perform transformation to x, y, z axis according to the formula
    x = (altitude + N) * np.cos(latitude) * np.cos(longitude)
    y = (altitude + N) * np.cos(latitude) * np.sin(longitude)
    z = (altitude + (1 - e ** 2) * N) * np.sin(latitude)
    return x, y, z


'''rotate enu to match the camera'''
def enu_to_camera(row, R):
    coord = np.array([row[0], row[1]]).reshape(2, 1)
    result = R.dot(coord).ravel().tolist()
    result.append(row[2])
    return result


'''
    Rotation matrix for each point cloud to transform it to the position with respect to the camera's perspective, instead of with respect to the earth's origin
'''
def create_rotation_matrix(row):
    latitude_radian = row['latitude']
    longitude_radian = row['longitude']
    lat_sin = math.sin(latitude_radian)
    lat_cos = math.cos(latitude_radian)
    lon_sin = math.sin(longitude_radian)
    lon_cos = math.cos(longitude_radian)
    R = np.array([
        [-lon_sin, lon_cos, 0],
        [-lon_cos * lat_sin, -lat_sin * lon_sin, lat_cos],
        [lat_cos * lon_cos, lat_cos * lon_sin, lat_sin]
    ])
    return R


'''coordinate transformation for each point cloud'''
def ecef_to_enu(row, R, x0, y0, z0):
    coord_diff = np.array([row['x'] - x0, row['y'] - y0, row['z'] - z0]).reshape(3, 1)
    result = R.dot(coord_diff).ravel().tolist()
    return result



''' Create objet file '''
def create_obj_file(df, filename, intensity_values):
    obj_df = pd.DataFrame()
    obj_df[['x', 'y', 'z']] = df.apply(pd.Series)
    obj_df['intensity'] = intensity_values
    obj_df['v'] = 'v'
    obj_df = obj_df[['v', 'x', 'y', 'z', 'intensity']]
    obj_df.to_csv(filename, sep=' ', index=False, header=False)


''' Function to detect lanes '''
def detect_lanes(image):
    # Read in and grayscale the image
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    cv2.imwrite('output/gray.jpg', gray)

    # Apply Gaussian smoothing
    blur_gray = cv2.GaussianBlur(gray, (kernel_size, kernel_size), 0)
    cv2.imwrite('output/blur_gray.jpg', blur_gray)

    # Apply Canny for edge detection
    edges = cv2.Canny(blur_gray,50, 150,apertureSize = 3)
    cv2.imwrite('output/edges.jpg', edges)


    # Apply Hough Transform
    minLineLength = 0
    maxLineGap = 0
    lines = cv2.HoughLinesP(edges, 1, np.pi/180, 100, minLineLength,maxLineGap)
    for arr in lines:
        x1,y1,x2,y2 = arr[0]
        cv2.line(image,(x1,y1),(x2,y2),(0,255,0), 2)

    cv2.imwrite('output/output.jpg',image)
    
    return edges, image


def show_image(image):
    fig = plt.figure(figsize=(50, 50))
    vax = fig.add_subplot(121)
    plt.axis("off")
    plt.imshow(image)





def main():
    ''' Read data '''
    print("Reading final_project_point_cloud.fuse")
    preprocessing()

    print("Generating binary image from the cloud points")
    # Read data from pointclod.obj file
    point_clouds = pd.read_csv('output/pointcloud.obj', sep=',', header=None, names=['combined'])

    point_clouds = point_clouds['combined'].str.split(' ', expand=True)
    point_clouds.columns = ['v','latitude', 'longitude', 'altitude', 'intensity']

    # Genearting image from point cloud
    fig = plt.figure(figsize=(30, 30))
    plt.plot(point_clouds['latitude'].tolist(), point_clouds['longitude'].tolist(), '.', color='k')
    plt.axis('off')
    fig.savefig("output/image.jpg", bbox_inches='tight')

    print("Detecting lane marking")
    # Code to detect lane marking
    edges, image = detect_lanes(cv2.imread('output/image.jpg'))
    print("Out put mage saved: output/output.jpg")
    # show_image(image)



main()