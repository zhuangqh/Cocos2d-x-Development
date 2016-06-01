#include "LoginScene.h"
#include "cocostudio/CocoStudio.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "Global.h"
#include "GameScene.h"
#include <regex>
using std::to_string;
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;
USING_NS_CC;

using namespace cocostudio::timeline;

Scene* LoginScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoginScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoginScene::init()
{
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    textField = TextField::create("Player Name", "fonts/arial.ttf", 30);
    textField->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
    this->addChild(textField, 2);

    button = Button::create();
    button->setTitleText("Login");
    button->setTitleFontSize(30);
    button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
	button->addTouchEventListener(CC_CALLBACK_2(LoginScene::onButtonTouch, this));
    this->addChild(button, 2);

	// load data from local store
	Global::loadStatus();
	textField->setString(Global::gameUsername);
	login(Global::gameUsername);
    return true;
}

void LoginScene::onButtonTouch(cocos2d::Ref *ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED)
	{
		login(textField->getString());
	}
}

void LoginScene::onLoginResponse(HttpClient * sender, HttpResponse *response)
{
	if (!response) return;

	if (!response->isSucceed()) {
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}

	string res = Global::toString(response->getResponseData());
	Document d;
	d.Parse<0>(res.c_str());

	if (d.HasParseError()) {
		CCLOG("GetParseError %s\n", d.GetParseError());
	}

	if (d.IsObject() && d.HasMember("result") && d.HasMember("info")) {
		bool result = d["result"].GetBool();
		if (result) {
			Global::saveStatus(response->getResponseHeader(), textField->getString());
			Director::getInstance()->replaceScene(TransitionFade::create(1, GameScene::createScene()));
		}
		else {
			CCLOG("Failed to login: %s\n", d["info"].GetString());
		}
	}
}

void LoginScene::login(string username)
{
	if (!username.empty()) {
		HttpRequest* request = new HttpRequest();
		vector<string> headers;
		// set content-type and cookie
		headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
		request->setHeaders(headers);
		request->setUrl("http://localhost:8080/login");
		request->setRequestType(HttpRequest::Type::POST);
		request->setResponseCallback(CC_CALLBACK_2(LoginScene::onLoginResponse, this));

		// write the post data
		string postDataStr = "username=" + username;
		request->setRequestData(postDataStr.c_str(), postDataStr.size());
		request->setTag("login");
		HttpClient::getInstance()->send(request);
		request->release();
	}
}
