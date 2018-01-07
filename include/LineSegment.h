#ifndef __LineSegment__
#define  __LineSegment__

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using cv::Vec4i;

/*****************************************************************************
* @class name : LineSegment
* @author : chen
* @date : 2018/1/4 12:09
* @describe: 封装一个线段的意义 
* @last change :
*****************************************************************************/
class LineSegment
{
public:
	LineSegment(const Vec4i &vec4i) :vec4i(vec4i)
	{
		calInside();
	}
	LineSegment() = default;
	~LineSegment()
	{
	}
	double Angle() const { return angle; }
	void Angle(double val) { angle = val; }
	double Slope() const { return slope; }
	void Slope(double val) { slope = val; }
	bool Slope_existence() const { return slope_existence; }
	void Slope_existence(bool val) { slope_existence = val; }
	bool Intercep_existence() const { return intercep_existence; }
	void Intercep_existence(bool val) { intercep_existence = val; }
	double Intercept() const { return intercept; }
	void Intercept(double val) { intercept = val; }
public:
	const int& operator [] (int index) const
	{
		return vec4i[index];
	}
	int& operator [] (int index)
	{
		return vec4i[index]; 
	}
private:
	void calInside() {
		if (vec4i[2] == vec4i[0])
		{
			Slope_existence(false);
			Intercep_existence(false);
			Angle(90);
			return;
		}
		else
		{
			Slope((-vec4i[3] + vec4i[1]) * 1.0 / (vec4i[2] - vec4i[0]));
			Intercept(-vec4i[1] - Slope()*vec4i[0]);
			Angle(atan(Slope()) / 3.14 * 180);
		}
	}
private:
	Vec4i vec4i;
	bool slope_existence = true;
	bool intercep_existence = true;
	double angle = 0;
	double slope = 0;
	double intercept = 0;
};

bool lineCmp(const LineSegment &lft, const LineSegment &rgh)
{
	return lft.Angle() < rgh.Angle();
}

#endif