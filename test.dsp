// import the needed libraries
declare name 		"tester";
declare version 	"1.0";
declare author 		"harry";
declare license 	"BSD";
declare copyright 	"(c) 2006";

import("music.lib");
import("oscillator.lib");

// generate a sawtooth wave, using "saw1" from "oscillator.lib"
sawGen = saw1( vslider("Freq", 60, 0, 127, 0.1) );

// get variables for A D S and R stages, using sliders
attack     = 1.0/(SR*nentry("[1:]attack [unit:ms][style:knob]", 20, 1, 1000, 1)/1000);
decay      = nentry("[2:]decay[style:knob]", 2, 1, 100, 0.1)/100000;
sustain    = nentry("[3:]sustain [unit:pc][style:knob]", 10, 1, 100, 0.1)/100;
release    = nentry("[4:]release[style:knob]", 10, 1, 100, 0.1)/100000;

balance    = hslider("balance", 0.5, 0, 1, 0.0000001);

// set the process function, using the button to trigger the ADSR, and then volume scale the sawtooth
process =  button("play"): hgroup("ADSR", adsr(attack, decay, sustain, release) : *(sawGen)) * balance;
