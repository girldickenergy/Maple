#pragma once

#include <vector>

#include "TaikoManiaObjectType.h"

class TaikoManiaStage
{
private:
	TaikoManiaObjectType m_Type;
	std::vector<int> m_Objects;

public:
	TaikoManiaStage(TaikoManiaObjectType type)
	{
		m_Type = type;
		m_Objects = {};
	}

	~TaikoManiaStage()
	{
		m_Objects.clear();
	}

	TaikoManiaObjectType GetType()
	{
		return m_Type;
	}

	void AddObject(int time)
	{
		m_Objects.push_back(time);
	}

	const std::vector<int>& GetObjects()
	{
		return m_Objects;
	}
};
