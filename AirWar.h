#pragma once

#include <QtWidgets/QWidget>
#include<qpushbutton.h>
#include<qsound.h>
#include "ui_AirWar.h"
#include"config.h"
#include"map.h"
#include"HeroPlane.h"
#include"Bullet.h"
#include"enemyplane.h"
#include"bomb.h"
#define WIDTH 512
#define HEIGHT 768
#define AIRWIDHT 100
#define AIRHEIHT 68

class AirWar : public QWidget
{
    Q_OBJECT

public:
    AirWar(QWidget *parent = Q_NULLPTR);

	void reset();
	void initScene();
	void playGame();
	void updatePosition();
	void paintEvent(QPaintEvent *pe);

	Map m_map;
	void timerEvent(QTimerEvent *te);
	void mouseMoveEvent(QMouseEvent *me);
	HeroPlane m_hero;
	Bullet temp_bullet;

	void enemyToScene();
	EnemyPlane m_enemys[ENEMY_NUM];
	int m_recorder;
	void collisionDetection();

	Bomb m_bombs[BOMB_NUM];

	int Py = 0;
	int Py2 = -HEIGHT;
	int airPosY = HEIGHT - AIRHEIHT + 2;
	int airPosX = 50 + AIRWIDHT;

	QPushButton *pushbutton;
	QSound *sound;
private:
    Ui::AirWarClass ui;
};
