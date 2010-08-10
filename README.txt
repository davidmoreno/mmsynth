Monjemono Synth
===============

Compilation & Install
---------------------

To compile just execute:

$ cmake . && make

You can then execute it directly, change dir to src/ and run:

$ gui/mmsynth

or you can install it:

$ make install

depending on where do you install, it might be necesary too to execute ldconfig
to make the system know that there is a new system library installed.

Finally you run it with:

$ mmsynth

Check out the 'mmsynth --help' to check all available options.

It is possible to force an installation dir with

$ cmake -DCMAKE_INSTALL_PREFIX=/usr/local/
$ make


Usage
-----

You can select a synth to run appending the synth name, and you can force an
audio system or audio backend at runtime. Execute:

$ mmsynth --help

to see more u to date help.

You can add the environment variable DEBUG to show more debug info, usually 
quite usefull if something do not work as expected. Also you can add the
environment variable MSYNTH=<dirofinstall_or_srcdir> to force the use of
some installation. 

It looks for synths and skins at several disk positions. First it looks at
current dir, then at env var MSYNTH and finally at installation dir. There
it looks for the synth or uis directory as needed.


Homepage and feedback
---------------------

Project is hosted at http://code.google.com/p/mmsynth/, if you have any 
sugestion, jus tused it and wanted to say hello, or whatever, write me at 
monjemono (im-at) gmail.com.




Notes about MMSynth internal workings
-------------------------------------

The midi routing is not especially complicated inside program, and should be a 
perfect black box, but a simple explanation follows: 

| GUI <--- SYNTH
|  \\      /^
|     MIDI

as can be seen in the figure, there are three subsystems (Actually 4, as GUI is
synthui and mainwidow), and all are interconnected. MIDI send messages to both 
GUI (through signals) and to Synth (through an event queue sent at chunk 
process petition).  The synth sends the messages to the GUI, these messages are
 for visualization pourposes only, so there is no need for RT processing (*). 
The GUI sends the commands to the MIDI and it enqueues them for the Synth queue,
 and also to the external connected midi periphepals.

At MIDI object, when receivin gmesasges it performs the midi filtering, as the 
controller conversion, for example.

(*) The non RT necesity is not real, as you might want to implement a 
secuencer, for example at the synth, and want to send thenote on precisely to
the other midi gear. anyway this, by the moment is not supported in RT, 
althought it should be pretty fast and should work usually. With RT necesity i
mean it is not optimized at all. 

