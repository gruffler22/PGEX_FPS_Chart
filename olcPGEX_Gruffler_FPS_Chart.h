/*

	PGEX Extension for OLC Pixel Game Engine

	What is this?

	This is an extension to the olcPixelGameEngine, which provides the ability to see via a chart overlay the FPS performance on a frame by frame and second by second basis.
	This was made to to see if I was getting FPS spikes.

	How to use?

	Like any other PGEX extension you include the file in your project and include the line:

	  #define OLC_PGEX_Gruffler_FPS_Chart_H

	-once and only once in a .cpp file. 

	Usually this is done in a .cpp file just after including the olcPixelGameEngine itself.
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

	The chart itself rescales dynamically on the last minutes worth of data.
	The individual frames are plotted realtime in the foreground.
	The seconds are summarised as candles in the background.

	Made for my own uses but maybe someone else will find it useful!

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
*/

#pragma once

#ifndef OLC_PGEX_Gruffler_FPS_Chart_H
#define OLC_PGEX_Gruffler_FPS_Chart_H

#include "olcPixelGameEngine.h"

namespace olc
{
	class FPSChart : public olc::PGEX
	{
	public:
		FPSChart() : PGEX(true)
		{
			_show = true;
			_canvas = new olc::Sprite(240, 100);
			_result = new olc::Decal(_canvas);
			_ticksIter = 0;
			for (int i = 0; i < 240; i++)
				_ticks[i] = 0.0f;
			_highestFPS = 0.001f;	//Arbitrary value for now...
			for (int i = 0; i < 60; i++)
				_seconds[i] = { 0.0f, 0.0f, 0.0f, 0.0f };
			_secsIter = 0;
			_secSum = 0.0f;
			_wipInterval = {0.0f, 0.0f, 0.0f, 0.0f};
		}

		~FPSChart()
		{
			if (_canvas)
				delete(_canvas);
			if (_result)
				delete(_result);
		}

		//No copy/assignment
		FPSChart(const FPSChart&) = delete;
		FPSChart& operator = (FPSChart&) = delete;

		//Set whether to show FPS overlay chart or not
		void showChart(bool show)
		{
			_show = show;
		}

		//Get whether the FPS chart is overlaying or not
		bool isChartShowing() const
		{
			return _show;
		}

	private:
		bool _show;
		olc::Sprite* _canvas;
		olc::Decal* _result;
		float _ticks[240];
		int _ticksIter;
		float _highestFPS;
	
		struct interval
		{
			float in = 0.0f;
			float out = 0.0f;
			float high = 0.0f;
			float low = 0.0f;
		};

		interval _seconds[60];
		int _secsIter;
		float _secSum;
		interval _wipInterval;

	protected:
		virtual void OnBeforeUserCreate() 
		{ 
			//Nothing
		}

		virtual void OnAfterUserCreate() 
		{ 
			//Nothing
		}

		virtual void OnBeforeUserUpdate(float& fElapsedTime) 
		{ 
			//record new tick
			float currentFPS = 1.0f / fElapsedTime;
			_ticks[_ticksIter] = currentFPS;
			_ticksIter++;

			//Record highest value
			if (currentFPS > _highestFPS)
				_highestFPS = currentFPS;

			//Cycle tick buffer
			if (_ticksIter >= 240)
			{
				_ticksIter = 0;

				//Check if we can reduce range
				_highestFPS = 0.0f;
				for (int i = 0; i < 240; i++)
					_highestFPS = std::max(_ticks[i], _highestFPS);
				for (int i = 0; i < 60; i++)
					_highestFPS = std::max(_seconds[i].high, _highestFPS);
			}

			_secSum += fElapsedTime;
			_wipInterval.high = std::max(_wipInterval.high, currentFPS);
			_wipInterval.low = std::min(_wipInterval.low, currentFPS);
			while (_secSum > 1.0f)
			{
				_secSum -= 1.0f;
				_wipInterval.out = currentFPS;
				_seconds[_secsIter] = _wipInterval;
				_secsIter++;
				if (_secsIter >= 60)
					_secsIter = 0;
				_wipInterval.in = _wipInterval.high = _wipInterval.low = currentFPS;
			}
		}

		virtual void OnAfterUserUpdate(float fElapsedTime) 
		{
			if (_show)
			{
				//Get existing draw target (to return to this after our shenanagins)
				olc::Sprite* externalDrawTarget = PGEX::pge->GetDrawTarget();

				//clear old data
				PGEX::pge->SetDrawTarget(_canvas);
				PGEX::pge->Clear(olc::VERY_DARK_CYAN);

				//Draw grid
				if (_highestFPS <= 10.0f)
				{
					//One line per 1 FPS
					float targetFPS = 1.0f;

					while (targetFPS <= _highestFPS)
					{
						int y = 100 - int((targetFPS / _highestFPS) * 100.0f);
						//Draw
						PGEX::pge->DrawLine({ 0, y }, { 240, y }, olc::DARK_CYAN);
						PGEX::pge->DrawString({ 0,y }, std::to_string(int(targetFPS)), olc::CYAN);
						//increment fps
						targetFPS += 1.0f;
					}
				}
				else if (_highestFPS <= 50.0f)
				{
					//One line per 5 FPS
					float targetFPS = 5.0f;

					while (targetFPS <= _highestFPS)
					{
						int y = 100 - int((targetFPS / _highestFPS) * 100.0f);
						//Draw
						PGEX::pge->DrawLine({ 0, y }, { 240, y }, olc::DARK_CYAN);
						PGEX::pge->DrawString({ 0,y }, std::to_string(int(targetFPS)), olc::CYAN);
						//increment fps
						targetFPS += 5.0f;
					}
				}
				else if (_highestFPS <= 100.0f)
				{
					//One line per 10 FPS
					float targetFPS = 10.0f;

					while (targetFPS <= _highestFPS)
					{
						int y = 100 - int((targetFPS / _highestFPS) * 100.0f);
						//Draw
						PGEX::pge->DrawLine({ 0, y }, { 240, y }, olc::DARK_CYAN);
						PGEX::pge->DrawString({ 0,y }, std::to_string(int(targetFPS)), olc::CYAN);
						//increment fps
						targetFPS += 10.0f;
					}
				}
				else {
					//One line per 50 FPS
					float targetFPS = 50.0f;

					while (targetFPS <= _highestFPS)
					{
						int y = 100 - int((targetFPS / _highestFPS) * 100.0f);
						//Draw
						PGEX::pge->DrawLine({ 0, y }, { 240, y }, olc::DARK_CYAN);
						PGEX::pge->DrawString({ 0,y }, std::to_string(int(targetFPS)), olc::CYAN);
						//increment fps
						targetFPS += 50.0f;
					}
				}

				//Paint second data
				for (int i = 59; i >= 0; i--)
				{
					interval temp = _seconds[(_secsIter + i)%60];
					
					//Candle wick
					int y1 = 100 - int((temp.low / _highestFPS) * 100.0f);
					int y2 = 100 - int((temp.high / _highestFPS) * 100.0f);

					PGEX::pge->DrawLine({ i * 4 + 2, y1 }, { i * 4 + 2, y2 }, olc::DARK_YELLOW);

					//Candle body
					y1 = 100 - int((temp.in / _highestFPS) * 100.0f);
					y2 = 100 - int((temp.out / _highestFPS) * 100.0f);

					if(y2>y1)
						PGEX::pge->FillRect({ i * 4 + 1 , y1 }, { 3, y2 - y1 }, olc::DARK_RED);
					else
						PGEX::pge->FillRect({ i * 4 + 1 , y2 }, { 3, y1 - y2 }, olc::DARK_GREEN);
				}

				//Paint tick data
				int lastY = 100 - int((_ticks[(_ticksIter + 239) % 240] / _highestFPS) * 100.0f);
				for (int i = 238; i >= 0; i--)
				{
					float t = _ticks[(_ticksIter + i) % 240];
					int y = 100 - int((t / _highestFPS) * 100.0f);
					PGEX::pge->DrawLine({ i + 1,lastY }, { i, y }, olc::YELLOW);
					lastY = y;
				}

				//Update decal
				if (_result)
					delete(_result);
				_result = new olc::Decal(_canvas);

				//Return existing draw target
				PGEX::pge->SetDrawTarget(externalDrawTarget);

				//Paint our chart onto screen
				PGEX::pge->DrawDecal({ 0.0f, 0.0f }, _result, { 1.0f, 1.0f }, olc::Pixel(255,255,255,128));
			}	
		}
	};

}
#endif
