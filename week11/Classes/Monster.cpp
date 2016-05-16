#include"Monster.h"
USING_NS_CC;

Factory* Factory::factory = NULL;
Factory::Factory() {
	initSpriteFrame();
}
Factory* Factory::getInstance() {
	if (factory == NULL) {
		factory = new Factory();
	}
	return factory;
}
void Factory::initSpriteFrame(){
	auto texture = Director::getInstance()->getTextureCache()->addImage("Monster.png");
	monsterDead.reserve(4);
	for (int i = 0; i < 4; i++) {
		auto frame = SpriteFrame::createWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(Rect(258-48*i,0,42,42)));
		monsterDead.pushBack(frame);
	}
}

Sprite* Factory::createMonster() {
	Sprite* mons = Sprite::create("Monster.png", CC_RECT_PIXELS_TO_POINTS(Rect(364,0,42,42)));
	monster.pushBack(mons);
	return mons;
}

// 逐个删除并播放死亡动画
void Factory::removeMonster(const Vector<Sprite*>& collision) {
	auto collisionIt = collision.begin();

	for (; collisionIt != collision.end(); collisionIt++) {
		removeMonster(*collisionIt);
	}
}

void Factory::removeMonster(Sprite* mon) {
	auto deadAnimation = Animation::createWithSpriteFrames(monsterDead, 0.1f);
	auto dead = Animate::create(deadAnimation);
	auto seq = Sequence::create(dead, CallFunc::create(CC_CALLBACK_0
	(Sprite::removeFromParent, mon)), NULL);

	mon->runAction(seq);
	monster.eraseObject(mon);
}

void Factory::moveMonster(Vec2 playerPos,float time){
	for (auto mon : monster) {
		Vec2 monsterPos = mon->getPosition();
		Vec2 direction = playerPos - monsterPos;
		direction.normalize();   // 标准化为单位向量
		mon->runAction(MoveBy::create(time, direction * 30));
	}
}

Vector<Sprite*> Factory::collider(Rect rect) {
	Vector<Sprite*> collision;

	for (auto monIt = monster.begin(); monIt != monster.end(); monIt++) {
		if (rect.intersectsRect((*monIt)->getBoundingBox())) {
			collision.pushBack(*monIt);
		}
	}
	return collision;
}
