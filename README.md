# MLang
![build status](https://github.com/TheNeverMan/mlang/actions/workflows/c-cpp.yml/badge.svg)  
MLang is a really minimalistic esoteric programming language. Programs are heavily space limited as interpreter accepts them in form of 8x8 bitmaps.

Unlike other esoteric languages which accept images as input, here it is mostly a convension and a somewhat reasonable argument for such a radical space limitation for code (almost like programming a small QR code). Pixels are interpreted linearly from top left corner to down right, no fancy 2D switching of program counter direction or anything like that. There is only basic jump command.

 ---
Lack of program space is main obstacle. Shortest commands (like Print or Ask) are two bytes long (two pixels of bitmap).
This means that absolute maximum command length of any program is 56/2=28 MLang commands.
Abusing random language features and using as many subroutines and loops is pretty much necessary to write anything more complex than truth-machine (this has one funny consequence, not every command uses all of available modes, so every time I write a new program in MLang and I am few bytes short on program space I am tempted to just add new RID command mode to the standard, just for this program).

For example FizzBuzz program (it can be found in `sample-programs`) was made after days of engineering (at first I thought that making it output newlines and making terminate at the same time is impossible).
It uses all 8 variables and 100% of 56 bytes of program space (it only prints `F` and `B` instead of full words, but there is just no space for strings here) and I can say that it is the best program that I have ever written in my life ever.
# Docs
File `docs/docs.txt` contains full language specification and explanation of all commands. It isn't the best specification ever, but to my defense this thing is so simple that it is mostly selfexplanatory.

In `sample-programs` directory there are few sample programs written by me that show (underwhelming) capabilities of this language. By interacting with them you get to experience the amazing bitmap obfuscation feature, that when you encode program to MLang format it is almost impossible to tell what it does at glance or debug it (without carefully writing down it pixel by pixel somewhere else).

Files with `.mli` extension are written in MLang intermediate notation (see next paragraph).

# Interpreter and Toolchain
`src` directory contains code of C interpreter that accepts MLang programs as input. It is called `mlangc` because previously I made a terrible C++ one (the "barely functional product") and this is an attempt to create good and usable one. This interpreter has options to produce warning (unwanted or undefined behaviours that are handled in potentially unexpected way - like returning when stack is empty), treat them as errors, produce verbose debug output or run in step by step mode.

`utilities` directory contains transcoder that converts intermediate notation of MLang to PPM format. Intermediate notation is a simple text notation of MLang programs. Transcoder is written in AWK and it outputs ASCII PPM bitmaps to stdout. They can't be directly fed to interpreter - it expects binary files. `transcode.sh` does the neccesary conversion by converting output PPM to BMP and then again to PPM. It relies on `ppmtobmp` and `bmptoppm`, it also expects `transcoder.awk` in the same working directory.

---
To build interpreter execute `make` in `src` directory. To run interpreter execute `./mlangc`. To view allowed options run `./mlangc -h`.
