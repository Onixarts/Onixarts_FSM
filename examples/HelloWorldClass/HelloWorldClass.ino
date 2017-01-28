// Onixarts_FSM HellowWorldClass demo.
// Connect 3 buttons to pins 8,9,10 and GND
// FSM will change states from S1->S2->S3-> S1.... when button8, button9, button10 is pressed one after other
// Transitions between states occured on defined events passed to FSM.Notify() method:
// S1(button1Pushed)->S2
// S2(button2Pushed)->S3
// S2(Reset)->S1
// S3(button3Pushed)->S1
// S3(Reset)->S1
// Implementation in class.
//
// Github: https://github.com/Onixarts/Onixarts_FSM
// Author's site: http://onixarts.pl
// Contact: software@onixarts.pl
// 
// Copyright (C) 2017  Bartosz Rosa (onixarts.pl)
//
// This program is free software : you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.If not, see <http://www.gnu.org/licenses/>.

#include "Arduino.h"
#include <OnixartsFSM.h>

// we're using namespace Onixarts::Tools defined in OnixartsFSM library
using namespace Onixarts::Tools;

// events definition in namespace Event, so it can be called by typing Event::Reset
namespace Event
{
	const byte Button1Pushed = 1;
	const byte Button2Pushed = 2;
	const byte Button3Pushed = 3;
	const byte Reset = 4;
}

// class definition inherits fro FSM::Machine class. You can also put FSm::Machine as a class member
class MyClass : public FSM::Machine
{
	// declare states as inner classes using macros
	// In BEGIN_STATE macro pass parent class name (MyClass), state class (State1), and transitions count for this state
	// In TRANSITION simply pass event and target state instance
	//---------------------------------------------------------------
	BEGIN_STATE(MyClass, State1, 1)
	TRANSITION(Event::Button1Pushed, state2)
	};
	void OnEnter()
	{
		// put Your Enter event code here
		Serial.println("State 1 entered");

		// accessing parent class (MyClass) members
		// GetParent().DoSomething();
	}
	END_STATE(state1) // type the state instance name
	//---------------------------------------------------------------
	BEGIN_STATE(MyClass, State2, 2)
	TRANSITION(Event::Button2Pushed, state3)
	TRANSITION(Event::Reset, state1)
	};
	void OnEnter()
	{
		Serial.println("State 2 entered");
	}
	END_STATE(state2)
	//---------------------------------------------------------------
	BEGIN_STATE(MyClass, State3, 2)
	TRANSITION(Event::Button3Pushed, state1)
	TRANSITION(Event::Reset, state1)
	};
	void OnEnter()
	{
		Serial.println("State 3 entered");
	}
	END_STATE(state3)
	//---------------------------------------------------------------

public:

	// In constructor's initialization list init all states with passing this pointer
	MyClass()
		: FSM::Machine() // don't put initial state here, because Start event will be generated for this task before Arduino's setup()
		, state1(this)
		, state2(this)
		, state3(this)
	{};

	// Set initial state
	void Init()
	{
		SetCurrentState(state1);
	}
};

// declaration of MyClass
MyClass myClass;

void setup()
{
	Serial.begin(115200);
	Serial.println("Onixarts FSM class demo");

	// set pin 8,9,10 as input with PULL UP resistors
	pinMode(8, INPUT_PULLUP);
	pinMode(9, INPUT_PULLUP);
	pinMode(10, INPUT_PULLUP);

	// init will set initial state. You can omit this call and set initial state in FSM:Machine constructor, but it will be called before setup() function
	myClass.Init();
}

void loop()
{
	// read inputs and notify FSM if any connected button is pushed (state LOW)

	if (!digitalRead(8))
		myClass.Notify(Event::Button1Pushed);

	if (!digitalRead(9))
		myClass.Notify(Event::Button2Pushed);

	if (!digitalRead(10))
		myClass.Notify(Event::Button3Pushed);
}