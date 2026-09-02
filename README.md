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
- Display for face expressions (most of them are rendered live on device, minimal pre-drawn animations)
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

## Clothing
- I used screenshots and images online to try and get everything as lore-accurate as possible
- I've never properly sewed, or used a sewing machine before so everything was just yoloed, I have no idea what anything was I just brute forced it
### Head and Legs
- A flexible white polyester fleece layer was first glued over the assembled shell to make the base more consistent and to stop any of the shell colours bleed through to the actual clothing layer
- The clothing layer was the same material, 'stretched' over the assembled shell and help down with pins and 3D printed clamps
### Ears
- The ears were separately made and slid over the 3D printed shell like a sock.
- They are not directly attached to the head for easier removable and servicing
- When making, they were sized about 5mm larger to allow for flexing when the ears move.
- I think I could have made the actual ears a bit smaller, they do look a bit more puffy
- The charms and stuff were 3D printed in ABS, smoothed with acetone, painted and then glued + sewed on as required
### Body
> I hate sleeves

- The jacket for the body used non-stretchy polyester suit jacket material
- The actual jacket is 2 pieces and rectangular so that was pretty straightforward
- There is a fleece backing underneath to add bulk
- The sleeves were a pain to make because they needed to allow enough flex for the arms to articulate, but also not look to big
- The 2 pieces of jacket are attached together with a 3D printed 'zip' on the back
- Everything is attached to the body with strategically placed press studs

### Scarf
- This was straightforward, I modelled the shape based on pictures and what looked good
- Once satisfied the shape was cut and the edges sewed
- I used white vinyl heat transfer paper to add the decals with an iron

## Electronics and Interactivity
- I was running out of time so all of this is a bit scuffed but:
### Basic Concept, TLDR
- Everything is powered by battery so no wires to Eous is required
- A host computer (Jetson Nano) on Eous will connect to a Wifi hotspot and provide an asynchronous web-server with basic controls and show battery life
- Any phone connected to the Wifi hotspot can browse the server to select the different modes
- When the host computer detects a mode change, this gets communicated to the main control board (ESP32) which controls the screen and serves animations and motor control
- The main control board is just a basic state machine
- There is a sub control board that controls all the servos (mainly because the main board I have doesn't have enough ports for the PWM servos)

Realistically everything could have been done on a single ESP32, but I was lazy and didn't want integrate it all. Also the modularity let me test each function separately without needing to enable/disable code, making it easier to debug.

### Some Extra Details
#### Auto Gaze Animation
- Eous' eye looking function is all rendered live on the device, there is no pre-drawn animation (I'm very proud of this)
- Eous' ears will also move based on the position of its eyes.
- The program sends the eye drawing function a screen position, and the function will automatically calculate the trajectory, eye position and ear position
- There is a custom bezier curve for smooth point to point transitions (when moving from one eye position to another)
#### Animations
- Currently programmed animations include:
  - Random look-around: Program will randomly choose a gaze position and move Eous' eyes and ears to those positions
  - Eous will blink!
  - Wave mode: One arm will wave and a happy expression appears
  - Praise the sun: Eous' eyes trip out and the pose is made
  - Sleep mode: Eous will look like they are sleeping
#### Battery Life?
- Seemed to be about 2.5 hours with 3x 3.7V 3400mAh LiPo cells in 3S config.

## Takeaways and Future Plans
- This was a very cool project, I learned a lot of things
- I don't ever want to see a sewing machine again
- I want to take it everywhere and add a ton of new features
### Plans and Future Work
- I want to add auto gaze tracking! The Jetson Nano on it is underutilised, I want to slap a camera on it so it can track and interactively play games with people
- I want it to walk around by itself. It's so heavy to carry around.
- Open source??
- Slap a speaker on it

## That's really it lol
Thank you for reaching the end
- Find my actual content [here](https://x.com/k_lef111256) (Twitter)
- And [here](https://youtube.com/@kleffff) (Youtube) 
