# Chip-8 Script

[![Build Status](https://travis-ci.com/pauwell/chip8-script.svg?branch=master)](https://travis-ci.com/pauwell/chip8-script)


Chip-8 Script is built to provide a thin layer of abstraction over the native chip-8 opcodes.

* :fire: The `easy` way to create chip-8 ROMs.
* :fire: Using variables with meaningful names instead of managing chip-8's registers.
* :fire: You can easily predict to which opcodes your code will compile.
* :fire: Use labels for jumps instead of line numbers.

### Build

```$ export CC=gcc-7```

```$ export CXX=g++-7```

```$ cmake .```

```$ cmake --build .```

### Syntax showcase
```basic
'Define variables.
VAR x = 10
VAR y = x

'For-loop with nested if-condition.
FOR i = 0 TO 10 STEP 1
    IF x == y:
        x += 1
    ENDIF
ENDFOR

'This is how you enter raw opcodes.
RAW "6004"

'This is how you call the built-in functions.
CALL cls 'Clear the screen.
CALL waitkey 'Wait for user input.
CALL readsprite 'Load sprite into memory.
CALL draw 'Render screen.
```
