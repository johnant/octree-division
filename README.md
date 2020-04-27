Copyright 2015

Ioannis Antoniadis

Aristotle University of Thessaloniki, Greece

Department of Electrical and Computer Engineering

# octree-division
Octree spatial division

## Algorithm
Given an array with dimentions Nx3 and a natural number S, the algorithm attempts the division of the 3D space using an octree into structures that contain the total space information. The algorithm was first introduced in 1980 by Donald Meagher at Rensellaer University of New York and was used in Computer Graphics, representation and processing of three-dimentional objects.

The algorithm initiates with the coordinates (x,y,z) of N points in 3D space. The algorithm splits the 3D space into cubes so that, given a number S, each cube contains S or less points. To simplify the process, the current implementation assumes that:
* the N input points reside in the first eighth of the unit sphere
* the first, single cube of the division is the unit cube, centered at (0,0,0)

## Implementation
The current, parallel implementation is written in C with two different techniques:
* OpenMP
* p-threads

## Input
The Nx3 array of the 3D points and the number S can be set within the source code.
