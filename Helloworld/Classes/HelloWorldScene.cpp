#include "HelloWorldScene.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width / 2,
		origin.y + closeItem->getContentSize().height / 2));

	// add a button to show sth
	auto clickItem = MenuItemImage::create(
		"huaji.png",
		"zhangzhe.png",
		CC_CALLBACK_1(HelloWorld::clickMeCallBack, this));

	clickItem->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + clickItem->getContentSize().height));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

	auto menu2 = Menu::create(clickItem, NULL);
	menu2->setPosition(Vec2::ZERO);
	this->addChild(menu2, 2);
    /////////////////////////////
    // 3. add your codes below...

    // create and initialize a label 
    
    auto label = Label::createWithTTF("Hello Cocos2d-x!", "fonts/SIMYOU.ttf", 24);
	auto nameLabel = Label::createWithTTF("14331401 庄清惠啊", "fonts/SIMYOU.ttf", 24);
    // position the label
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height));
	nameLabel->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - 2*label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);
	this->addChild(nameLabel, 2);

    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("fire.jpg");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void HelloWorld::clickMeCallBack(Ref* pSender) {

}
