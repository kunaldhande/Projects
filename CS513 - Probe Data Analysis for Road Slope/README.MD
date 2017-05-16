Input: 		

Link data: data/Partition6467LinkData.csv		
Probe data: data/Partition6467ProbePoints.csv	

Output:	

data/MatchedPointsOutput.csv 
data/Slopes.csv 
data/Evaluation.csv



How to run:	

Run this command in commandline: 

1. Mapping probe data  with link data
    - Go MapMatching folder and type

        python map_matching.py


2. Calculating slope and evaluating with given slope
    - Go Slope folder and type

        python slope_calculation_evaluation.py



Note: Data folder should present in same directory as MapMatching and Slope
