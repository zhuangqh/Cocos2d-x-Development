#include "GameScene.h"
#include "LoginScene.h"
#include "json/rapidjson.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include <sstream>
#include <ctime>
#include <regex>
using std::regex;
using std::match_results;
using std::regex_match;
using std::cmatch;
using namespace rapidjson;

USING_NS_CC;

cocos2d::Scene* GameScene::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = GameScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool GameScene::init() {
    if (!Layer::init())
    {
        return false;
    }

    Size size = Director::getInstance()->getVisibleSize();
    visibleHeight = size.height;
    visibleWidth = size.width;

    score_field = TextField::create("Score", "Arial", 30);
    score_field->setPosition(Size(visibleWidth / 4, visibleHeight / 4 * 3));
    this->addChild(score_field, 2);

    submit_button = Button::create();
    submit_button->setTitleText("Submit");
    submit_button->setTitleFontSize(30);
    submit_button->setPosition(Size(visibleWidth / 4, visibleHeight / 4));
	submit_button->addTouchEventListener(CC_CALLBACK_2(GameScene::sumitGrade, this));
    this->addChild(submit_button, 2);

    rank_field = TextField::create("", "Arial", 30);
    rank_field->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4 * 3));
    this->addChild(rank_field, 2);

    rank_button = Button::create();
    rank_button->setTitleText("Rank");
    rank_button->setTitleFontSize(30);
    rank_button->setPosition(Size(visibleWidth / 4 * 3, visibleHeight / 4));
	rank_button->addTouchEventListener(CC_CALLBACK_2(GameScene::getRank, this));
    this->addChild(rank_button, 2);

	exit_button = Button::create();
	exit_button->setTitleText("Exit");
	exit_button->setTitleFontSize(30);
	exit_button->setPosition(Size(visibleWidth / 2, visibleHeight / 2));
	exit_button->addTouchEventListener(CC_CALLBACK_2(GameScene::exit, this));
	this->addChild(exit_button, 2);

	auto name = Label::createWithTTF("My name: " + Global::gameUsername, "fonts/arial.ttf", 30);
	name->setPosition(Size(visibleWidth / 2, visibleHeight / 4 * 3));
	this->addChild(name, 2);
	srand(time(NULL));

    return true;
}

void GameScene::sumitGrade(cocos2d::Ref * ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED)
	{
		HttpRequest* request = new HttpRequest();
		vector<string> headers;
		// set content-type and cookie
		headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
		headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
		log(Global::gameSessionId.c_str());
		request->setHeaders(headers);
		request->setUrl("http://localhost:8080/submit");
		request->setRequestType(HttpRequest::Type::POST);
		request->setResponseCallback(CC_CALLBACK_2(GameScene::onSubmitResponse, this));

		// write the post data
		string postDataStr = "score=" + score_field->getString();
		request->setRequestData(postDataStr.c_str(), postDataStr.size());
		request->setTag("submit");
		HttpClient::getInstance()->send(request);
		request->release();
	}
}

void GameScene::onSubmitResponse(HttpClient * sender, HttpResponse *response)
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
		if (!result) {
			CCLOG("Failed to login: %s\n", d["info"].GetString());
			CCLOG("Please submit again");
		}
	}

	//log(res.c_str());
}

void GameScene::getRank(cocos2d::Ref * ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED)
	{
		HttpRequest* request = new HttpRequest();
		vector<string> headers;
		// set content-type and cookie
		headers.push_back("Content-Type: application/x-www-form-urlencoded; charset=UTF-8");
		headers.push_back("Cookies: GAMESESSIONID=" + Global::gameSessionId);
		request->setHeaders(headers);
		request->setUrl(("http://localhost:8080/rank?top=10&&rand=" + Global::getRandomStr()).c_str());
		request->setRequestType(HttpRequest::Type::GET);
		request->setResponseCallback(CC_CALLBACK_2(GameScene::onGetRankResponse, this));

		request->setTag("GET rank");
		HttpClient::getInstance()->send(request);
		request->release();
	}
}

void GameScene::exit(cocos2d::Ref * ref, Widget::TouchEventType touchType)
{
	if (touchType == Widget::TouchEventType::ENDED)
	{
		Global::saveStatus(NULL, "");
		Director::getInstance()->end();
	}
}

void GameScene::onGetRankResponse(HttpClient * sender, HttpResponse *response)
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
		if (!result) {
			CCLOG("Failed to login: %s\n", d["info"].GetString());
		}
		else {
			setRankBoard(d["info"].GetString());
		}
	}

	
}

void GameScene::setRankBoard(string rankStr)
{
	if (rankStr.empty()) return;

	for (unsigned i = 1; i < rankStr.size(); ++i) {
		if (rankStr[i] == '|') {
			rankStr[i] = '\n';
		}
	}

	rank_field->setString(rankStr.substr(1));
}
