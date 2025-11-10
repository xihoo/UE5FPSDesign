The purpose of this component is to allow for easy functionality of a range finder, whether its handheld or weapon mounted.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/908369fb-e616-4793-ae7b-bcf31daab341)

# Details Panel
## Initialize
### Range Finder Mesh Name
This is the name of the mesh to be used for performing the trace from the range finder.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/df9da103-07de-4fa9-b70e-57604ca04ae4)
### Range Finder Laser Socket Name
This is the name of the socket on the Range Finder Mesh Name used for tracing.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/cea64815-6f75-48bb-b19f-396b0f6073c3)
### Text Render Component Name
This is the name of the text render component used for the range finder to display the range.
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/6253b086-a222-4ba8-8182-f16986641350)

## Settings
![image](https://github.com/SneakyKittyGameDev/SKGSFExample/assets/29245429/c91f8ad7-0669-4059-a1d2-410aa2952044)
### Min Trace Distance
This is the minimum distance that the range finder will read. If the hit is less than this distance away, 0 is returned.
### Max Trace Distance
This is the maximum distance that the range finder will trace. Distances hit further than this will return 0.
### Measurement Symbols
#### Meter/Yard/Inch/Centimeter Symbol
These are the corresponding symbols to be used in the display such as 150M/150m if using Meters.
### Use In Yard Meter
If true, it will use Yards/Meters for measurement, if false it will use Inch/Centimeter.
### Use Metric
If true, it will use Meter/Centimeter for measurement based on the above setting, if false it will use Yard/Inch based on the above setting.
### Auto Range
If true, the range finder will automatically fetch the range at what its looking at based on the given interval of Auto Range Interval.
### Auto Range Interval
This is the rate at which the range finder will fetch the range. 1.0 = fetch once every 1 second.


# Functions
## Range Category
### GetRange
This function will range whatever the range finder is looking at and apply and return the result.
### GetRangeAsString
This function will range whatever the range finder is looking at and apply and return the result as a string.

## Getters
### GetRangeFinderMesh
This function will return the [mesh](#range-finder-mesh-name) used for the range finder systems.
### GetTextRenderComponent
This function will return the [text render component](#text-render-component-name) used for the range finder systems.

# Events
### OnRanged
This event fires off each time the range finder runs.