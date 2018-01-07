#ifndef __ClassifyLineSegment__
#define __ClassifyLineSegment__

#include<vector>
#include <climits>
#include <algorithm>
#include "LineSegment.h"

using std::vector;

/*****************************************************************************
* @class name : ClassifyLineSegment
* @author : chen
* @date : 2018/1/4 17:10
* @describe: 基于线段分组的图像车道线识别算法
			1.1、limitAngle_1：2条线段小于这个角度被视为一组
			1.2、limitAngle_2：2条线段距离Y轴的角度之和小于这个值被视为一组
			2.1、max_distance：2条线段小于这个值被视为一组，这个值会自动调整
			2.2、min_distance：max_distance自动调整的下限
			2.3、punishmentAngle：大角度算出的距离往往偏差比小角度算出的距离小，设置的惩罚角度
			2.4、punishmentValue：惩罚角度惩罚值
			3.1、线段抽象的开角设置
* @last change : 
*****************************************************************************/
class ClassifyLineSegment
{
public:
	ClassifyLineSegment(const vector<LineSegment> &LineSegments, int limitAngle_1 = 12, int limitAngle_2 = 12,
		int max_distance = 6, int min_distance = 1,
		int punishmentAngle = 160,double punishmentValue = 3.5, int openAngle = 45)
		:limitAngle_1(limitAngle_1), limitAngle_2(limitAngle_2),
		max_distance(max_distance), min_distance(min_distance),
		punishmentAngle(punishmentAngle),punishmentValue(punishmentValue),
		openAngle(openAngle)
	{
		//将线段分为多类别
		ify_step_1(LineSegments);
		//每一个类别细分为2类
		ify_step_2();
		ify_step_3();
	}
	vector<vector<LineSegment>> Container_step1()const
	{
		return container_step1;
	}
	vector<vector<LineSegment>> Container_step2()const
	{
		return container_step2;
	}
	vector<LineSegment> Container_step3()const
	{
		return container_step3;
	}
	int LimitAngle_1() const { return limitAngle_1; }
	void LimitAngle_1(int val) { limitAngle_1 = val; }
	int LimitAngle_2() const { return limitAngle_2; }
	void LimitAngle_2(int val) { limitAngle_2 = val; }
	int Max_distance() const { return max_distance; }
	void Max_distance(int val) { max_distance = val; }
	int Min_distance() const { return min_distance; }
	void Min_distance(int val) { min_distance = val; }
	int PunishmentAngle() const { return punishmentAngle; }
	void PunishmentAngle(int val) { punishmentAngle = val; }
	double PunishmentValue() const { return punishmentValue; }
	void PunishmentValue(double val) { punishmentValue = val; }
private:
	LineSegment algorthm_for_2(const vector<LineSegment> &vecMyLine) const
	{
		int x_min = INT_MAX, y_min;
		int x_max = INT_MIN, y_max;
		for (int i = 0; i < vecMyLine.size(); ++i)
		{
			if (vecMyLine[i][0] < x_min)
			{
				x_min = vecMyLine[i][0];
				y_min = vecMyLine[i][1];
			}
			if (vecMyLine[i][2] > x_max)
			{
				x_max = vecMyLine[i][2];
				y_max = vecMyLine[i][3];
			}
		}
		return LineSegment(Vec4i(x_min, y_min, x_max, y_max));
	}
	bool cmp_step_1(const LineSegment &lft, const LineSegment &rgh) const
	{
		if (abs(lft.Angle() - rgh.Angle()) < LimitAngle_1())
		{
			return true;
		}
		else if ((90 - abs(lft.Angle()) + 90 - abs(rgh.Angle())) < LimitAngle_2())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool cmp_step_2(const LineSegment &lft, const LineSegment &rgh, int limit) const
	{
		//计算2直线的距离,如果小于5个像素，返回true，否则返回false;
		double jl;
		double a1 = lft.Slope();
		double a2 = rgh.Slope();
		if (lft.Slope_existence() == false || rgh.Slope_existence() == false)
		{
			jl = abs(rgh[0] - lft[0]);
		}
		else if (a1*a2 < 0)//斜率一正一负判断非同类
		{
			std::cout << "步骤二:距离判断为非同线段: 因为斜率一正一负" << std::endl;
			return false;
		}
		else
		{
			double a_ = (a1 + a2) / 2;
			double fz = sqrt(a_*a_ + 1);
			double rgh_intercept = -rgh[1] - a_*rgh[0];
			double lft_intercept = -lft[1] - a_*lft[0];
			jl = abs(rgh_intercept - lft_intercept) / fz;
			//角度比较大的对距离因素影响偏大
			//对之处理
			if ((abs(rgh.Angle()) + abs(lft.Angle())) > PunishmentAngle() && jl > PunishmentValue())
			{
				//角度处理
				std::cout << "步骤二:经过角度处理：因为斜率偏大\n";
				jl -= PunishmentValue();
			}
			//jl = abs(rgh.getIntercept() - lft.getIntercept()) / fz;
		}
		if (jl < limit)
		{
			std::cout << "步骤二:距离判断为同线段:" << jl << "\n";
			return true;
		}
		else
		{
			std::cout << "步骤二:距离判断为非同线段:" << jl << "\n";
			return false;
		}
	}
	//将所有线段划分到多个组别
	void ify_step_1(vector<LineSegment> lineSegments)
	{
		//std::sort(lineSegments.begin(), lineSegments.end(), lineCmp);
		while (lineSegments.size() > 0)
		{
			LineSegment lineSegment_ = lineSegments[lineSegments.size() - 1];
			lineSegments.pop_back();

			vector<LineSegment> lineSegment_tmp_vec;
			lineSegment_tmp_vec.push_back(lineSegment_);

			for (size_t i = 0; i < lineSegments.size();)
			{
				if (cmp_step_1(lineSegment_, lineSegments[i]) == true)
				{
					lineSegment_tmp_vec.push_back(lineSegments[i]);
					lineSegments.erase(lineSegments.begin() + i);
					continue;
				}
				++i;
			}
			container_step1.push_back(lineSegment_tmp_vec);
		}
	}	
	//使每个组别仅仅抽象成只有2条线段
	void ify_step_2()
	{
		for (int i = 0; i < container_step1.size(); ++i)
		{
			int limit = Max_distance();
			while (1)
			{
				vector<LineSegment> lineSegment_vec = container_step1[i];
				if (lineSegment_vec.size() <= 1)
				{
					break;
				}
				vector<LineSegment> lineSegment_vec_1, lineSegment_vec_2, lineSegment_vec_3;
				LineSegment lineSegment;
				lineSegment_vec_1.push_back(lineSegment_vec[0]);
				for (int j = 1; j < lineSegment_vec.size(); ++j)
				{
					if (cmp_step_2(lineSegment_vec[0], lineSegment_vec[j], limit))
					{
						lineSegment_vec_1.push_back(lineSegment_vec[j]);
					}
					else
					{
						lineSegment_vec_2.push_back(lineSegment_vec[j]);
					}
				}
				if (lineSegment_vec_1.size() == 0 || lineSegment_vec_2.size() == 0 && limit > Min_distance())
				{
					--limit;
				}
				else
				{
					if (lineSegment_vec_1.size() == 0 || lineSegment_vec_2.size() == 0)
					{
						break;
					}
					lineSegment = algorthm_for_2(lineSegment_vec_1);
					lineSegment_vec_3.push_back(lineSegment);
					lineSegment = algorthm_for_2(lineSegment_vec_2);
					lineSegment_vec_3.push_back(lineSegment);
					container_step2.push_back(lineSegment_vec_3);
					break;
				}
			}
		}
	}
	//使每个组别由一条线段来描述
	void ify_step_3()
	{
		for (auto i = 0; i < container_step2.size(); ++i)
		{
			int start_x_1, start_y_1, end_x_1, end_y_1;
			int start_x_2, start_y_2, end_x_2, end_y_2;
			int start_x, start_y, end_x, end_y;
			if (container_step2[i][0].Angle() < openAngle && container_step2[i][0].Angle() > -openAngle)
			{
				if (container_step2[i][0][0] < container_step2[i][0][2])
				{
					start_x_1 = container_step2[i][0][0];
					start_y_1 = container_step2[i][0][1];
					end_x_1 = container_step2[i][0][2];
					end_y_1 = container_step2[i][0][3];
				}
				else
				{
					start_x_1 = container_step2[i][0][2];
					start_y_1 = container_step2[i][0][3];
					end_x_1 = container_step2[i][0][0];
					end_y_1 = container_step2[i][0][1];
				}
				if (container_step2[i][1][0] < container_step2[i][1][2])
				{
					start_x_2 = container_step2[i][1][0];
					start_y_2 = container_step2[i][1][1];
					end_x_2 = container_step2[i][1][2];
					end_y_2 = container_step2[i][1][3];
				}
				else
				{
					start_x_2 = container_step2[i][1][2];
					start_y_2 = container_step2[i][1][3];
					end_x_2 = container_step2[i][1][0];
					end_y_2 = container_step2[i][1][1];
				}
			}
			else
			{
				if (container_step2[i][0][1] < container_step2[i][0][3])
				{
					start_x_1 = container_step2[i][0][0];
					start_y_1 = container_step2[i][0][1];
					end_x_1 = container_step2[i][0][2];
					end_y_1 = container_step2[i][0][3];
				}
				else
				{
					start_x_1 = container_step2[i][0][2];
					start_y_1 = container_step2[i][0][3];
					end_x_1 = container_step2[i][0][0];
					end_y_1 = container_step2[i][0][1];
				}
				if (container_step2[i][1][1] < container_step2[i][1][3])
				{
					start_x_2 = container_step2[i][1][0];
					start_y_2 = container_step2[i][1][1];
					end_x_2 = container_step2[i][1][2];
					end_y_2 = container_step2[i][1][3];
				}
				else
				{
					start_x_2 = container_step2[i][1][2];
					start_y_2 = container_step2[i][1][3];
					end_x_2 = container_step2[i][1][0];
					end_y_2 = container_step2[i][1][1];
				}
			}
			start_x = (start_x_1 + start_x_2) / 2;
			start_y = (start_y_1 + start_y_2) / 2;
			end_x = (end_x_1 + end_x_2) / 2;
			end_y = (end_y_1 + end_y_2) / 2;

			LineSegment lineSegment(Vec4i(start_x, start_y, end_x, end_y));
			container_step3.push_back(lineSegment);
		}
	}
	
private:
	vector<vector<LineSegment>> container_step1;
	vector<vector<LineSegment>> container_step2;
	vector<LineSegment> container_step3;
	int limitAngle_1;
	int limitAngle_2;
	int max_distance;
	int min_distance;
	int openAngle;
	int punishmentAngle;
	double punishmentValue;
};

#endif