# cppCore

This is a response to StimService, EyeTrack, and xpControl from rigBase.

I've had the last year to try and put it into some sort of order and it's still
acting terribly, so I'm beginning to think it's time to construct something new.

Design principles are use whatever the best tool for the job is, assuming I can
keep it consistent. Since I have vcpkg setup, I can easily use boost, zeromq,
and some other common libraries to try and make a more simple and elegant
version quickly. This is a *fast* attempt at a common framework given the
identified requirements:

## User story

I have a primate subject that I trained to perform a simple touch or
manipulation task. Using standard training protocols, we can reward the subject
with water using the NIDAQ system, and move robots using their almost always C
interface libraries. As a scientist, I care about quickly making modifications
to my protocol and being able to incorporate new features quickly.

## Software Engineer perspective

We need a data-driven model of the protocol that's easily extensible for new
hardware and can be reprogrammed easily. Most of the code if possible should be
in higher level languages, at least the protocol section, so it can be
understood and modified by a scientist without worrying about things like
memory leaks. This necessitates something like PsychoPy to drive the stimulus
and a client/server connection to a c++ application that acts as a ZeroMQ
server to receive commands from PsychoPy and relay them to a seperate thread for
the hardware control.

PsychoPy already provides a fully featured Protocol model with millisecond
temporal accuracy, and is in a good systems level language (which Matlab is not)
allowing it to directly control hardware or interface well with C if required
(see projects like Cython and Boost.Python). PsychoPy already has a remote
control ability as well, so this can replace xpControl.

This is a service model application since it has no direct UI component.
StimService is a good model to start from, but can be significantly simplified.
Threading will be required, and each separate hardware system, such as NIDAQ,
Blackrock Stimulators, or the various motion control systems can be spawned
through a common "Stimulator" interface on it's own thread. Threadpooling is
probably the appropriate answer. 

This should be done with the standard library as much as possible on C++17 or
greater, it's better to have smart pointers etc available to minimize
programmer error. 

I like the idea of using promises/futures internally for thread comm, and zeromq
for sockets out to PsychoPy or other interfaces, allowing me to easily provide
status updates and commands on the wire without crossing those wires.
Implementation details need to be simple, reliable, and battle-tested already,
we don't want to mess with reinventing the wheel.

Starting here, and adding the boost logging, program options, and property trees
that I've been experimenting with to enable this to act like a full service
easily.

See for 0mq and promise/future: https://blog.pramp.com/inter-thread-communication-in-c-futures-promises-vs-sockets-aeebfffd2107




StimService -

Binds to a socket and subscribes to a controller


Controller 