# UFlow

UFlow is a Urban sprawl simulator.

## Overview

The name steems from 'urban flow', in reference to the way the system is modelled.

The diffusion equation is used to compute future states of a city; the quantity u(x,y) represents urbanization, and the condition (u > min) triggers the conversion from empty to occupied land. The system does not consider multiple land categories. Initial values for u are set to represent a binary image, but the model invites to think on other possibilities as population density; this rests to be tested.

A matrix of coefficients is calibrated to fit the observed evolution between two images. It is used in a second step to compute a forecast. The coefficients are adjusted locally, trying to make the diffusion process match the second image. This contrasts with approaches that make use of global parameters.


## Installation and use

A succinct manual is available.
Run the makefile, then execute the model with one of the examples provided.
The program generates two log files and a few output images.

It might be interesting to play with parameters and images to get the feel of how the simulator reacts.


## Built with

 * [Lodepng](https://github.com/lvandeve/lodepng) Version 20180611, from Lode Vandevenne


## License

GPLv3


## Author

André Koscianski (koscianski.org) .

I can be reached by email at the university I work: utfpr.edu.br.
