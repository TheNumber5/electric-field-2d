# Description
A simple C program that can calculate resulting electric gradients from a file and output it into another file in the format `<x_position> <y_position> <electric_field_intensity`, or electric vector fields in the form `<x_position> <y_position> <electric_field_intensity_x> <electric_field_intensity_y>`.
# Instructions
The command format is: `ElectricField2D.exe <Input file path> <Output file path> <Command>`.
For gradient, the command is `-g` and for vector field it is `-vf`.
The input file format is:
```
<size of simulation field> <field precision step> <relative permittivity of space> <proximity to particle ignoring margin> <number of charged particles>
<x position of particle 1> <y position of particle 1> <particle 1 charge in Coulombs>
<x position of particle 2> <y position of particle 2> <particle 2 charge in Coulombs>
...
<x position of particle 2> <y position of particle 2> <particle 2 charge in Coulombs>
```
Example of input file:
```
20 0.2 1 0.5 3
0 3 0.000000000016
0 -3 -0.000000000016
0 1 -0.000000000016
```
Here there are 2 particles with the electric charge of 100 million protons and one with the charge of 100 million electrons.

To view the gradient file in gnuplot you can use:
```
plot "[Path to output file]" with image
```
And to view the vector field file in gnuplot you can use:
```
plot "[Path to output file]" using 1:2:3:4 with vectors head filled
```
