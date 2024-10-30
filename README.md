A lightweight OpenFOAM function object for switching numerical schemes at specified times during runtime. Designed for OpenFOAM v2012 with performance and scalability in mind.
The schemeSwitcher allows you to dynamically change numerical schemes during simulation without the overhead of runTimeModifiable option. This function object allows for switching to from low to high-order scheme at a given time-step without killing the simulation. Currently, the only criteria for switch is the time step.
