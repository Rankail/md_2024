# Mathe Dual Wettbewerb 2024 - tetris-for-the-win

Some explanations for this repo:

## utils
- Logger: using [spdlog](https://github.com/gabime/spdlog)
- reading input & printing results
- data-structs for graphicdata
- score-calculation
- modern Matrix-header written by me
- Argument class for better arguments (only really used in first solver)
- some other small utils functions/defines

## pointCalc / pointCalc2
calculates scores for all files\
v2 is after midterm

## solver001
constant number of iterations with push-/pull-forces

## solver002
iterate over nodes and place based on 1 neigbor (really bad)

## visualizer
- simple visualization of output-files (doesn't calculate score)

## visualizer001
- visualizes current state 
- interactive (functions like solver001)

keys:
- qwert: 1/10/100/1k/10k iterations once
- asdfg: 1/10/100/1k/10k iterations repeated while held
- esc: quit
- p: print current state to output file
- 1234567890ß´: switch to that input
- m: print score to console

## visualizer003
keys:
- c: find smallest scaling factor without collisions
(no i don't know why i made an entirely different folder just for this)

## visualizer004
tried [Box2d](https://github.com/erincatto/box2d)'s [DistanceJoint](https://box2d.org/documentation_v3/md_dynamics.html#autotoc_md86)\
it was wiggling way to much and i didn't get it to calm down or give any good results

## visualizer005
always calculate and display score (empty score shown in earlier visualizers because i was lazy)
separated distance, overlap & angle-forces and added a factor for each\
loads old score on start and prints result if new highscore was found\
this gave me by far the best outputs before midterm:
1. find minimal non collision scaling
2. decrease angle-force (it would always destory the score)
3. run some iterations and probably decrease distance-strength

keys:
- removed p
- arrows: move camera
- +#: bad camera zooming
- x: calculate best rotation
- y: randomize node-positions (only useful for Population-Density-Afro-Eurasia)
- uio: strengthen factor for distance/overlap/angle-forces
- jkl: weaken factor for distance/overlap/angle-forces

### midterm

## visualizer006
just switched to the new score-function\
i never got around to fix the rotation function to use the average

## visualizer007
i had an idea with polygons but never got around to fully implement it\
this constructs all polygons made by the edges between nodes\
does not account for non-connected parts overlapping

### 10-day-break

## visualizer008
first try on gradient descent\
never got the derivative for the angle working\
thought would switch to a numeric approach (i clearly don't understand what that actually is)\
somehow got some better scores