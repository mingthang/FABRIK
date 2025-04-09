#include "Mesh2D.h"

void Mesh2D::SetName(const std::string& name)
{
	m_name = name;
}

const std::string& Mesh2D::GetName() const
{
	return m_name;
}

int32_t Mesh2D::GetIndexCount() const
{
	return indexCount;
}