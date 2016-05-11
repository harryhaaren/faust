// test case for gate button
import("oscillator.lib");

midigate	= button ("gate");
midifreq	= 110;
process = saw2(midifreq) * midigate : _ ;
