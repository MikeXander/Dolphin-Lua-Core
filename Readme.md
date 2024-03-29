# Dolphin Lua Core + TAStudio (custom Dolphin build)

This project adds Lua support and TAStudio interface in the revision 5.0 of Dolphin Emulator. The Lua API is based on Dragonbane0's Zelda Edition, which can be found [here](https://github.com/dragonbane0/dolphin).

## Lua Core

### Running scripts

To run already implemented Lua scripts, go to `Tools` - `Execute Script`. In the new window, select the desired script (note that only Lua scripts in `Scripts` folder are shown in the list) and click on `Start` whenever you want to execute it. To stop the script execution, click on `Cancel`.

**Important**: Please note that closing the `Execute Script` window does NOT stop the script execution. You have to click on `Cancel` while the desired script is selected to do so.

### Writing new scripts

You can write new scripts following the template of `Example.lua` (or any other implemented script) and save them in `Scripts` folder of the build. Dolphin will automatically recognize them after that.

Available functions:

```
ReadValue8(memoryAddress as Number) //Reads 1 Byte from the address
ReadValue16(memoryAddress as Number) //Reads 2 Byte from the address
ReadValue32(memoryAddress as Number) //Reads 4 Byte from the address
ReadValueFloat(memoryAddress as Number)  //Reads 4 Bytes as a Float from the address
ReadValueString(memoryAddress as Number, length as Number) //Reads "length" amount of characters from the address as a String

WriteValue8(memoryAddress as Number, value as Number) //Writes 1 Byte to the address
WriteValue16(memoryAddress as Number, value as Number) //Writes 2 Byte to the address
WriteValue32(memoryAddress as Number, value as Number) //Writes 4 Byte to the address
WriteValueFloat(memoryAddress as Number, value as Number)  //Writes 4 Bytes as a Float to the address
WriteValueString(memoryAddress as Number, text as String) //Writes the string to the address

GetPointerNormal(memoryAddress as Number) //Reads the pointer address from the memory, checks if its valid and if so returns the normal address. You can use this function for example to get Links Pointer from the address 0x3ad860. To the return value you simply need to add the offset 0x34E4 and then do a ReadValueFloat with the resulting address to get Links speed (in TWW)

SaveState(useSlot as Boolean, slotID/stateName as Number/String) //Saves the current state in the indicated slot number or fileName
LoadState(useSlot as Boolean, slotID/stateName as Number/String) //Loads the state from the indicated slot number or fileName

GetFrameCount() //Returns the current visual frame count. Can use this and a global variable for example to check for frame advancements and how long the script is running in frames

GetInputFrameCount() //Returns the current input frame count

MsgBox(message as String, delayMS as Number) //Dolphin will show the indicated message in the upper-left corner for the indicated length (in milliseconds). Default length is 5 seconds
SetScreenText(message as String) //Displays Text on Screen
RenderText(text, start_x, start_y, colour, size) // Displays custom text on screen. (0,0) is the top left. Colour is the hex code 0xRRGGBB. The regular size is 11.

CancelScript() //Cancels the script
```

Available input/controller functions.
Notes for every function:
- functions that set inputs are ignored during movie playback
- ControllerID coressponds to the controller you want to press inputs
  -   0 to 3 correspond to GameCube Controllers 1 to 4
  -   4 to 7 correspond to Wiimotes 1 to 4
  -   this parameter is optional. If no CID is given (or -1), it will apply the function to all controllers
      -   Ex. PressButton("A") will press A with every conntected controller
- The classic controller extension is currently unsupported because I'm lazy and don't know anyone who TASes with it
- for the IR functions, (0, 0) is the Top Right of the screen
- Currently, functions that have inputs within some range have undefined behaviour when given input outside those ranges

```
function PressButton(Button, ControllerID) end
-- Presses a button for the next input poll
-- returns: nil
-- Button: One of the following strings:
--     "A", "B", "X", "Y", "Z", "L", "ZL", "R", "ZR", "Start", "UP" or "D-Up", "DOWN" or "D-Down", "LEFT" or "D-Left", "RIGHT" or "D-Right", "C", "+", "-", "HOME", "1", "2"
-- If the controller doesn't have the specified button, or button is any other string, the function call will simply be ignored.
-- For GameCube Controllers, "L" and "R" set the shoulder buttons to the maximum value of 255. 


function ReleaseButton(Button, ControllerID) end
-- Ensures a button is not pressed on the next input poll
-- returns: nil


function GetWiimoteKey(ControllerID) end
-- This returns the controller for the current input poll, and a string containing it's Extension decryption key
-- The key will be all 0s if it does not have a wiimote extension
-- returns: ControllerID, Key


function SetIRX(X, ControllerID) end
function SetIRY(Y, ControllerID) end
-- Sets the IR X/Y value of the given controller
-- returns: nil
-- X: An integer between 0 and 1023 (inclusive)
-- Y: An integer between 0 and 1023 (inclusive)


function GetIR(ControllerID) end
-- returns the IR coordinates of the specified controller if the current input poll corresponds to that controller
-- otherwise it returns nil
-- returns: X, Y


function SetAccelX(X, ControllerID) end
function SetAccelY(Y, ControllerID) end
function SetAccelZ(Z, ControllerID) end
-- these set the corresponding accelerometer values for the given wiimote
-- returns: nil
-- X, Y, Z: integers from 0 to 1023 (inclusive)


function GetAccel(ControllerID) end
-- returns the Accelerometer data of the specified controller if the current input poll corresponds to that controller
-- otherwise it returns nil
-- returns: X, Y, Z


function SetNunchukAccelX(X, ControllerID) end
function SetNunchukAccelY(Y, ControllerID) end
function SetNunchukAccelZ(Z, ControllerID) end
-- these set the corresponding accelerometer values for the nunchuk extension of the given wiimote (if it has one)
-- Interpretation note: 512 is 0 acceleration along that axis. Anything less is negative, anything more is positive.
-- returns: nil
-- X, Y, Z: integers from 0 to 1023 (inclusive)


function GetNunchukAccel(ControllerID) end
-- returns the Accelerometer data for the nunchuk extension of the specified controller if the current input poll
-- corresponds to that controller (and if it has a nunchuk extesnion) otherwise it returns nil
-- returns: X, Y, Z


function SetMainStickX(X, ControllerID) end
function SetMainStickY(Y, ControllerID) end
-- these set the X/Y coordinate for the main control stick
-- For wiimotes, this sets the nunchuk stick
-- returns: nil
-- X, Y: integers from 0 to 255 (inclusive)


function SetCStickX(X, ControllerID) end
function SetCStickY(Y, ControllerID) end
-- sets the X/Y coordinate for the secondary stick (C/R-Stick)
-- returns: nil
-- X, Y: integers from 0 to 255 (inclusive)


function SetIRBytes(ControllerID, bytes, ...) end
-- Input as many bytes as you want to overwrite.
-- Note: ControllerID is NOT optional for this function
-- If you attempt to write more bytes than the controller sends, it will write the maximum amount of bytes possible
-- returns: the number of bytes written (for one controller)


function ExampleUsage()
    RenderText("test", 50, 50, 0xFF00FF, 14)

    local WIIMOTE_1 = 4 -- ControllerID of wiimote 1
    local WIIMOTE_2 = 5
    SetScreenText(GetWiimoteKey(WIIMOTE_1)) -- show the extension key

    PressButton("C", WIIMOTE_1) -- Presses the C button (if wiimote 1 has it)

    local x = 0
    local y = 0
    x, y = GetIR(WIIMOTE_1) -- get the current IR
    x = x + 10              -- shift 10 to the left
    y = y + 42              -- shift 42 down
    SetIRX(x)               -- no controller ID so this sets ALL wiimotes
    SetIRY(y)               -- to the same coordinates
    SetIRBytes(WIIMOTE_2, 0xFF, 0xA0) -- set the first 2 bytes of wiimote 2's IR

    -- a function that can change all accel values at once
    function SetAccel(X, Y, Z, ControllerID)
        SetAccelX(X, ControllerID)
        SetAccelY(Y, ControllerID)
        SetAccelZ(Z, ControllerID
    end
    SetAccel(42, 512, 101, WIIMOTE_2)
end
```

Implemented callbacks:

```
function onScriptStart()
    -- called when Start button is pressed
end

function onScriptCancel()
    -- called when Cancel button is pressed or if CancelScript() is executed
end

function onScriptUpdate()
	-- called once every frame
end

function onStateLoaded()
	-- called when a savestate was loaded successfully by the Lua script
end

function onStateSaved()
	-- called when a savestate was saved successfully by the Lua script
end
```

## TAStudio Interface

### How to use it

* To open the TAStudio interface, go to Movie - TAStudio. Once the game is being played, the inputs grid will be populated.
* Hint: if you're using the 5.0 version and the inputs are being registered multiple times each frame, select the "Group by VI" option.
* Savestate before the region you want to modify the inputs.
* Select the inputs you want to modify in the grid. Use the buttons on the side to manipulate them (Hint: clicking with the right mouse button is a shortcut for "Toggle selected inputs").
* To manipulate stick inputs, use the TAS Input sticks at the down-right corner of the interface (the Get button gets the selected stick input from the grid, and the Set button sets the selected frame with the TAS Input stick input).
* Once you want to send the inputs to Dolphin, make sure to check the Read+Write option.

### Implemented features

* Input read/write in real time.
* When savestates are loaded, the inputs before it are populated automatically.
* Button input manipulation (set, clear and toggle).
* Stick input manipulation (TAS Input style).
* Insert blank inputs.
* Copy/Paste inputs.

### TODO

NOTE: If you want to build this version: Use Microsoft Visual Studio 2017 without any upgrades or use Microsoft Visual Studio 2015 Update 2 and Windows 10 SDK 10.0.10586.0
