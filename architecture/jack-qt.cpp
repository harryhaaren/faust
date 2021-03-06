/************************************************************************

	IMPORTANT NOTE : this file contains two clearly delimited sections : 
	the ARCHITECTURE section (in two parts) and the USER section. Each section 
	is governed by its own copyright and license. Please check individually 
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it 
    and/or modify it under the terms of the GNU General Public License 
	as published by the Free Software Foundation; either version 3 of 
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License 
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work 
	that contains this FAUST architecture section and distribute  
	that work under terms of your choice, so long as this FAUST 
	architecture section is not modified. 


 ************************************************************************
 ************************************************************************/

#include <libgen.h>
#include <stdlib.h>
#include <iostream>
#include <list>

#include "faust/dsp/timed-dsp.h"
#include "faust/gui/PathBuilder.h"
#include "faust/gui/FUI.h"
#include "faust/gui/JSONUI.h"
#include "faust/misc.h"
#include "faust/gui/faustqt.h"
#include "faust/audio/jack-dsp.h"

#ifdef OSCCTRL
#include "faust/gui/OSCUI.h"
#endif

#ifdef HTTPCTRL
#include "faust/gui/httpdUI.h"
#endif

// Always include this file, otherwise -poly only mode does not compile....
#include "faust/gui/MidiUI.h"

#ifdef MIDICTRL
#include "faust/midi/RtMidi.cpp"
#endif

/**************************BEGIN USER SECTION **************************/

/******************************************************************************
*******************************************************************************

							       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/

<<includeIntrinsic>>

<<includeclass>>

#ifdef POLY
#include "faust/dsp/poly-dsp.h"
#endif

dsp* DSP;

/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/
					
std::list<GUI*> GUI::fGuiList;
ztimedmap GUI::gTimedZoneMap;

/******************************************************************************
*******************************************************************************

                                MAIN PLAY THREAD

*******************************************************************************
*******************************************************************************/

static bool hasMIDISync()
{
    JSONUI jsonui;
    mydsp tmp_dsp;
    tmp_dsp.buildUserInterface(&jsonui);
    std::string json = jsonui.JSON();
    
    return ((json.find("midi") != std::string::npos) &&
            ((json.find("start") != std::string::npos) ||
            (json.find("stop") != std::string::npos) ||
            (json.find("clock") != std::string::npos)));
}

int main(int argc, char *argv[])
{
 	char name[256];
	char rcfilename[256];
	char* home = getenv("HOME");

	snprintf(name, 255, "%s", basename(argv[0]));
	snprintf(rcfilename, 255, "%s/.%src", home, name);
   
#ifdef POLY

    int poly = lopt(argv, "--poly", 4);
    int group = lopt(argv, "--group", 1);
 
#if MIDICTRL
    if (hasMIDISync()) {
        DSP = new timed_dsp(new mydsp_poly(poly, true, group));
    } else {
        DSP = new mydsp_poly(poly, true, group);
    }
#else
    DSP = new mydsp_poly(poly, false, group);
#endif

#else

#if MIDICTRL
    if (hasMIDISync()) {
        DSP = new timed_dsp(new mydsp());
    } else {
        DSP = new mydsp();
    }
#else
    DSP = new mydsp();
#endif
    
#endif
    
    if (DSP == 0) {
        std::cerr << "Unable to allocate Faust DSP object" << std::endl;
        exit(1);
    }

    QApplication myApp(argc, argv);

    QTGUI interface;
    FUI finterface;
    DSP->buildUserInterface(&interface);
    DSP->buildUserInterface(&finterface);

#ifdef HTTPCTRL
    httpdUI httpdinterface(name, DSP->getNumInputs(), DSP->getNumOutputs(), argc, argv);
    DSP->buildUserInterface(&httpdinterface);
    std::cout << "HTTPD is on" << std::endl;
#endif

#ifdef OSCCTRL
    OSCUI oscinterface(name, argc, argv);
    DSP->buildUserInterface(&oscinterface);
    std::cout << "OSC is on" << std::endl;
#endif

    jackaudio_midi audio;
#ifdef MIDICTRL
    audio.init(name, DSP, true);
#else
    audio.init(name, DSP);
#endif

#ifdef MIDICTRL
    int rtmidi = lopt(argv, "--rtmidi", 0);

    MidiUI* midiinterface;
    if (rtmidi) {
        midiinterface = new MidiUI(name);
        printf("RtMidi is used\n");
    } else {
        midiinterface = new MidiUI(&audio);
        printf("JACK MIDI is used\n");
    }
   
    DSP->buildUserInterface(midiinterface);
    std::cout << "MIDI is on" << std::endl;
#endif

    finterface.recallState(rcfilename);	
     
    audio.start();

    printf("ins %d\n", audio.get_num_inputs());
    printf("outs %d\n", audio.get_num_outputs());

#ifdef HTTPCTRL
    httpdinterface.run();
#ifdef QRCODECTRL
    interface.displayQRCode(httpdinterface.getTCPPort());
#endif
#endif

#ifdef OSCCTRL
    oscinterface.run();
#endif
#ifdef MIDICTRL
    midiinterface->run();
#endif
    interface.run();

    myApp.setStyleSheet(interface.styleSheet());
    myApp.exec();
    interface.stop();

    audio.stop();
    finterface.saveState(rcfilename);
     
    return 0;
}

/********************END ARCHITECTURE SECTION (part 2/2)****************/

