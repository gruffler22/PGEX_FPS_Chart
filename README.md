# PGEX_FPS_Chart
PGEX Extension for OLC Pixel Game Engine

What is this?

This is an extension to the olcPixelGameEngine, which provides the ability to see via a chart overlay the FPS performance on a frame by frame and second by second basis.
This was made to to see if I was getting FPS spikes.

How to use?

Like any other PGEX extension you include the file in your project and include the line...
  #define OLC_PGEX_Gruffler_FPS_Chart_H
...once and only once in a .cpp file. 
Usually this is done in a .cpp file just before including the olcPixelGameEngine itself.
Eg.
  #define OLC_PGE_APPLICATION
  #include "olcPixelGameEngine.h"

  #define OLC_PGEX_Gruffler_FPS_Chart_H
  #include "olcPGEX_Gruffler_FPS_Chart.h"

To use the extension create an instance of the FPSChart class.
It only has two exposed members:

	//Set whether to show FPS overlay chart or not
	void showChart(bool show)

	//Get whether the FPS chart is overlaying or not
	bool isChartShowing() const

It all should just work in the background (via the olcPixelGameEngine).

Enjoy!



License (OLC-3)
(The licence is inherited from the OneLoneCoder Pixel Game Engine.)

Copyright 2018, 2019, 2020, 2021 OneLoneCoder.com
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions or derivations of source code must retain the above
copyright notice, this list of conditions and the following disclaimer.
2. Redistributions or derivative works in binary form must reproduce
the above copyright notice. This list of conditions and the following
disclaimer must be reproduced in the documentation and/or other
materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its
contributors may be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
