// Onixarts_FSM HellowWorld demo.
// Connect 3 buttons to pins 8,9,10 and GND
// FSM will change states from S1->S2->S3-> S1.... when button8, button9, button10 is pressed one after other
// Transitions between states occured on defined events passed to FSM.Notify() method:
// S1(button1Pushed)->S2
// S2(button2Pushed)->S3
// S2(Reset)->S1
// S3(button3Pushed)->S1
// S3(Reset)->S1
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

// declaration few states objects
// You can also pass enter and exit callback function in constructor or nothing - then You can add this by calling SetEnterEventDelegate(...) function
FSM::State state1(OnState1Entered);
FSM::State state2(OnState2Entered);
FSM::State state3(OnState3Entered); 

// declaration of FS Machine with initial state
FSM::Machine fsm(state1);

// transitions declaration as arrays of Transition structs for each state
FSM::Transition state1Transations[] = { { Event::Button1Pushed, &state2 } };
FSM::Transition state2Transations[] = { { Event::Button2Pushed, &state3 },
										{ Event::Reset, &state1 }
									  };
FSM::Transition state3Transations[] = { { Event::Button3Pushed, &state1 }, 
										{ Event::Reset, &state1 }
									  };

void setup()
{
	Serial.begin(115200);
	Serial.println("Onixarts FSM demo");

	// set pin 8,9,10 as input with PULL UP resistors
	pinMode(8, INPUT_PULLUP);
	pinMode(9, INPUT_PULLUP);
	pinMode(10, INPUT_PULLUP);

	// set transitions arrays for each state
	state1.SetTransitions(InsertTransition(state1Transations));
	state2.SetTransitions(InsertTransition(state2Transations));
	state3.SetTransitions(InsertTransition(state3Transations));

	// set initial state if You didn't set it in machine constructor
	// fsm.SetCurrentState(state1);
}

void loop()
{
	// read inputs and notify FSM if any connected button is pushed (state LOW)

	if (!digitalRead(8))
		fsm.Notify(Event::Button1Pushed);

	if (!digitalRead(9))
		fsm.Notify(Event::Button2Pushed);

	if (!digitalRead(10))
		fsm.Notify(Event::Button3Pushed);
}

// Callback function called when FSM enters state 1
void OnState1Entered(byte event)
{
	Serial.println("State 1 entered");
}

// Callback function called when FSM enters state 2
void OnState2Entered(byte event)
{
	Serial.println("State 2 entered");
}

// Callback function called when FSM enters state 3
void OnState3Entered(byte event)
{
	Serial.println("State 3 entered");
}
