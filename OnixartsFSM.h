// Onixarts_FSM is a Finite State Machine implementation library for Arduino.
// FSM helps with state based system implementation.
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

#ifndef _OnixartsFSM_h
#define _OnixartsFSM_h

#include "Arduino.h"
#define BEGIN_STATE(parentClass, stateClass, transitionsCount)\
					class stateClass : public Onixarts::Tools::FSM::State{\
						parentClass& m_parent;\
						byte idx;\
						Onixarts::Tools::FSM::Transition transitionsTable[transitionsCount];\
						parentClass& GetParent() {return m_parent;}\
						public:\
						stateClass(parentClass* parent) : m_parent(*parent), idx(0) { size = transitionsCount; transitions = transitionsTable; memset(transitionsTable, 0, size*sizeof(Onixarts::Tools::FSM::Transition) ); \
						
#define TRANSITION(event, toState)\
	if(idx < size) \
	transitionsTable[idx++].Init(event, &(m_parent.toState));\

#define END_STATE(instanceName)	} instanceName;

namespace Onixarts 
{ 
	namespace Tools 
	{
		namespace FSM 
		{
			#define InsertTransition(trans) trans, sizeof(trans)/sizeof(trans[0])
			class State;
			typedef void (*FSMEventDelegate)(byte eventID);
			//-------------------------------------------------------------------------------------------------------------------
			struct Transition
			{
				byte eventID;
				State* state;
				void Init(uint8_t eventID, State* nextState) { this->eventID = eventID; state = nextState; }
			};
			//-------------------------------------------------------------------------------------------------------------------
			class Machine
			{
				protected:
					State* currentState;
				public:
					Machine();
					Machine(State& state);
					void Notify(byte eventID );
					State* GetCurrentState() { return this->currentState; }
					void SetCurrentState(State& state); 

			};
			//-------------------------------------------------------------------------------------------------------------------
			class State
			{
				protected:
					Transition* transitions;
					uint8_t size;
					
					FSMEventDelegate m_enterDelegate;	// pointer to enter state callback
					FSMEventDelegate m_exitDelegate;	// pointer to exit state callback

					virtual void OnEnter(byte eventID) {};
					virtual void OnExit(byte eventID) {};

				public:
					State();
					State(FSMEventDelegate enterDelegate);
					State(FSMEventDelegate enterDelegate, FSMEventDelegate exitDelegate);
					bool Notify(byte eventID,  State*& nextState ); // returns true if there was a transformation. nextState is set to state from transitions array, or to this if event not found.
					void SetTransitions(Transition* transitions, byte size);
					
					void SetEnterEventDelegate(FSMEventDelegate fsmDelegate) { m_enterDelegate = fsmDelegate; }
					void SetExitEventDelegate(FSMEventDelegate fsmDelegate) { m_exitDelegate = fsmDelegate; }
					
					void Enter(byte eventID = 0);
					void Exit(byte eventID = 0);
			};
		}
	}
}

#endif
