#pragma once

/*****************************************************************************
* @function name : MinAvgDistanceIndex
* @author : chen
* @date : 2018/1/10 9:42
* @inparam : 蕴含元素的容器，元素距离的定义函数
* @outparam : 最能模拟这类容器的下标引用
* @last change :
*****************************************************************************/
template<typename ElementType>
void MinAvgDistance(const vector<ElementType> &element, size_t &index,
	std::function<bool(const ElementType&, const ElementType&, double&)> function)
{
	double distance_sum = INT_MAX;
	for (size_t i = 0; i < element.size(); ++i)
	{
		ElementType curr = element[i];
		double curr_sum = 0;
		for (size_t j = 0; j < element.size(); ++j)
		{
			double value;
			function(curr, element[j], value);
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
* @describe : K_均值聚类使元素多分,这类k_均值算法的特点是质心是元素的其中一个
* @last change :
*****************************************************************************/
template<typename ElementType>
bool KMeanClassify(const vector<ElementType> &element_vec, vector<vector<ElementType>> &result
	, size_t types, std::function<bool(const ElementType&, const ElementType&, double&)> function)
{
	if (element_vec.size() < types)
	{
		types = element_vec.size();
	}
	vector<vector<ElementType>> result_container;
	vector<vector<size_t>> types_index_vec;
	vector<size_t> centroid_vec;
	types_index_vec.resize(types);
	result_container.resize(types);
	for (size_t i = 0; i < types; ++i)
	{
		result_container[i].push_back(element_vec[i]);
		types_index_vec[i].push_back(i);
		centroid_vec.push_back(i);
	}
	bool loop_flag = true;
	while (loop_flag)
	{
		for (size_t i = 0; i < element_vec.size(); ++i)
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
			ElementType lineSegment_curr = element_vec[i];
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

