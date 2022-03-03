#include "AirWar.h"
#include<qicon.h>
#include<qpainter.h>
#include<qevent.h>
#include<qmenubar.h>
#pragma execution_character_set("utf-8")

AirWar::AirWar(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

	initScene();
	playGame();

	pushbutton = new QPushButton("重新开始");
	pushbutton->setParent(this);
	pushbutton->setFixedSize(100, 50);
	pushbutton->move(this->width()*0.5 - pushbutton->width()*0.5, this->height()*0.5 + pushbutton->height());
	pushbutton->hide();
	connect(pushbutton, &QPushButton::clicked, [=]() {
		reset();
		pushbutton->hide();
	});
}
void AirWar::reset()
{
	m_hero.reset();
	temp_bullet.reset();
	for (int i = 0; i < ENEMY_NUM; i++)
		m_enemys[i].reset();
	for (int i = 0; i < BOMB_NUM; i++)
		m_bombs[i].reset();
}
void AirWar::initScene()
{
	srand((unsigned int)time(NULL));
	m_recorder = 0;
	setFixedSize(GAME_WIDTH, GAME_HEIGHT);
	setWindowTitle(GAME_TITLE);
	setWindowIcon(QIcon(GAME_ICON));
}
void AirWar::playGame()
{
	startTimer(GAME_RATE);
	sound = new QSound(":/img/bomb.wav");
	sound->setParent(this);
}
void AirWar::paintEvent(QPaintEvent *pe)
{
	QPainter *painter = new QPainter(this);

	painter->drawPixmap(0, m_map.m_map1_posY, m_map.m_map1);
	painter->drawPixmap(0, m_map.m_map2_posY, m_map.m_map2);
	if (!m_hero.m_Free)
		painter->drawPixmap(m_hero.m_X, m_hero.m_Y, m_hero.m_Plane);
	else
	{
		painter->drawPixmap(m_hero.m_X, m_hero.m_Y, m_bombs[0].m_pixArr[m_bombs[0].m_index]);
		painter->setPen(Qt::red);
		painter->setFont(QFont("Arial", 30));
		painter->drawText(rect(), Qt::AlignCenter, QString("游戏结束！"));
		pushbutton->show();
		return;
	}

	for (int i = 0; i < BULLET_NUM; i++)
		if (!m_hero.m_bullets[i].m_Free)
			painter->drawPixmap(m_hero.m_bullets[i].m_X, m_hero.m_bullets[i].m_Y, m_hero.m_bullets[i].m_Bullet);

	for (int i = 0; i < ENEMY_NUM; i++)
		if (!m_enemys[i].m_Free)
			painter->drawPixmap(m_enemys[i].m_X, m_enemys[i].m_Y, m_enemys[i].m_enemy);

	for (int i = 0; i < BOMB_NUM; i++)
		if (!m_bombs[i].m_Free)
			painter->drawPixmap(m_bombs[i].m_X, m_bombs[i].m_Y, m_bombs[i].m_pixArr[m_bombs[i].m_index]);
}
void AirWar::timerEvent(QTimerEvent *te)
{
	enemyToScene();
	updatePosition();
	//temp_bullet.m_Free = false;
	//temp_bullet.updatePosition();
	update();
	collisionDetection();
}
void AirWar::updatePosition()
{
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		//非空闲敌机 更新坐标
		if (m_enemys[i].m_Free == false)
			m_enemys[i].updatePosition();
	}
	m_map.mapPosition();
	//发射子弹
	m_hero.shoot();
	//计算子弹坐标
	for (int i = 0; i < BULLET_NUM; i++)
		if (!m_hero.m_bullets[i].m_Free)
			m_hero.m_bullets[i].updatePosition();

	for (int i = 0; i < BOMB_NUM; i++)
		if (m_bombs[i].m_Free == false)
			m_bombs[i].updateInfo();
}
void AirWar::mouseMoveEvent(QMouseEvent *me)
{
	int x = me->x() - m_hero.m_Rect.width()*0.5;
	int y = me->y() - m_hero.m_Rect.height()*0.5;

	if (x <= 0)
		x = 0;
	if (x >= GAME_WIDTH - m_hero.m_Rect.width())
		x = GAME_WIDTH - m_hero.m_Rect.width();
	if (y <= 0)
		y = 0;
	if (y >= GAME_HEIGHT - m_hero.m_Rect.height())
		y = GAME_HEIGHT - m_hero.m_Rect.height();
	m_hero.setPosition(x, y);
}
void AirWar::enemyToScene()
{
	m_recorder++;
	if (m_recorder < ENEMY_INTERVAL)
		return;
	m_recorder = 0;
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (m_enemys[i].m_Free)
		{
			//敌机空闲状态改为false
			m_enemys[i].m_Free = false;
			//设置坐标
			m_enemys[i].m_X = rand() % (GAME_WIDTH - m_enemys[i].m_Rect.width());
			m_enemys[i].m_Y = -m_enemys[i].m_Rect.height();
			break;
		}
	}
}
void AirWar::collisionDetection()
{
	//遍历所有非空闲的敌机
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (m_enemys[i].m_Free)
			//空闲飞机 跳转下一次循环
			continue;
		//遍历所有 非空闲的子弹
		for (int j = 0; j < BULLET_NUM; j++)
		{
			if (m_hero.m_bullets[j].m_Free)
				//空闲子弹 跳转下一次循环
				continue;
			//如果子弹矩形框和敌机矩形框相交，发生碰撞，同时变为空闲状态即可
			if (m_enemys[i].m_Rect.intersects(m_hero.m_bullets[j].m_Rect))
			{
				sound->play();
				m_enemys[i].m_Free = true;
				m_hero.m_bullets[j].m_Free = true;
				for (int k = 0; k < BOMB_NUM; k++) {
					if (m_bombs[k].m_Free) 
					{ //爆炸状态设置为非空闲 
						m_bombs[k].m_Free = false; 
						//更新坐标
						m_bombs[k].m_X = m_enemys[i].m_X;
						m_bombs[k].m_Y = m_enemys[i].m_Y;
						break;
					}
				}
			}
			if (m_hero.m_Rect.intersects(m_enemys[i].m_Rect))
			{
				for (int k = 0; k < BOMB_NUM; k++) {
					if (m_bombs[k].m_Free) 
					{ //爆炸状态设置为非空闲 
						m_bombs[k].m_Free = false; 
						//更新坐标
						m_bombs[k].m_X = m_hero.m_X;
						m_bombs[k].m_Y = m_hero.m_Y;
						break;
					}
				}
				m_hero.m_Free = true;
			}
		}
	}
}