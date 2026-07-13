# City Walk Optimisation Entries: 

## After Removing Open Sets and Closed Sets to reduce HashSet lookups twice per frame A* Median went down from ~120 Microseconds to ~90 Microseconds, which is a 25% difference. 

## Currently The Project can work with 100 AI's in the scene. FPS Stats with 100 AI Actors: AVG: 46 FPS, MAX: 74, MIN: 3 ( right after starting the game )

I Also Tried to benchmark the same scene with 64 AI's which resulted in 49 FPS average.
Benchmarks Stats outside of UE Editor: AVG FPS 58 MAX 114 MIN 3
Since now Benchmarks are done in Game Environment ( outside of editor which consumes some performance ) 

# 128 Living AI's Performance:

## Performance Stats with no further GPU Optimisations or etc.

Performance with Full LOD for all AI Agents: AVG FPS 46.9, MAX 98, MIN 4 FPS.

LOD System is yet to be implemented

## Performance stats with Mesh LOD Optimisations

Performance with Full LOD for all AI Agents: AVG FPS 57.6, MAX 96.7, MIN 5 FPS.

## Performance Stats Before vs After Mesh Atlasing: 

Performance with a single mesh, containing 9 Materials per NPC: AVG FPS: 64.4, MAX 90.7,  MIN 7.5 FPS.
Performance with a single mesh, containing 1 Material  per NPC: AVG FPS: 93.1, MAX 115.1, MIN 8.5 FPS.

Draw Calls Went from ~1900 - 2000 to ~300 - 400. 82% Less.
Draw Thread Time went from 14.7 to 7.5 ms. 49% Less.

## Mesh Atlasing gave a 44% Higher Average FPS and 26% Higher Max FPS
