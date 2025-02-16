/*
#include <ctime>
#include <boost/statechart/event.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/transition.hpp>

namespace sc = boost::statechart;

struct EvStartStop : sc::event< EvStartStop > {};
struct EvReset : sc::event< EvReset > {};

struct Active;
struct StopWatch : sc::state_machine< StopWatch, Active > {};

struct Stopped;



// The simple_state class template accepts up to four parameters:
// - The third parameter specifies the inner initial state, if
//   there is one. Here, only Active has inner states, which is
//   why it needs to pass its inner initial state Stopped to its
//   base
// - The fourth parameter specifies whether and what kind of
//   history is kept

// Active is the outermost state and therefore needs to pass the
// state machine class it belongs to

struct Active : sc::simple_state< Active, StopWatch, Stopped >
{
  public:
    typedef sc::transition< EvReset, Active > reactions;

    Active() : elapsedTime_( 0.0 ) {}
    double ElapsedTime() const { return elapsedTime_; }
    double & ElapsedTime() { return elapsedTime_; }
  private:
    double elapsedTime_;
};

// Stopped and Running both specify Active as their Context,
// which makes them nested inside Active
struct Running : sc::simple_state< Running, Active >
{
  typedef sc::transition< EvStartStop, Stopped > reactions;
};

struct Stopped : sc::simple_state< Stopped, Active >
{
  typedef sc::transition< EvStartStop, Running > reactions;
};

// Because the context of a state must be a complete type (i.e.
// not forward declared), a machine must be defined from
// "outside to inside". That is, we always start with the state
// machine, followed by outermost states, followed by the direct
// inner states of outermost states and so on. We can do so in a
// breadth-first or depth-first way or employ a mixture of the
// two.

int main()
{
  StopWatch myWatch;
  myWatch.initiate(); 
  myWatch.process_event( EvStartStop() );
  myWatch.process_event( EvStartStop() );
  myWatch.process_event( EvStartStop() );
  myWatch.process_event( EvReset() );
  return 0;;
  return 0;
}
*/

#include <iostream>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state_machine.hpp>
//#include <boost/statechart/state.hpp>
#include <boost/statechart/event.hpp>
#include <boost/statechart/transition.hpp>

// 네임스페이스 정의
namespace sc = boost::statechart;

// 이벤트 정의
struct EventA : sc::event<EventA> {};
struct EventB : sc::event<EventB> {};

// 상태 기계 정의
struct MyStateMachine;

// 상태 정의
struct State1;
struct State2;

struct MyStateMachine : sc::state_machine<MyStateMachine, State1> {};

// State1: 첫 번째 상태
struct State1 : sc::simple_state<State1, MyStateMachine> {
public:
    State1() {
        std::cout << "Entered State1" << std::endl;
    }
    typedef sc::transition<EventA, State2> reactions;

    // EventA 이벤트가 발생하면 State2로 전환
};

// State2: 두 번째 상태
struct State2 : sc::simple_state<State2, MyStateMachine> {
    State2() {
        std::cout << "Entered State2" << std::endl;
    }
    typedef sc::transition<EventB, State1> reactions;

    // EventB 이벤트가 발생하면 State1로 전환
};

class client{
public:
  client(){
    fsm_.initiate();

    fsm_.process_event(EventA());
  }
private:
    MyStateMachine fsm_;
};

int main() {

  client cli_;
    //MyStateMachine fsm;
    //fsm.initiate();  // 상태 기계 초기화

    //fsm.process_event(EventA());  // State1에서 EventA 발생 -> State2로 전환
    //fsm.process_event(EventB());  // State2에서 EventB 발생 -> State1로 전환

    return 0;
}

