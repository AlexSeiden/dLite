# dLite
Software to drive a programmable LED dance floor

I built this for my son's 8th birthday. :-)

![alt text](https://github.com/AlexSeiden/dLite/blob/master/app/images/Lewy_dLite.jpg "Lewy's Party!")


## Overview
dLite is an interactive application that drives an LED dance floor with music
visualizations. 

The patterns are generated by what I call “Cues.” The user creates Cues by
connecting nodes in a dataflow visual programming language.

Nodes can have multiple inputs and outputs. Some of these inputs come
explicitly from user input (such as hand-chosen colors), some come directly
from the music (such as using sound amplitude within a specific frequency
range). 


## Architecture

dLite is written in C++, with the Qt framework. The GUI is entirely in Qt; 
I’ve used some of the other Qt classes (such as their container classes) where
it makes things easier.

The software interfaces to the physical LEDs through a UDP socket.

FFTs are provided by the fftreal library, contained in the 3rdparty/fftreal
subdir. The FFT runs in a separate thread.


The system uses the 
[Vamp audio analysis Plugins](https://code.soundsoftware.ac.uk/projects/qm-vamp-plugins/)
for beat detection, song segmentation, etc. 
Right now, this happens externally to the app, but it’s on
the feature list to build this in directly.

Cues are evaluated by starting at their RenderNode, and recursively traversing
the DAG to evaluate all connected nodes. Unconnected nodes are not evaluated.
This fills the frame buffer for every pixel. Multiple cues can write to the
same pixel (each write is called a “firing”), and these are composited together
by the DanceFloor object.

## Code Organization

Here's a guide to the most notable classes:

### Controller objects and general classes

`dLite/app/engine`

|File                     | Notes    
| ----------------------- | ---------
| dancefloor.cpp          | Renders the entire dance floor. 
| firing.cpp              | Decay and compositing of cues on a pixel-by-pixel basis.
| dancefloorhardware.cpp  | Sends pixel data over a socket to the ColorKinetics LEDs
| audioplayback.cpp       | The main callback that handles audio playback, handoff to spectrum analyzer, and calling the dance floor update routine for each refresh. 
| frequencyspectrum.cpp   | Calls the FFT, converts results for use by rest of system
| dispatch.cpp            | provides a “broadcast/subscribe” pattern for Qt signal & slots.
| region.cpp              | Encapsulates an arbitrary bitmap of cells on the dance floor.
| subrange.cpp            | A type that defines a “window” into a spectral analysis
| nodeparameters.cpp      | classes that define the various parameter types for nodes, such as float, color, region, etc. Handles their evaluation, internal connections & serialization.  Some view logic is in here; code cohesion was more important than MVC purity.

### Node classes

These classes define the various types of dataflow nodes that can be instatiated in a DAG for pattern generation.

`dLite/app/nodes`

| File                    | Notes    
| ----------------------- | ---------
| Node.cpp                | Abstract base class for all node classes
| NodeFactory.cpp         | Creates nodes. Tracks all available node types. Handles basics of node serialization, include traversing node graphs.
| RenderNode.cpp          | The abstract base class for all node classes that write to the frame buffer.
| BeatFiles.cpp           | Contains several different types of nodes related to the beat. Can be used to “clock” the transitions of other nodes.
| ColorNodes.cpp          | Various nodes that manipulate and output colors, such as random color selection, HSV manipulation, etc.
| RegionNode.cpp          | Node that defines arbitrary static patterns on the screen, i.e. selections of pixels.  Uses the DanceFloorWidget for interactive editing.
| PathNode.cpp            | Outputs a path along the dance floor as a sequence of x,y positions
| RandomNode.cpp          | Various sorts of random number generators
| SegmentController.cpp   | This node uses a segmentation analysis of the music, based on audio preprocessing, that divides the song up into verse, chorus, bridge, etc. 
| Remap.cpp               | A node that scales and offsets floating point values.
| SublevelNode.cpp        | A node that selects a specified window from the spectral analysis, thresholds it, and outputs a float.
| Shape.cpp               | Several cue classes that draw simple geometric shapes, parameterized by position, size, etc. Uses Qt drawing primitives.
| DotCue.cpp              | The simplest possible shape--an non-antialiased dot on the screen. Useful for debugging because it does not rely on any of the Qt drawing code.


### Viewing classes

GUI widgets & the like.

`dLite/app/views`

| File                    | Notes    
| ----------------------- | ---------
| main_window.cpp         | The central window, where other tool windows can be docked.
| dance_floor_widget.cpp  | Draws the on-screen representation of the dance floor. Used both in playback, and for editing nodes like regions and paths.
| graph_widget.cpp        | Where the user lays out and connects the nodes to form the Cue DAG that renders the patterns.
| node_item.cpp           | Draws nodes in the graph, using the Qt QGraphicsView framework.
| connector_item.cpp      | Class for the connector lines that attach node outputs to node inputs.
| cuesheet_view.cpp       | A cuesheet can contain multiple cues. Individual cue sheets can be tabbed 
| transport.cpp           | The playback "head". Allows scrubbing through the song. Also displays automatic verse/chorus/bridge segmentation.
| spectrograph.cpp        | Draws a real-time frequency spectrum. Adapted from the Qt "spectrum" example.
| group_node_item.cpp     | Allows nodes to be combined into groups, which can then be used as abstractions. (A little bit.)
| node_library.cpp        | Visual interface that allows users to instatiate nodes.

