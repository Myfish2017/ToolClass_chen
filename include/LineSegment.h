#ifndef __LineSegment__
#define  __LineSegment__

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>
#include <map>
#include <functional>
using std::vector;
using cv::Vec4i;
using std::map;


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
bool SubLineSegmentByAngle(const LineSegment &lft, const LineSegment &rgh, double &value)
{
	value = std::min(std::abs(rgh.Angle() - lft.Angle()),
		180 - std::abs(rgh.Angle()) - std::abs(lft.Angle()));
	return true;
}

/*****************************************************************************
* @function name : SubLineSegmentByDistance
* @author : chen
* @date : 2018/1/10 8:53
* @inparam : 2条线段，和距离计算类型
* @outparam : 距离引用
* @last change :
*****************************************************************************/
bool SubLineSegmentParallelByDistance(const LineSegment &lft, const LineSegment &rgh, double &value)
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
	return false;
}


bool SubLineSegmentByDistance(const LineSegment &lft, const LineSegment &rgh, double &value)
{
	if (lft.Slope_existence() == false|| rgh.Slope_existence() == false)
	{
		value = abs(rgh[0] - lft[0]);
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
	}
	return true;
}

/*****************************************************************************
* @function name : MinAvgDistanceIndex
* @author : chen
* @date : 2018/1/10 9:42
* @inparam : 线段_向量
* @outparam : 最能模拟这组线段的线段下标引用
* @last change :
*****************************************************************************/
void MinAvgDistance(const vector<LineSegment> &lineSegment, size_t &index,
	std::function<bool(const LineSegment&, const LineSegment&, double&)> function)
{
	double distance_sum = INT_MAX;
	for (size_t i = 0; i < lineSegment.size(); ++i)
	{
		LineSegment curr = lineSegment[i];
		double curr_sum = 0;
		for (size_t j = 0; j < lineSegment.size(); ++j)
		{
			double value;
			function(curr, lineSegment[j], value);
			curr_sum += value;
		}
		if (curr_sum < distance_sum)
		{
			index = i;
			distance_sum = curr_sum;
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
bool KMeanLineSegment(const vector<LineSegment> &lineSegment_vec, vector<vector<LineSegment>> &result
	, size_t types, std::function<bool(const LineSegment&, const LineSegment&, double&)> function)
{
	if (lineSegment_vec.size() < types)
	{
		types = lineSegment_vec.size();
	}
	vector<vector<LineSegment>> result_container;
	vector<vector<size_t>> types_index_vec;
	vector<size_t> centroid_vec;
	types_index_vec.resize(types);
	result_container.resize(types);
	for (size_t i = 0; i < types; ++i)
	{
		result_container[i].push_back(lineSegment_vec[i]);
		types_index_vec[i].push_back(i);
		centroid_vec.push_back(i);
	}
	bool loop_flag = true;
	while (loop_flag)
	{
		for (size_t i = 0; i < lineSegment_vec.size(); ++i)
		{
			bool centroid_flag = false;
			for (size_t j = 0; j < centroid_vec.size(); ++j)
			{
				if (centroid_vec[j] == i)
				{
					centroid_flag = true;
					break;
				}
			}
			if (centroid_flag == true)
			{
				continue;
			}
			LineSegment lineSegment_curr = lineSegment_vec[i];
			size_t min_distance_index = 0;
			double min_value = INT_MAX;
			double value;
			for (size_t j = 0; j < types; ++j)
			{
				function(lineSegment_curr, result_container[j][0], value);
				if (value < min_value)
				{
					min_value = value;
					min_distance_index = j;
				}
			}
			result_container[min_distance_index].push_back(lineSegment_curr);
			types_index_vec[min_distance_index].push_back(i);
		}
		size_t index = 0;
		loop_flag = false;
		centroid_vec.clear();
		result = result_container;
		for (size_t i = 0; i < types; ++i)
		{
			MinAvgDistance(result_container[i], index, function);
			result_container[i][0] = result_container[i][index];
			types_index_vec[i][0] = types_index_vec[i][index];
			centroid_vec.push_back(types_index_vec[i][index]);
			result_container[i].erase(result_container[i].begin() + 1, result_container[i].end());
			types_index_vec[i].erase(types_index_vec[i].begin() + 1, types_index_vec[i].end());
			if (index != 0)
			{
				loop_flag = true;
			}
		}
	}
	return true;
}
#endif