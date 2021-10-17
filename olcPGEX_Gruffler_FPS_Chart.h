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

		void showChart(bool show)
		{
			_show = show;
		}

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
			void debugPrint()
			{
				printf("In: %f Out: %f High: %f Low: %f\n", in, out, high, low);
			}
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
				_seconds[_secsIter].debugPrint();
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
