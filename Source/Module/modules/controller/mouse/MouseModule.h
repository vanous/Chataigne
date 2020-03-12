/*
  ==============================================================================

    MouseModule.h
    Created: 12 Mar 2020 3:08:47pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

#include "../../../Module.h"

class MouseModule :
	public Module,
	public MouseListener,
	public Timer
{
public:
	MouseModule();
	~MouseModule();

	void clearItem() override;

	MouseInputSource* source;

	IntParameter* updateRate;

	IntParameter* currentScreen;
	Point2DParameter* absolutePosition;
	Point2DParameter* relativePosition;
	BoolParameter* leftButtonDown;
	BoolParameter* middleButtonDown;
	BoolParameter* rightButtonDown;


	void setCursorPosition(Point<float>& pos, bool isRelative);
	void setButtonDown(int buttonID);
	void setButtonUp(int buttonID);
	void setButtonClick(int buttonID);

	void sendButtonEvent(int buttonEvent);

	void mouseDown(const MouseEvent& e) override;
	void mouseUp(const MouseEvent& e) override;

	void onControllableFeedbackUpdateInternal(ControllableContainer* cc, Controllable* c) override;
	
	void updateMouseInfos();

	void timerCallback() override;

	String getTypeString() const override { return getTypeStringStatic(); }
	const static String getTypeStringStatic() { return "Mouse"; }

	static MouseModule* create() { return new MouseModule(); }

};
