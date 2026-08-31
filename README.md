# Eous Build Log
en-nah (have a seat and let me tell you a story)

### Disclaimers
- The content below is a log of the build process of a life-sized version of the Bangboo: Eous from the game Zenless Zone Zero.
- Character design belongs to miHoYo
- This repo does not serve as a complete build guide, but more as a base and general direction, in which you, the reader, may also build one! (mainly because the way I built it is jank as heck)
- All content is provided as-is, use at own risk

## Concept and Features
> Because its cute
I wanted to build something cool because Eous is funny as hell
### Features
- Display for face expressions
- Articulating ears and arms for expressions
- Remotely controlled expressions
- Currently programmed expressions:
  - Randomly look around (and blink)
  - Sleep mode
  - PRAISE THE SUN
  - Wave
### Construction
- 3D printed internal structure
- Fabric exterior
- Removable head
### Electronics
- 4x MG996R 180 degree Servos (1 for each arm + 1 for each ear)
- 1x 96x48px Flexible RGB LED Matrix Screen from [Waveshare](https://www.waveshare.com/rgb-matrix-p2.5-96x48-f.htm)
- 1x ESP32 LED Matrix Driver Board from [Waveshare](https://www.waveshare.com/product/esp32-related/esp32-s3-rgb-matrix.htm)
- 1x ESP32 for servo control
- 1x Deconstructed Jetbot with nVidia Jetson Nano (4GB) for wireless connectivity and power (this is REALLY overkill)
 
## Structure Design
- I found a model I liked on the internet, I think it was [this one](https://makerworld.com/en/models/595629-zenless-zone-zero-bangboo-eous-jue-qu-ling-yi-ai-s#profileId-816838).
  - Using `insert CAD/3D modelling software`, I scaled it to an appropriate size and designed around it
- The overall dimensions are:
  - Main Body Width: `303mm`
  - Height (feet to head): `515mm`
  - Height (feet to ear tip): `660mm`
### Core Design Considerations
The structure's design was done this way in consideration with the following:
#### Screen! and Head
- A flexible LED screen was used to simulate a nice curve over Eous' face (I think I over-flexed the screen, the Red and Blue channels in a corner is not working well anymore)
- The screen is press fitted over the face's fascia with general guides to allow the screen to curve into an acceptable shape
- A tinted plastic sheet is placed over the screen to hide any gaps, plus to soften the LEDs
- The face's surround structure is actually receded in about 20mm, so that the outer fabric can loosely drape over the screen to soften the look
- The head's shell is divided into may pieces to accommodate for the size of the 3D printer (half the build surface doesn't stick and I'm too lazy to replace it)
- Additional strengthening was designed for:
  - The ears: To reduce fatigue as they will move a lot
  - The base of the head: So that I can make the head be tool-lessly removed (twist lock)
#### Ears
- The ears come in 3 major pieces, each piece linked by a hinged system
- A hard, flexible wire (I used 0.9mm MIG welding wire) can be thread through the pieces so that when you push/pull on it, the ears will flex
#### Main Body
- Mainly designed to hold the articulating arms
- The arms articulate with a offsetted gear
- The ends of the arms are formed around the 3D model's shape
- The clamping system for the head is also directly attached to the main body
#### Legs and Lower Body
- This was fairly straightforward
- As with the head, the shell was divided into pieces to fit in my printer
- The lower body directly attaches to the main body
- It is shelled to leave space for electronics, and also provisioned for future revisions, possibly with articulating legs ;)
### Construction
- Everything is held together with bolts, glue and tears.

