#ifndef __LineSegment__
#define  __LineSegment__

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
using std::vector;
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

/*****************************************************************************
* @function name : SubLineSegmentByAngle
* @author : chen
* @date : 2018/1/10 8:21
* @inparam : 2条线段
* @outparam : 1个线段角度差值引用
* @last change :
*****************************************************************************/
void SubLineSegmentByAngle(const LineSegment &lft, const LineSegment &rgh, double &value)
{
	value = std::min(std::abs(rgh.Angle() - lft.Angle()),
		180 - std::abs(rgh.Angle()) - std::abs(lft.Angle()));
}

/*****************************************************************************
* @function name : SubLineSegmentByDistance
* @author : chen
* @date : 2018/1/10 8:53
* @inparam : 2条线段，和距离计算类型
* @outparam : 距离引用
* @last change :
*****************************************************************************/
bool SubLineSegmentByDistance(const LineSegment &lft, const LineSegment &rgh, double &value, int type = 0)
{
	if (type == 0)
	{
		if (lft.Slope_existence() == true)
		{
			if (lft.Slope() != rgh.Slope())
			{
				return false;
			}
			double fz = sqrt(rgh.Slope()*rgh.Slope() + 1);
			double rgh_intercept = -rgh[1] - rgh.Slope()*rgh[0];
			double lft_intercept = -lft[1] - lft.Slope()*lft[0];
			value = abs(rgh_intercept - lft_intercept) / fz;
			return true;
		}
		else
		{
			value = abs(rgh[0] - lft[0]);
			return true;
		}
	}
	else if (type == 1)
	{
		if (lft.Slope_existence() == false || rgh.Slope_existence() == false)
		{
			value = abs(rgh[0] - lft[0]);
			return true;
		}
		else
		{
			/*double angle_ave = (lft.Angle() + rgh.Angle()) / 2;
			double slope_ave = tan(angle_ave);*/
			
			double slope_ave = (lft.Slope() + rgh.Slope()) / 2;
			double fz = sqrt(slope_ave*slope_ave + 1);
			double rgh_intercept = -rgh[1] - slope_ave*rgh[0];
			double lft_intercept = -lft[1] - slope_ave*lft[0];
			value = abs(rgh_intercept - lft_intercept) / fz;
			return true;
		}
	}
	return false;
}

/*****************************************************************************
* @function name : MinAvgDistanceIndex
* @author : chen
* @date : 2018/1/10 9:42
* @inparam : 线段_向量
* @outparam : 最能模拟这组线段的线段下标引用
* @last change : 
*****************************************************************************/
void MinAvgDistance(const vector<LineSegment> &lineSegment,size_t &index)
{
	double distance_sum = INT_MAX;
	for (size_t i = 0; i < lineSegment.size(); ++i)
	{
		LineSegment curr = lineSegment[i];
		double curr_sum = 0;
		for (size_t j = 0; j < lineSegment.size(); ++j)
		{
			double value;
			SubLineSegmentByDistance(curr, lineSegment[j],value);
			curr_sum += value;
		}
		if (curr_sum < distance_sum)
		{
			index = i;
			curr_sum = distance_sum;
		}
	}
}


/*****************************************************************************
* @function name : KMeanLineSegment
* @author : chen
* @date : 2018/1/10 10:18
* @describe : K_均值聚类使线段多分
* @last change : 
*****************************************************************************/
bool KMeanLineSegment(const vector<LineSegment> &lineSegment_vec, vector<vector<LineSegment>> &result, size_t types)
{
	if (lineSegment_vec.size() < types)
	{
		return false;
	}

	return true;
}

#endif