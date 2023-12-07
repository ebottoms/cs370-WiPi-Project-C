# Folium library "getting started" link: https://python-visualization.github.io/folium/latest/getting_started.html
import folium
from folium.plugins import HeatMap

import csv
import math

MIN_SIGNAL_STRENGTH_DBM = -128
MAX_SIGNAL_STRENGTH_DBM = 0

def main():
    raw_log_data = []
    with open('log7.csv', mode='r') as csv_file:
        log = csv.reader(csv_file)
        for lines in log:
            if(lines[0] == 'nan' or lines[1] == 'nan'):
                continue
            raw_log_data.append([float(lines[0]), float(lines[1]), float(lines[2])])

    # finding min and max signal values
    min_signal_strength = 0
    max_signal_strength = -128
    for point in raw_log_data:
        if(point[2] < min_signal_strength):
            min_signal_strength = point[2]

        if(point[2] > max_signal_strength):
            max_signal_strength = point[2]

    # finding lattitude and longitude bounds
    min_lat = 90
    min_long = 180
    max_lat = -90
    max_long = -180
    for point in raw_log_data:
        if(point[0] < min_lat):
            min_lat = point[0]

        if(point[0] > max_lat):
            max_lat = point[0]

        if(point[1] < min_long):
            min_long = point[1]

        if(point[1] > max_long):
            max_long = point[1]

    # creating map from bounds 
    NWbound = (max_lat, min_long)
    SEbound = (min_lat,max_long)
    center = ((NWbound[0]+SEbound[0])/2,(NWbound[1]+SEbound[1])/2)

    map = folium.Map(location=center, zoom_start=16)

    # creating grid on which to average data points and overlay over the map
    grid_division_size_meters = 10

    data_span_lat = max_lat - min_lat
    data_span_long = max_long - min_long
    num_grid_divisions_lat = math.ceil(data_span_lat / (grid_division_size_meters / 100000))
    num_grid_divisions_long = math.ceil(data_span_long / (grid_division_size_meters / 100000))

    grid = [[ [0,0,0,0] for i in range(num_grid_divisions_long)] for j in range(num_grid_divisions_lat)]
    for i in range(num_grid_divisions_lat):
        for j in range(num_grid_divisions_long):
            grid[i][j] = [round(min_lat+(i/((1/grid_division_size_meters)*100000)),5), round(min_long+(j/((1/grid_division_size_meters)*100000)),5), 0, 0]
    
    for point in raw_log_data:
        lat = point[0]
        long = point[1]
        signal_strength = point[2]
        i = round((lat-min_lat)*(1/grid_division_size_meters)*100000)-1
        j = round((long-min_long)*(1/grid_division_size_meters)*100000)-1
        grid[i][j][2] += normalize(signal_strength)
        grid[i][j][3] += 1

    processed_data = []
    for i in range(len(grid)):
        for j in range(len(grid[0])):
            if(grid[i][j][3] == 0): # prevent division by zero
                grid[i][j][3] = 1
            processed_data.append([grid[i][j][0], grid[i][j][1], grid[i][j][2] / grid[i][j][3]])

    # print info and generate heatmap
    print('Northwestern bound: ' + str(max_lat) + ', ' + str(min_long) + '\nSoutheastern bound: ' + str(min_lat) + ', ' + str(max_long))
    print('\nMaximum signal strength: ' + str(max_signal_strength) + '\nMinimum signal strength: ' + str(min_signal_strength))

    HeatMap(processed_data, max_zoom=2, radius=grid_division_size_meters*2, min_opacity=0, blur=10,overlay=False).add_to(map)
    map.save("heatmap.html")


def normalize(signal_strength):
    return (signal_strength - MIN_SIGNAL_STRENGTH_DBM) / (MAX_SIGNAL_STRENGTH_DBM - MIN_SIGNAL_STRENGTH_DBM)


if __name__ == "__main__":
    main()