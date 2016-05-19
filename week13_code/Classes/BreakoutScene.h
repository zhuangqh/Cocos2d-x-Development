#pragma once

#include "cocos2d.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

class Breakout :public Layer {
public:
	void setPhysicsWorld(PhysicsWorld * world);
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(Breakout);

private:
    //���ּ����벥��
    Sprite* player;
	PhysicsWorld* m_world;
	Size visibleSize;
	Vector<PhysicsBody*> enemys;

    void preloadMusic();
    void playBgm();

    void addBackground();
    void addEdge();
	void addPlayer();

	void addContactListener();
	void addTouchListener();
	void addKeyboardListener();

	void update(float f);

	bool onConcactBegan(PhysicsContact& contact);
    bool onTouchBegan(Touch *touch, Event *unused_event);
    void onTouchMoved(Touch *touch, Event *unused_event);
    void onTouchEnded(Touch *touch, Event *unused_event);
    void onKeyPressed(EventKeyboard::KeyCode code, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode code, Event* event);

	void newEnemys();
	void addEnemy(int type, Point p);
};