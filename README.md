# ArduinoSoundSynthesizer

Designed for the Arduino UNO.

All button pins are active LOW.

Options: 
 * PIN                  : Output pin (default: 2)
 * PIN_RECORD           : Record button pin (default: 11)
 * PIN_SUB              : Subtract button pin (default: 12)
 * PIN_ADD              : Add button pin (default: 13)
 * PINS                 : Keyboard button pins (default: 3 - 10)

Additional Options: 
 * frequencies          : Stores the absolute Hertz frequencies of the notes. Values higher than 2000 Hz may not play accurately.
 * frequencyMultiplier  : Multiplies the frequencies by this value in Hz.
 * frequencyOffset      : Offset the frequencies by this value in Hz.
 * dutyCycle            : Ratio of output being high to output being low.
 * interval             : Minimum amount of time between playing notes.
    * Minimum is about 0.01 for higher notes to play correctly.
    * General formula for whether a note would play correctly: 2 * frequency / interval < clockSpeed

Build Instructions: 
1. Connect PIN to speaker output. 
1. Connect PINS to each keyboard key.
1. Connect PIN 11 - 13 to extra buttons at the top.

Operating Instructions: 
* PIN_RECORD (11) is the toggle button to record songs.
* Press both PIN_ADD (13) and PIN_SUB (12) at the same time to play songs. (default plays 8 notes in order)
* EEPROM saving is automatic when you let go record.
* Hold both PIN_SUB (12) and PIN_RECORD (11) at the same time for three seconds to clear the EEPROM.

Serial Commands: 
 * Basic Commands: 
   * play     : Plays current song in memory
   * playrec  : Alias for play
   * stop     : Stops playing any song
   * stoprec  : Alias for stop
* Toggle Commands: 
   * printoff : Stops printing every note played to serial
   * printon  : Begins printing every note played to serial
   * romoff   : Stops automatic saving to rom
   * romon    : Begins automatic saving to rom
* EEPROM Commands: 
   * saverom  : Saves song in memory to the EEPROM
   * printrom : Prints the contents of the EEPROM to serial
   * clear    : Clears the contents of the EEPROM
   * clearrom : Alias for clear
* Recording Commands: 
   * beginrec : Begins recording to memory
   * rec      : Alias for beginrec
   * record   : Alias for beginrec
   * endrec   : Stops recording to memory
