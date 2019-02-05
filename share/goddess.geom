# ALL THE DIMENSIONS ARE IN MILLIMETERS!
--------------------------------------------------------------------------------------------------

# Goddess Mount infos

Barrel Radius = 98.43
Barrel Length = 228.6

# Angle of the target ladder with the beam line in degrees
Target Ladder Angle = 60

--------------------------------------------------------------------------------------------------

# The spacing between 2 detectors from a telescope (e.g. dE and E1 layer from QQQ5 of sector DA)

Barrel Layer Spacing = 4.8
Endcap Layer Spacing = 3.3

--------------------------------------------------------------------------------------------------

# Offsets are given as (X, Y, Z) with Z the beam direction

# For the SuperX3, the reference offset is for sector D0
# X: center of detector vs. center of the chamber
# Y: distance detector to barrel mount outer radius
# Z: start of active area compared to center of chamber)
SuperX3 Offset = (0, 11.55, 12.66)

# For the QQQ5, the reference offset is for sector DA
# X: tip of the QQQ5 arc circle vs. center of the chamber
# Y: first strip lower edge to center of the chamber
# Z: distance detector to barrel mount end )
QQQ5 Offset = (0, 25.2, -16.5)

# Offset given compared to the center of the chamber
Beam Spot Offset = (0, 0, 0)

--------------------------------------------------------------------------------------------------

# QQQ5 infos
# "Delta Pitch" is the difference in width between 2 consecutive strips
# "QQQ5 Ref Phi" is the Phi angle for the reference sector, the QQQ5s Downstream A (typically 90 degrees)
# "QQQ5 Delta Phi" is the Phi difference between 2 consecutive endcap sectors, e.g. sector Upstream B and sector Upstream C (typically 90 degrees)
# "QQQ5 Ref Zrot" is the rotation of the reference detector (QQQ5 DA) around the beam axis
# "QQQ5 Delta Zrot" is the difference between the rotation of 2 consecutive detectors around the beam axis

QQQ5 First Strip Width = 2.55
QQQ5 Delta Pitch = 0.05

QQQ5 Ref Phi = Pi/2
QQQ5 Delta Phi = Pi/2

QQQ5 Ref Zrot = 0
QQQ5 Delta Zrot = Pi/2

--------------------------------------------------------------------------------------------------

# Super X3 infos
# "SuperX3 Ref Phi" is the Phi angle for the reference sector, the SuperX3 Downstream 0 (typically 90 degrees)
# "SuperX3 Delta Phi" is the Phi difference between 2 consecutive barrel sectors, e.g. sector Upstream B and sector Upstream C (typically 30 degrees)
# "SuperX3 Ref Zrot" is the rotation of the reference detector (SX3 D0) around the beam axis
# "SuperX3 Delta Zrot" is the difference between the rotation of 2 consecutive detectors around the beam axis

SuperX3 Active Length = 75.0
SuperX3 Active Width = 40.3

SuperX3 Ref Phi = Pi/2
SuperX3 Delta Phi = Pi/6

SuperX3 Ref Zrot = 0
SuperX3 Delta Zrot = Pi/6

--------------------------------------------------------------------------------------------------

# BB10 infos
# The BB10s share their angular and rotation values with the SuperX3

BB10 Active Width = 40.3 

