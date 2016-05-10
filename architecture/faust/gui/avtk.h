#ifndef FAUST_LV2_AVTK_GUI
#define FAUST_LV2_AVTK_GUI

#include "faust/gui/UI.h"

#include <avtk.hxx>

class WidgetEntry
{
public:
	WidgetEntry(Avtk::Widget* wid, float* val)
	{
		w = wid;
		v = val;
	}
	Avtk::Widget *w;
	float* v;
	float min;
	float max;
	float step;
};

class AvtkUI : public UI, public Avtk::UI
{

public:
	AvtkUI() :
		Avtk::UI(520, 300)
	{
		printf("avtkUI constructor\n");
		widgetX = 20;
		widgetY = widgetX + 60;
	}
	virtual ~AvtkUI()
	{
		printf("avtkUI destructor\n");
	}

	virtual void widgetValueCB(Avtk::Widget* w)
	{
		printf("%s : %f\n", w->label(), w->value());
		for(int i = 0; i < widgets.size(); i++) {
			if(w == widgets.at(i).w) {
				float min = widgets.at(i).min;
				float max = widgets.at(i).max;
				float step = widgets.at(i).step;
				*widgets.at(i).v = w->value() * (max-min) + min;
			}
		}
	}

	virtual int run()
	{
		if (metaAux.size() > 0) {
			for (size_t i = 0; i < metaAux.size(); i++) {
				unsigned num;
				if (metaAux[i].first == "midi") {
					if (sscanf(metaAux[i].second.c_str(), "ctrl %u", &num) == 1) {
						printf("found a MIDI CTRL thing\n");
						//fCtrlChangeTable[num].push_back(new uiMidiCtrlChange(fMidiHandler, num, this, zone, min, max, input));
					}
				}
				if (metaAux[i].first == "style") {
					printf("style : %s\n", metaAux[i].second.c_str());
				}
			}
		}
		Avtk::UI::run();
	}

	// -- widget's layouts
	void group_helper(const char* label)
	{
		printf("creating group %s now!\n", label);
		widgets.push_back(WidgetEntry(new Avtk::Group(this, widgetX, widgetY, 90, 20, label), 0));
		currentGroup = (Avtk::Group*)widgets.back().w;
	}
	virtual void openTabBox(const char* label)
	{
		printf("tab box %s\n", label);
		group_helper(label);
	}
	virtual void openHorizontalBox(const char* label)
	{
		printf("Horizontal box %s\n", label);
		group_helper(label);
	}
	virtual void openVerticalBox(const char* label)
	{
		printf("vertical box %s\n", label);
		group_helper(label);
	}
	virtual void closeBox()
	{
		printf("Close box\n");
		currentGroup->end();
	}

	Avtk::Group* currentGroup;

	// -- active widgets
	virtual void addButton(const char* label, FAUSTFLOAT* zone) {}
	virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) {}
	virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {}
	virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
	{
		widgets.push_back(WidgetEntry(new Avtk::Slider(this, 20, widgetY, 90, 20, label), zone));
		widgets.back().min = min;
		widgets.back().max = max;
		widgets.back().step = step;
		// draw label on widget
		widgets.back().w->label_visible = 1;
		widgetY += 25;
	}
	virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
	{
		widgets.push_back(WidgetEntry(new Avtk::Dial(this, widgetX, 20, 50, 50, label), zone));
		widgets.back().min = min;
		widgets.back().max = max;
		widgets.back().step = step;
		widgetX += 55;
	}

	// -- passive widgets
	virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
	virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}

	// -- metadata declarations
	virtual void declare(FAUSTFLOAT* ptr, const char* key, const char* val)
	{
		metaAux.push_back(std::make_pair(key, val));
		//printf("dec %s : %s : %s\n", widgets.at(int(ptr)), m1, m2);
		/*
		for(int i = 0; i < widgets.size(); i++) {
			if(ptr == widgets.at(i).v)
				printf("\twidget: %s : %s : %s\n", m1, m2);
		}
		*/
	}

	int widgetX;
	int widgetY;

	std::vector<WidgetEntry> widgets;
	std::vector<std::pair <std::string, std::string> > metaAux;
};

#endif
