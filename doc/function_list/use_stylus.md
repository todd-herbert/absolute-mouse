# Function list - `USE_STYLUS`

`<>` indicates an optional argument

## begin
#### `begin( <width>, <height>)`
Begin USB communications. Passing `<width>` and `<height>` sets the dimensions of the screen, in pixels, in mm, or any other system you find convenient.

## set_dimensions
#### `set_dimensions( width, height)`
Sets the dimensions of the screen, in pixels, in mm, or any other system you find convenient.


## click
#### `click( x, y )`
Perform a single click at (x, y).

## right_click
#### `right_click( x, y )`
Peform a right click at (x, y).

## double_click
#### `double_click( x, y )`
Peform a double click at (x, y).

## long_press
#### `long_press( x, y, duration)`
Press, at point (x, y), or wherever the cursor is currently located, for "duration" in milliseconds.


## drag
#### `drag ( from_x, from_y, to_x, to_y, duration )`
Perform a click and drag maneuver, with duration in ms. Minimum duration 100ms

## hold
#### `hold( x, y )`
Press and hold indefinitely at (x, y).

## travel
#### `travel ( from_x, from_y, to_x, to_y, duration )`
Gradually move the cursor from point to point, with duration in ms. Minimum duration 100ms. Useful with `hold` and `release`.

## release
#### `release ()`
Stop an indefinite press started by `hold`.

## blink
#### `blink ( x, y )`
Force the cursor to briefly appear and dissapear. As `USE_STYLUS` tends not to display a visible cursor, this function is useful in the DevKit, for figuring out co-ordinate values.