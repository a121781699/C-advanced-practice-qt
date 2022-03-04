#pragma once
#include<qpixmap.h>
class Map
{
public:
	Map();
	void mapPosition();
public:
	QPixmap m_map1;
	QPixmap m_map2;

	int m_map1_posY;
	int m_map2_posY;

	int m_scroll_speed;
};

