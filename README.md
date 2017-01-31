# Onixarts FSM
Finite State Machine implementation for Arduino. You can define states that Your system or object can be in, and transitions between these states fired by specified events.

## Features
- Unlimited states
- Up to 256 transitions for each state
- Enter and Exit events for each state
- Events global callback functions
- Can be used internally in class objects
- No dynamic memory allocation

## Overview
Library is build with two classes:

`Machine` handles notifications and switching states. 

`State` defines single objects or system state. Each `State` has attached `Transition` array, where You can define transitions to another states on specific events.

If machine is currently in `state1` and it has defined transition to `state2` on button 1 push event it will switch from `state1` to `state2` when You Notify `Machine` with `Event::Button1Pressed` event.
Before switching the Exit event is raised on `state1`. You can attach a callback function for this event. After state beeing switched to `state2` the Enter event is raised in `state2`, which You can also use.

## Why I need this?
You don't :). But sometimes is much easier to implement complicated system behavior by states and transitions than multiple switch, case and if statements. Here You have code blocks that acts like You defined in 
transitions. It is very easy to maintain. Just draw Your states, events and transitions on a paper, then translate it to the code.

## Basic usage
First, You need instantiate `Machine` and `State` objects like this:

```C++
using namespace Onixarts::Tools;
FSM::Machine fsm;
FSM::State state1, state2, state3;
```

You can also pass to `State` constructor enter and exit callback function delegates and set initial state like:

```C++
FSM::State state1(OnState1Entered);
FSM::State state2(OnState2Entered, OnState2Exit);
FSM::State state3(NULL, OnState3Exit); 
FSM::Machine fsm(state1);
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

If You didn't set initial state in constructor You can always do it in `setup()` function by calling `SetCurrentState()`. This function also can be used to force state change. 

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

## Advanced usage in class

As You may know, You can't simply pass object method as callback function, because in compilation time `this` pointer is undefined - so the compiler won't know which object's method should be called. In such cases You are usually define global callback function and then in function body You call myObject1.DoSomething() or declare class method as static - but it has no access to the object's data however.

In Onixarts_FS library You can implement derived states inside Your class. There are some macros defined that will simplify proces to minimum lines od code.

Below is an example of custom class `MyClass` with state definition.

```C++
class MyClass : public FSM::Machine
{
	BEGIN_STATE(MyClass, State2, 2)
	TRANSITION(Event::Button2Pushed, state3)
	TRANSITION(Event::Reset, state1)
	};
	void OnEnter()
	{
		Serial.println("State 1 entered");

		// accessing parent class (MyClass) members
		// GetParent().DoSomething();
	}
	END_STATE(state1) // type the state instance name

public:
	MyClass()
		: FSM::Machine() // don't put initial state here, because Start event will be generated for this task before Arduino's setup()
		, state1(this)
		//, state2(this)
		//, state3(this)
	{};

	void Init()
	{
		SetCurrentState(state1);
	}
};

```

`MyClass` inherits from `FSM::Machine` so You can directly call `Notify()` method on Your class object. To define new state use `BEGIN_STATE()` macro. First parameter is the parent class name, 
second is the defined state name (`State1` here), the third one is the number of transitions for this task. You should always put here proper transitions cound number. Lower number
will cause that some transitions won't be added to task, higher will work, but it will allocate unnecessarily SRAM memory.

After the state definition put state transitions to another states defined in `MyClass`. Simply type `TRANSITION(event, targetTaskName)`. Don't forget to put `};` in the end of transitions (because of the macro structure).
Notice the lowercase state instances names here, not the state name with Capital letter. 

Next You can define `OnEnter()` and `OnExit()` methods that will be called by Machine when transition occured. You can access parent class methods by calling `GetParent()` method.

Another thing You should do is to call constructors for each defined state in parent class constructor initialization list. 

Because it is very common scenario that `MyClass` object will be declared in global space in ino file You can't acctualy know when exactly this constructor will be called. It will be called before `setup()` function, where 
for example You can't use `Serial` output. So instead of passing initial state to the `FSM::Machine` constructor, call `SetCurrentState()` in `Init()` method of the `MyClass` object, from the `setup()`.
After calling this function `OnEnter()` method will be called on initial state.


```C++
MyClass myClass;

void setup()
{
	Serial.begin(115200);
	Serial.println("Onixarts FSM class demo");

    // this function should set initial state in the class.
	myClass.Init();
}
```

Complete code You can find in HelloWorldClass example. It does the same thing that HelloWorld do, but using class.