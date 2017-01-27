## Onixarts_FSM
Finite State Machine implementation for Arduino. You can define states that Your system or object can be in, and transitions between these states fired by specified events.

### Features
- Unlimited states
- Up to 256 transitions for each state
- Enter and Exit events for each state
- Events global callback functions
- Can be used internally in class objects
- No dynamic memory allocation

### Overview
Library is build with two classes:

`Machine` handles notifications and switching states. 

`State` defines single objects or system state. Each `State` has attached `Transition` array, where You can define transitions to another states on specific events.

If machine is currently in `state1` and it has defined transition to `state2` on button 1 push event it will switch from `state1` to `state2` when You Notify `Machine` with `Event::Button1Pressed` event.
Before switching the Exit event is raised on `state1`. You can attach a callback function for this event. After state beeing switched to `state2` the Enter event is raised in `state2`, which You can also use.

### Why I need this?
You don't :). But sometimes is much easier to implement complicated system behavior by states and transitions than multiple switch, case and if statements. Here You have code blocks that acts like You defined in 
transitions. It is very easy to maintain. Just draw Your states, events and transitions on a paper, then translate it to the code.

### Basic usage
First, You need instantiate `Machine` and `State` objects like this:

```C++
using namespace Onixarts::Tools;
FSM::Machine fsm;
FSM::State state1, state2, state3;
```

You can also pass to `State` constructor enter and exit callback function delegates like:

```C++
FSM::State state1(OnState1Entered);
FSM::State state2(OnState2Entered, OnState2Exit);
FSM::State state3(NULL, OnState3Exit); 
```

Next, lets define some events that my occure in our system. Events are just a byte number. I've put all events in namespace Event, so I can get it by typing Event::Button1Pushed for example. 
You can type just #define if You like.

```C++
namespace Event
{
	const byte Button1Pushed = 1;
	const byte Button2Pushed = 2;
	const byte Button3Pushed = 3;
	const byte Reset = 4;
}
```

Now it's time to define transitions. `Transition` is a struct with two parameters: event and target state. 

```C++
FSM::Transition state1Transations[] = { { Event::Button1Pushed, &state2 } };
FSM::Transition state2Transations[] = { { Event::Button2Pushed, &state3 },
										{ Event::Reset, &state1 }
									  };
FSM::Transition state3Transations[] = { { Event::Button3Pushed, &state1 }, 
										{ Event::Reset, &state1 }
									  };
```

I defined here three `Transition` arrays, one for each state. In `state1Transitions` array there is only one transition defined, from current (`state1`) state to `state2` on `Event::Button1Pushed`.
The `state2Transitions` has got two transitions. One for `state3` on `Event::Button2Pushed` and one for `state1` on `Event::Reset`, etc.

Because this library don't use dynamic memory allocation (for safety reasons) You have to attach these arrays to specific states. This code should be places in `setup()` function.

```C++
state1.SetTransitions(InsertTransition(state1Transations));
state2.SetTransitions(InsertTransition(state2Transations));
state3.SetTransitions(InsertTransition(state3Transations));
```

The `InsertTransition` macro helps calculating `Transition` array size, so the `State` class knows how many transitions are defined.

If You didn't specify callback functions in `State` constructors You can always attach it by calling `SetEnterEventDeleate` and `SetExitEventDelegate` functions:

```C++
state1.SetEnterEventDelegate(OnState1Entered);
state2.SetEnterEventDelegate(OnState2Entered);
state2.SetExitEventDelegate(OnState2Exit);
state3.SetExitEventDelegate(OnState3Exit);
```

Last thing that should be done is to set current, initial state in a `setup()` function.

```C++
fsm.SetCurrentState(state1);
```

You can now change states by calling `Notify()` method. Let's say we want to change states like S1->S2->S3-> S1... if we push button1, button2 and button3 one after other. In `loop()` function 
we check if button connected to Arduino PINs are pushed and notify `Machine` class object of that:

```C++
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
```

If any of three buttons is pressed we send event notification to `Machine` object. If there is a `Transition` defined for specified event in current machine state it will fire the transition.
Firing transition will also raise Exit and Enter events in current and target state, so we can simply react on this changes.

```C++
// Callback function called when FSM enters state 1
void OnState1Entered()
{
	Serial.println("State 1 entered");
}

// Callback function called when FSM enters state 2
void OnState2Entered()
{
	Serial.println("State 2 entered");
}

// Callback function called when FSM enters state 3
void OnState3Entered()
{
	Serial.println("State 3 entered");
}
```

Complete code You can find in HelloWorld example folder.

### Advanced usage in class
TODO