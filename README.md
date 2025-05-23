# Sound Player Art Installation

### Description

This project is part of an art instalation and is publicly available to assist with anyone needing to modify or repair the sound player module

This project is in three parts, a main controller board, a speaker, and a six-button module.

The main controller board is populated by an Arduino Pro Micro, a 74HC165 Shift-In Register anda DFPlayer Mini MP3 player module with an SD card.

The six-button module contains six push-buttons pulled to VCC when pressed.  Each buttonis pulled to ground with a 10K on the main controller board.


### Sound Structure

The six buttons each pull up one of the eight shift register pins, changing it from a `1` to a `0`. 

The button matrix specifies the target button (or buttons) and its position corresponds with a MP3 file on the SD card.  Files must be formatted at `####.mp3`.  e.g.  `0001.mp3`, `0002.mp2`, `0003.mp3`, etc.
 

### Schematic
![soundplayer schematic](https://raw.githubusercontent.com/bovercash/soundplayer/refs/heads/main/schematic.svg)
