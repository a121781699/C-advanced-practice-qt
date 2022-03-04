#include "map.h"
#include"config.h"

Map::Map()
{
	m_map1.load(MAPPATH);
	m_map2.load(MAPPATH);

	m_map1_posY = -GAME_HEIGHT;
	m_map2_posY = 0;

	m_scroll_speed = MAPSCROLLSPEED;
}

void Map::mapPosition()
{
	m_map1_posY += MAPSCROLLSPEED;
	if (m_map1_posY >= 0)
		m_map1_posY = -GAME_HEIGHT;
	m_map2_posY += MAPSCROLLSPEED;
	if (m_map2_posY >= GAME_HEIGHT)
		m_map2_posY = 0;
}
