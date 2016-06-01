#include "Thunder.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include <algorithm>

USING_NS_CC;

using namespace CocosDenshion;

Scene* Thunder::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Thunder::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool Thunder::init() {
    if ( !Layer::init() ) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();

    auto bgsprite = Sprite::create("background.jpg");
    bgsprite->setPosition(visibleSize / 2);
     bgsprite->setScale(visibleSize.width / bgsprite->getContentSize().width, \
         visibleSize.height / bgsprite->getContentSize().height);
    this->addChild(bgsprite, 0);

    player = Sprite::create("player.png");
    player->setPosition(visibleSize.width / 2, player->getContentSize().height + 5);
    player->setName("player");
    this->addChild(player, 1);

	fireButton = Sprite::create("fire.png");
	fireButton->setScale(0.1);
	fireButton->setPosition(visibleSize.width - fireButton->getContentSize().width / 10,
		visibleSize.height / 2 - 50);
	
	this->addChild(fireButton, 2);

    addEnemy(5);

    preloadMusic();
    playBgm();

    addTouchListener();
    addKeyboardListener();
    addCustomListener();

	schedule(schedule_selector(Thunder::update), 0.016f, kRepeatForever, 0);	
	schedule(schedule_selector(Thunder::enemyMove), 0.5f, kRepeatForever, 0);

	move = 0;
	gameOver = false;
    
    return true;
}

void Thunder::preloadMusic() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("music/bgm.mp3");
	audio->preloadEffect("music/explore.wav");
	audio->preloadEffect("music/fire.wav");
}

void Thunder::playBgm() {
	auto audio = SimpleAudioEngine::getInstance();
	audio->playBackgroundMusic("music/bgm.mp3", true);
}

void Thunder::addEnemy(int n) {
    enemys.resize(n * 3);
    for(unsigned i = 0; i < 3; ++i) {
        char enemyPath[20];
        sprintf(enemyPath, "stone%d.png", 3 - i);
        double width  = (visibleSize.width - 20) / (n + 1.0),
               height = visibleSize.height - (50 * (i + 1));
        for(int j = 0; j < n; ++j) {
            auto enemy = Sprite::create(enemyPath);
            enemy->setAnchorPoint(Vec2(0.5, 0.5));
            enemy->setScale(0.5, 0.5);
            enemy->setPosition(width * (j + 1), height);
            enemys[i * n + j] = enemy;
			addChild(enemy);
        }
    }
}

void Thunder::addTouchListener(){
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(Thunder::onTouchBegan, this);
		
	touchListener->onTouchMoved =
		CC_CALLBACK_2(Thunder::onTouchMoved, this);
	touchListener->onTouchEnded =
		CC_CALLBACK_2(Thunder::onTouchEnded, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void Thunder::addKeyboardListener() {
	auto keyBoardListener = EventListenerKeyboard::create();
	keyBoardListener->onKeyPressed =
		CC_CALLBACK_2(Thunder::onKeyPressed, this);
	keyBoardListener->onKeyReleased =
		CC_CALLBACK_2(Thunder::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyBoardListener, player);
}

void Thunder::update(float f) {
	static double count = 0;
	static int dir = 1;
	count += f;
	if (count > 1) { count = 0.0; dir = -dir; }

	// 每个子弹都飞出去
	for (auto it = bullets.begin(); it != bullets.end();) {
		if ((*it) != NULL) {
			(*it)->setPosition((*it)->getPositionX(), (*it)->getPositionY() + 5);
			if ((*it)->getPositionY() > visibleSize.height - 10) {
				(*it)->removeFromParentAndCleanup(true);
				it = bullets.erase(it);
				continue;
			}
		}
		++it;
	}

	// 检测是否发生碰撞 
	for (unsigned i = 0; i < enemys.size(); ++i) {
		if (enemys[i] != NULL) {
			enemys[i]->setPosition(enemys[i]->getPosition() + Vec2(dir, 0));
			for (unsigned j = 0; j < bullets.size(); ++j) {
				if (bullets[j] != NULL && bullets[j]->getPosition().getDistance(enemys[i]->getPosition()) < 30) {
					EventCustom e("meet");
					meetPair = Vec2(i, j);
					e.setUserData(&meetPair);
					_eventDispatcher->dispatchEvent(&e);
					// 删除数组里的数据
					enemys.erase(enemys.begin() + i);
					bullets.erase(bullets.begin() + j);
					--i;
					break;
				}
			}
		}
    }

	// 检测是否越界 
	Vec2 newPosition = player->getPosition() + Vec2(move, 0);
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	if (newPosition.x >= origin.x && newPosition.x <= origin.x + visibleSize.width) {
		player->setPosition(newPosition);
	}
}

void Thunder::enemyMove(float f)
{
	for (unsigned i = 0; i < enemys.size(); ++i) {
		if (enemys[i] != NULL) {
			Vec2 direction = player->getPosition() - enemys[i]->getPosition();
			direction.normalize();   // 标准化为单位向量
			enemys[i]->runAction(MoveBy::create(1.0f, direction * 10));
			if (player->getPosition().getDistance(enemys[i]->getPosition() + direction*10) < 30) {
				EventCustom e("collision");
				_eventDispatcher->dispatchEvent(&e);
			}
		}
	}
}

void Thunder::fire() {
    auto bullet = Sprite::create("bullet.png");
    bullet->setPosition(player->getPosition());
	addChild(bullet);
	bullets.push_back(bullet);
	// 播放发射音效
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/fire.wav", false, 1.0f, 1.0f, 1.0f);
}

void Thunder::meet(EventCustom * event) {
	meetPair = *(Vec2*)(event->getUserData());
	enemys[meetPair.x]->removeFromParentAndCleanup(true);
	bullets[meetPair.y]->removeFromParentAndCleanup(true);
	// 播放爆炸音效
	auto audio = SimpleAudioEngine::getInstance();
	audio->playEffect("music/explore.wav", false, 1.0f, 1.0f, 1.0f);
}

void Thunder::collision(EventCustom * event)
{
	// 飞船被撞，游戏结束 
	TTFConfig ttfConfig;
	ttfConfig.fontFilePath = "fonts/Marker Felt.ttf";
	ttfConfig.fontSize = 36;
	auto score = Label::createWithTTF(ttfConfig, "Good Game!");
	score->setPosition(Vec2(visibleSize.width / 2,
		visibleSize.height - score->getContentSize().height));
	addChild(score, 3);
	gameOver = true;
	unschedule(schedule_selector(Thunder::update));
	unschedule(schedule_selector(Thunder::enemyMove));
}

void Thunder::addCustomListener() {
	// 子弹与陨石碰撞 
	auto meetListener = EventListenerCustom::create("meet", CC_CALLBACK_1(Thunder::meet, this));
	_eventDispatcher->addEventListenerWithFixedPriority(meetListener, 2);
	// 玩家跟陨石碰撞 
	auto collisionListener = EventListenerCustom::create("collision", CC_CALLBACK_1(Thunder::collision, this));
	_eventDispatcher->addEventListenerWithFixedPriority(collisionListener, 1);
}

bool Thunder::onTouchBegan(Touch *touch, Event *event) {
	if (gameOver) return true; // 如果游戏结束，无效

	// 是否按到开火按钮 
	if (fireButton->getBoundingBox().containsPoint(touch->getLocation())) {
		fire();
	}
	else {
		player->setPositionX(touch->getLocation().x);
	}

	return true;
}

void Thunder::onTouchMoved(Touch *touch, Event *unused_event) {
	move = touch->getLocation().x - player->getPositionX();
}

void Thunder::onTouchEnded(Touch *touch, Event *unused_event) {
}

void Thunder::onKeyPressed(EventKeyboard::KeyCode code, Event* event) {
    switch (code) {
        case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_A:
			move -= 5;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        case cocos2d::EventKeyboard::KeyCode::KEY_D:
			move += 5;
            break;
        case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
            fire();
            break;
        default:
            break;
    }
}

void Thunder::onKeyReleased(EventKeyboard::KeyCode code, Event* event) {
	switch (code) {
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_A:
		move += 5;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
	case cocos2d::EventKeyboard::KeyCode::KEY_D:
		move -= 5;
		break;
	default:
		break;
	}
}
