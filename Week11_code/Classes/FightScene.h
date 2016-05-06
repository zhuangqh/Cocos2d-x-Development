#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <sstream>
using namespace cocos2d;
class FightScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
	void moveEvent(Ref*, char);
	void actionEvent(Ref*, char);
	void stopAc();
	// 更新分数并显示
	void updateScore(int);
	// 随机创建一个怪物
	void createMonster(float);
	// 移除怪物
	void moveMonster(float);
	// 检测角色是否碰到怪物，并播放动画
	void hitByMonster(float);
	// 角色攻击
	int attackMonster();
    // implement the "static create()" method manually
    CREATE_FUNC(FightScene);
	//void update(float dt) override;
private:
	cocos2d::Sprite* player;
	cocos2d::Vector<SpriteFrame*> attack;
	cocos2d::Vector<SpriteFrame*> dead;
	cocos2d::Vector<SpriteFrame*> run;
	cocos2d::Vector<SpriteFrame*> idle;
	cocos2d::Size visibleSize;
	cocos2d::Vec2 origin;
	cocos2d::Label* score;
	float HPbarPercentage;
	int killNum;
	cocos2d::ProgressTimer* pT;
};

#endif // __HELLOWORLD_SCENE_H__
