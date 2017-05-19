// test case for gate button
import("oscillator.lib");

midigate	= button ("gate");
midifreq	= hslider("freq", 330, 110, 4400, 0.01);
process = saw2(midifreq) * midigate : _ ;


