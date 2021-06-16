# Tsiltsen -- a tool for working with human-readable netlists.

## Purpose
I never like drawing schematics with a COD tool if they were never intended for use in producing a PCB. I always found using a bare netlist format much
more natural. A netlist lists the connections between pins on the various components in your design, and can be made very compact if you take advantage 
of the repitition inherent in much design work, eg a counter IC has eight outputs Q0, Q1,... Q7, or more succinctly Q[0-7].

A netlist is also the _natural_ format to use if you are wiring up a prototype by hand. Each time you make a connection, you strike it out on the netlist.

## A Simple example

Here's a very simple example circuit.
