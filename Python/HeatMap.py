import pandas as pd
import numpy as np
import folium
from folium.plugins import HeatMap

df = pd.read_csv('log2.csv', names=['Time', 'Latitude', 'Longitude', 'Signal_Strength'])

# sets no signal to -100 we should possibly delete these lines depending on how the mapping works
df.loc[df['Signal_Strength'] == 'Signal level not found', 'Signal_Strength'] = -100

df['Latitude'] = df['Latitude'].astype(float)
df['Longitude'] = df['Longitude'].astype(float)
df['Signal_Strength'] = df['Signal_Strength'].astype(float)

# normalize data for heatmap
df['Signal_Strength'] = (df['Signal_Strength'] + 100) / 50

# prints maxes and mins good data to have
latmin = df['Signal_Strength'].min()
latmax = df['Signal_Strength'].max()
latmin = df['Longitude'].min()
latmax = df['Longitude'].max()
longmin = df['Longitude'].min()
longmax = df['Longitude'].max()
print("Signal Min: " + str(latmin) + " Signal Max: " + str(latmax) )
print("Lat Min: " + str(latmin) + " Lat Max: " + str(latmax) )
print("Long Min: " + str(longmin) + " Long Max: " + str(longmax) )

# Creates Map
map_center = [df['Latitude'].mean(), df['Longitude'].mean()]
wifi_map = folium.Map(location=map_center, max_zoom=16, zoom_start=12)
# Add HeatMap 
heat_data = [[row['Latitude'], row['Longitude'], row['Signal_Strength']] for index, row in df.iterrows()]

HeatMap(heat_data).add_to(wifi_map)

wifi_map.save('wifi_heat_map.html')

