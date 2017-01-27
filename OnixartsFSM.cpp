#include "OnixartsFSM.h"

using namespace Onixarts::Tools::FSM;

Machine::Machine()
: currentState (NULL)
{
	
}

void Machine::Notify(byte event )
{
	if( currentState == NULL )
		return;
	
	State* nextState = NULL;
	if( currentState->Notify( event, nextState ) )
	{
		currentState->Exit();
		this->currentState = nextState;
		if( currentState != NULL )
			currentState->Enter();
	}
}

void Machine::SetCurrentState(State& state)	
{
	this->currentState = &state; 
	if(this->currentState != NULL ) 
		this->currentState->Enter();
}
//----------------------------------------------------------------------------------------

State::State()
: transitions( NULL )
, size(0)
, m_enterDelegate(NULL)
, m_exitDelegate(NULL)
{
}

State::State(FSMEventDelegate enterDelegate)
: transitions(NULL)
, size(0)
, m_enterDelegate(enterDelegate)
, m_exitDelegate(NULL)
{
}

State::State(FSMEventDelegate enterDelegate, FSMEventDelegate exitDelegate)
: transitions(NULL)
, size(0)
, m_enterDelegate(enterDelegate)
, m_exitDelegate(exitDelegate)
{
}

void State::SetTransitions( Transition* transitions, byte size )
{
	this->transitions = transitions;
	this->size = size;
}

bool State::Notify(byte eventID, State*& nextState )
{
	for(uint8_t i = 0; i < size; i++)
	{
		if( transitions[i].eventID == eventID )
		{
			nextState = transitions[i].state;
			return true;
		}
	}
	nextState = this;
	return false;
}

void State::Enter()
{
	OnEnter();
	if (m_enterDelegate != NULL)
		m_enterDelegate();
}

void State::Exit()
{
	OnExit();
	if (m_exitDelegate != NULL)
		m_exitDelegate();
}