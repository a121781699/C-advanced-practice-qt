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

	pushbutton = new QPushButton("���¿�ʼ");
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
		painter->drawText(rect(), Qt::AlignCenter, QString("��Ϸ������"));
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
		//�ǿ��ел� ��������
		if (m_enemys[i].m_Free == false)
			m_enemys[i].updatePosition();
	}
	m_map.mapPosition();
	//�����ӵ�
	m_hero.shoot();
	//�����ӵ�����
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
			//�л�����״̬��Ϊfalse
			m_enemys[i].m_Free = false;
			//��������
			m_enemys[i].m_X = rand() % (GAME_WIDTH - m_enemys[i].m_Rect.width());
			m_enemys[i].m_Y = -m_enemys[i].m_Rect.height();
			break;
		}
	}
}
void AirWar::collisionDetection()
{
	//�������зǿ��еĵл�
	for (int i = 0; i < ENEMY_NUM; i++)
	{
		if (m_enemys[i].m_Free)
			//���зɻ� ��ת��һ��ѭ��
			continue;
		//�������� �ǿ��е��ӵ�
		for (int j = 0; j < BULLET_NUM; j++)
		{
			if (m_hero.m_bullets[j].m_Free)
				//�����ӵ� ��ת��һ��ѭ��
				continue;
			//����ӵ����ο�͵л����ο��ཻ��������ײ��ͬʱ��Ϊ����״̬����
			if (m_enemys[i].m_Rect.intersects(m_hero.m_bullets[j].m_Rect))
			{
				sound->play();
				m_enemys[i].m_Free = true;
				m_hero.m_bullets[j].m_Free = true;
				for (int k = 0; k < BOMB_NUM; k++) {
					if (m_bombs[k].m_Free) 
					{ //��ը״̬����Ϊ�ǿ��� 
						m_bombs[k].m_Free = false; 
						//��������
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
					{ //��ը״̬����Ϊ�ǿ��� 
						m_bombs[k].m_Free = false; 
						//��������
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