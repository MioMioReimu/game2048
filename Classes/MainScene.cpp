#include "MainScene.h"
#include "base/base64.h"
#define PROBALITY_DIM 16
#define MAX_HISTORY 128
#define MAX_SAVE 64
const float probality[PROBALITY_DIM][PROBALITY_DIM] =
{
	{ 0.8, 1.0 },//2,4
	{ 0.7, 1.0 },//2,4
	{ 0.6, 1.0 },//2,4
	{ 0.6, 1.0 },//2,4
	{ 0.5, 0.9, 1.0 },//2,4,8
	{ 0.4, 0.9, 1.0 },//2,4,8
	{ 0.3, 0.8, 1.0 },//2,4,8
	{ 0.2, 0.8, 1.0 },//2,4,8
	{ 0.1, 0.7, 0.9, 1.0 },//2,4,8,16
	{ 0.6, 0.9, 1.0 },//2,4,8,16
	{ 0.4, 0.8, 1.0 },//2,4,8,16
	{ 0.4, 0.7, 1.0 }//2,4,8,16
};
std::stringstream ss;
bool Save::idx_loaded = false;
int Save::save_num = 0;
std::vector<int> Save::save_idxs = std::vector<int>();
void debugOutputPan(int *d)
{
	for (int i = 0; i < DIM; ++i)
		CCLOG("%d\t%d\t%d\t%d\t", d[i*DIM + 0], d[i*DIM + 1], d[i*DIM + 2], d[i*DIM + 3]);
	CCLOG(" ");
}
Save::Save(int idx)
{
	_save_idx = idx;
	_loaded = false;
	_data = new int[DIM*DIM];
	_max = 0;
	_score = 0;
	
}
Save::~Save()
{
	delete _data;
	while (_history_data.size() > 0)
	{
		delete _history_data.back();
		_history_data.pop_back();
	}
		
}
void Save::loadIdx()
{
	int *idxs;
	std::string idxs_base64 = cocos2d::UserDefault::getInstance()->getStringForKey("save_idxs");
	cocos2d::base64Decode((const unsigned char*)idxs_base64.c_str(), idxs_base64.length(), (unsigned char**)&idxs);
	save_num = cocos2d::UserDefault::getInstance()->getIntegerForKey("save_num");
	qsort(idxs, save_num, sizeof(int), [](const void* a, const void *b)-> int{
		return *(int*)a < *(int*)b;
	});
	for (int i = 0; i < save_num; ++i)
	{
		save_idxs.push_back(idxs[i]);
	}
	free(idxs);
	idx_loaded = true;
}
void Save::saveIdx()
{
	if (!idx_loaded)
	{
		loadIdx();
	}
	char * idxs_base64;
	cocos2d::base64Encode((const unsigned char*)save_idxs.data(), save_idxs.size()*sizeof(int), &idxs_base64);
	cocos2d::UserDefault::getInstance()->setIntegerForKey("save_num",save_num);
	cocos2d::UserDefault::getInstance()->setStringForKey("save_idxs", idxs_base64);
	free(idxs_base64);
}
bool Save::hasSave(int save_idx)
{
	if (!idx_loaded)
	{
		loadIdx();
	}
	return std::binary_search(save_idxs.begin(), save_idxs.end(), save_idx);
}
const std::vector<int> Save::getSaveIdxs()
{
	if (!idx_loaded)
	{
		loadIdx();
	}
	return save_idxs;
}
void Save::deleteSave(int save_idx)
{
	if (hasSave(save_idx))
	{
		for (auto it = save_idxs.begin(); it != save_idxs.end(); ++it)
		{
			save_idxs.erase(it);
			break;
		}
		--save_num;
		saveIdx();
		
	}
}
void Save::save()
{
	if (!hasSave(_save_idx))
	{
		++save_num;
		save_idxs.push_back(_save_idx);
		std::sort(save_idxs.begin(), save_idxs.end());
		saveIdx();
	}
	int * save_data = new int[DIM*DIM + 2 + 1 + (2+DIM*DIM)*_history_data.size()];
	save_data[0] = _history_data.size();
	save_data[1] = _score;
	save_data[2] = _max;
	memcpy(save_data + 3 , _data, sizeof(int)*(DIM*DIM));
	memcpy(save_data + 3 +DIM*DIM, _history_score.data(), sizeof(int)*save_data[0]);
	memcpy(save_data + 3+DIM*DIM + save_data[0], _history_max.data(), sizeof(int)*save_data[0]);
	for (int i = 0; i < save_data[0]; ++i)
	{
		memcpy(save_data + 3 + (i + 1)*DIM*DIM + 2 * save_data[0], _history_data.data()[i], DIM*DIM*sizeof(int));
	}
	char *save_data_base64;
	cocos2d::base64Encode((const unsigned char*)save_data, sizeof(int)*(DIM*DIM + 2 + 1 + (2 + DIM*DIM)*save_data[0]) , &save_data_base64);
	ss.clear();
	ss.str("");
	ss << "save"<< _save_idx;
	cocos2d::UserDefault::getInstance()->setStringForKey(ss.str().c_str(), save_data_base64);
	delete save_data;
	free(save_data_base64);
}
bool Save::load()
{
	if (!hasSave(_save_idx))
	{
		return false;
	}
	ss.clear();
	ss.str("");
	ss << "save" << _save_idx;
	auto save_data_base64 = cocos2d::UserDefault::getInstance()->getStringForKey(ss.str().c_str());
	int * save_data;
	cocos2d::base64Decode((const unsigned char*)save_data_base64.data(), save_data_base64.length(), (unsigned char**)(&save_data));
	int his_size = save_data[0];
	_score = save_data[1];
	_max = save_data[2];
	memcpy(_data, save_data + 3, sizeof(int)*DIM*DIM);
	_history_score.resize(his_size);
	for (int i = 0; i < his_size; ++i)
	{
		_history_score[i] = save_data[3 + DIM*DIM +i];
	}
	_history_max.resize(his_size);
	for (int i = 0; i < his_size; ++i)
	{
		_history_max[i] = save_data[3 + DIM*DIM + his_size + i];
	}
	
	for (int i = 0; i < _history_data.size(); ++i)
	{
		memcpy(_history_data[i], save_data + 3 + DIM*DIM + 2 * his_size + i*DIM*DIM, sizeof(int)*DIM*DIM);
		//debugOutputPan(save_data + 3 + DIM*DIM + 2 * his_size + i*DIM*DIM);
	}
	for (int i = _history_data.size(); i < his_size; ++i)
	{
		auto data = new int[DIM*DIM];
		_history_data.push_back(data);
		memcpy(_history_data[i], save_data + 3 + DIM*DIM + 2 * his_size + i*DIM*DIM, sizeof(int)*DIM*DIM);
	}
	free(save_data);
	return true;
}

MainScene::MainScene()
{
	this->data = new int[DIM*DIM];
	_blocks = new cocos2d::Sprite *[DIM*DIM];
	_status = GAME_UNSTARTED;
	_in_menu = false;
}
MainScene::~MainScene()
{
	delete data;
	delete _blocks;
}
void changeFontSize(int data, cocos2d::LabelTTF * l)
{
	if (data < 100)
		l->setFontSize(72);
	else if (data < 1000)
		l->setFontSize(54);
	else if (data < 10000)
		l->setFontSize(44);
	else if (data < 100000)
		l->setFontSize(36);
}
cocos2d::Scene * MainScene::create()
{
	MainScene *ret = new (std::nothrow) MainScene();
		if (ret && ret->init())
		{
			ret->autorelease();
			return ret;
		}
		else
		{
			CC_SAFE_DELETE(ret);
			return nullptr;
		}
}
void MainScene::onEnterTransitionDidFinish()
{
	cocos2d::Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(_menu_layer, true);
}
bool MainScene::init()
{
	//创建UI
	_scene = this;
	_scene->setName("main_scene");
	_scene->retain();
	cocos2d::Layer * layer = cocos2d::LayerColor::create(cocos2d::Color4B(163, 201, 215, 255));
	layer->setName("main_layer");
	_scene->addChild(layer);
	_layer = layer;
	_layer->retain();
	_scene->setOnExitCallback([&]()
	{
		if (_status == GAME_STARTED)
		{
			this->save(0);
			saveScoreBoard();
		}
	});

	cocos2d::Size visibleSize = cocos2d::Director::getInstance()->getVisibleSize();
	cocos2d::Vec2 origin = cocos2d::Director::getInstance()->getVisibleOrigin();
	//创建块背景
	auto block_bg = cocos2d::Sprite::create("bg-720p.png");
	block_bg->setName("block_bg");
	block_bg->setPosition(cocos2d::Vec2(30 + block_bg->getContentSize().width / 2,
		140 + block_bg->getContentSize().height / 2));
	layer->addChild(block_bg);
	block_bg->setPositionZ(0);
	
	//创建每个块
	for (int i = 0; i < DIM; ++i)
	for (int j = 0; j < DIM; ++j)
	{
		_blocks[i*DIM + j] = cocos2d::Sprite::create("block-720p.png");
		ss.clear();
		ss.str("");
		ss << data[i*DIM + j];
		std::string s = ss.str();
		cocos2d::LabelTTF* txt;
		//if (data[i*DIM + j] != 0)
		//	txt = cocos2d::LabelTTF::create(ss.str(), "Arial", 54);
		//else
		txt = cocos2d::LabelTTF::create("", "Arial", 54);
		txt->setName("block_txt");
		txt->setFontFillColor(cocos2d::Color3B(255, 255, 255));
		txt->setPosition(cocos2d::Vec2(_blocks[i*DIM + j]->getContentSize().width*0.5,
			_blocks[i*DIM + j]->getContentSize().height*0.5));
		changeFontSize(data[i*DIM + j], txt);
		_blocks[i*DIM + j]->addChild(txt);
		layer->addChild(_blocks[i*DIM + j]);
		_blocks[i*DIM + j]->setPosition(cocos2d::Vec2(65 + _blocks[i*DIM + j]->getContentSize().width*0.5 + j * 150,
			160 + 10 + _blocks[i*DIM + j]->getContentSize().height*0.5 + i * 150));

	}
	//创建得分板背景
	auto score_bar = cocos2d::Sprite::create("score-bg-720p.png");
	score_bar->setName("score_bar");
	score_bar->setPosition(cocos2d::Vec2(30 + score_bar->getContentSize().width / 2, 150 + block_bg->getContentSize().height
		+ score_bar->getContentSize().height / 2 + 30));
	layer->addChild(score_bar);
	//创建得分板
	_score_label = cocos2d::LabelTTF::create("0", "Arial", 48);
	_score_label->setColor(cocos2d::Color3B(0, 0, 0));
	_score_label->setPosition(cocos2d::Vec2(score_bar->getContentSize().width / 2,
		score_bar->getContentSize().height / 2));
	_score_label->setName("score_label");
	score_bar->addChild(_score_label);
	_score_label->retain();
	//
	auto score_bar_txt = cocos2d::LabelTTF::create("score", "Arial", 50);
	score_bar_txt->setColor(cocos2d::Color3B(0, 0, 0));
	score_bar_txt->setPosition(cocos2d::Vec2(30 + score_bar->getContentSize().width / 2, 150 + 
		block_bg->getContentSize().height
		+ score_bar->getContentSize().height / 2 + 30 + 20 + score_bar_txt->getContentSize().height));
	layer->addChild(score_bar_txt);

	//创建max板背景
	auto max_bar = cocos2d::Sprite::create("score-bg-720p.png");
	max_bar->setName("score_bar");
	max_bar->setPosition(cocos2d::Vec2(30 + max_bar->getContentSize().width / 2, 150 + block_bg->getContentSize().height
		+ max_bar->getContentSize().height *3 / 2 + 100));
	layer->addChild(max_bar);
	//创建max板
	_max_label = cocos2d::LabelTTF::create("0", "Arial", 48);
	_max_label->setColor(cocos2d::Color3B(0, 0, 0));
	_max_label->setPosition(cocos2d::Vec2(max_bar->getContentSize().width / 2,
		max_bar->getContentSize().height / 2));
	_max_label->setName("max_label");
	max_bar->addChild(_max_label);
	_max_label->retain();
	//
	auto max_bar_txt = cocos2d::LabelTTF::create("max", "Arial", 50);
	max_bar_txt->setColor(cocos2d::Color3B(0, 0, 0));
	max_bar_txt->setPosition(cocos2d::Vec2(30 + max_bar->getContentSize().width / 2, 150 +
		block_bg->getContentSize().height
		+ max_bar->getContentSize().height*3 / 2 + 100 + 20 + max_bar_txt->getContentSize().height));
	layer->addChild(max_bar_txt);

	//创建历史最高得分的UI
	auto all_max_score_bar = cocos2d::Sprite::create("score-bg-720p.png");
	all_max_score_bar->setName("max_score_bar");
	all_max_score_bar->setPosition(cocos2d::Vec2(50 + all_max_score_bar->getContentSize().width * 3 / 2,
		150 + block_bg->getContentSize().height + all_max_score_bar->getContentSize().height / 2 + 30));
	layer->addChild(all_max_score_bar);
	_max_score_label = cocos2d::LabelTTF::create("0", "Arial", 48);
	_max_score_label->setColor(cocos2d::Color3B(0, 0, 0));
	_max_score_label->setPosition(cocos2d::Vec2(all_max_score_bar->getContentSize().width / 2,
		all_max_score_bar->getContentSize().height / 2));
	_max_score_label->setName("score_label");
	all_max_score_bar->addChild(_max_score_label);
	_max_score_label->retain();
	auto max_score_bar_txt = cocos2d::LabelTTF::create("history score", "Arial", 50);
	max_score_bar_txt->setColor(cocos2d::Color3B(0, 0, 0));
	max_score_bar_txt->setPosition(cocos2d::Vec2(50 + all_max_score_bar->getContentSize().width * 3 / 2, 150 +
		block_bg->getContentSize().height
		+ all_max_score_bar->getContentSize().height / 2 + 30 + 20 + max_score_bar_txt->getContentSize().height));
	layer->addChild(max_score_bar_txt);

	//创建历史最高得分的UI
	auto all_max_max_bar = cocos2d::Sprite::create("score-bg-720p.png");
	all_max_max_bar->setName("max_max_bar");
	all_max_max_bar->setPosition(cocos2d::Vec2(50 + all_max_max_bar->getContentSize().width * 3 / 2,
		150 + block_bg->getContentSize().height + all_max_max_bar->getContentSize().height*3 / 2 + 100));
	layer->addChild(all_max_max_bar);
	_max_max_label = cocos2d::LabelTTF::create("0", "Arial", 48);
	_max_max_label->setColor(cocos2d::Color3B(0, 0, 0));
	_max_max_label->setPosition(cocos2d::Vec2(all_max_max_bar->getContentSize().width / 2,
		all_max_max_bar->getContentSize().height / 2));
	_max_max_label->setName("max_label");
	all_max_max_bar->addChild(_max_max_label);
	_max_max_label->retain();
	auto max_max_bar_txt = cocos2d::LabelTTF::create("history max", "Arial", 50);
	max_max_bar_txt->setColor(cocos2d::Color3B(0, 0, 0));
	max_max_bar_txt->setPosition(cocos2d::Vec2(50 + all_max_max_bar->getContentSize().width * 3 / 2, 150 +
		block_bg->getContentSize().height
		+ all_max_max_bar->getContentSize().height *3/ 2 + 100 + 20 + max_max_bar_txt->getContentSize().height));
	layer->addChild(max_max_bar_txt);

	//创建重新开始 按钮
	auto restart_button_callback = [&](cocos2d::Ref * pSender)
	{
		restart();
	};

	auto restart_button_item = cocos2d::MenuItemImage::create("restart-720p.png", "restart-pressed-720p.png", 
		"restart-unactive-720p.png", restart_button_callback);
	restart_button_item->setName("restart_button");
	restart_button_item->setPosition(cocos2d::Vec2(40 + restart_button_item->getContentSize().width / 2,
		restart_button_item->getContentSize().height / 2));
	
	
	auto restart_button = cocos2d::Menu::create(restart_button_item, NULL);
	restart_button->setPosition(cocos2d::Vec2::ZERO);
	layer->addChild(restart_button, 1);
	_restart_btn = restart_button;
	_restart_btn->retain();

	//创建undo
	auto undo_button_callback = [&](cocos2d::Ref * pSender)
	{
		undo();
	};
	auto undo_button_item = cocos2d::MenuItemImage::create("undo-720p.png", "undo-pressed-720p.png",
		"undo-unactive-720p.png", undo_button_callback);
	undo_button_item->setName("undo_button");
	undo_button_item->setPosition(cocos2d::Vec2(40 + restart_button_item->getContentSize().width + 
		undo_button_item->getContentSize().width / 2,
		undo_button_item->getContentSize().height / 2));
	undo_button_item->setEnabled(false);
	auto undo_button = cocos2d::Menu::create(undo_button_item, NULL);
	undo_button->setPosition(cocos2d::Vec2::ZERO);
	layer->addChild(undo_button, 1);
	_undo_btn = undo_button;
	_undo_btn->retain();

	//创建ai button
	auto ai_button_item = cocos2d::MenuItemImage::create("ai-720p.png", "ai-pressed-720p.png", [&](cocos2d::Ref *pSender)
	{
		move(getOptimalDirection());
	});
	ai_button_item->setName("ai_button");
	ai_button_item->setPosition(cocos2d::Vec2(40 + restart_button_item->getContentSize().width +
		undo_button_item->getContentSize().width + ai_button_item->getContentSize().width/2, 
		ai_button_item->getContentSize().height / 2));
	auto ai_button = cocos2d::Menu::create(ai_button_item, NULL);
	ai_button->setPosition(cocos2d::Vec2::ZERO);
	layer->addChild(ai_button, 1);

	//创建menubutton
	auto menu_button_item = cocos2d::MenuItemImage::create("menu-720p.png", "menu-pressed-720p.png", [&](cocos2d::Ref *pSender)
	{
		auto menu_layer_fadeto = cocos2d::FadeTo::create(0.3,100);
		menu_layer_fadeto->startWithTarget(_menu_layer);
		_menu_layer->runAction(menu_layer_fadeto);
		for (auto child : _menu_layer->getChildren())
		{
			auto fadeto = cocos2d::FadeTo::create(0.3, 255);
			fadeto->startWithTarget(child);
			child->runAction(fadeto);
		}
		cocos2d::Director::getInstance()->getEventDispatcher()->resumeEventListenersForTarget(_menu_layer, true);
		_in_menu = true;
	});
	menu_button_item->setName("menu_button");
	menu_button_item->setPosition(cocos2d::Vec2(40 + restart_button_item->getContentSize().width +
		undo_button_item->getContentSize().width + +ai_button_item->getContentSize().width +
		menu_button_item->getContentSize().width/2
		, menu_button_item->getContentSize().height / 2));
	auto menu_button = cocos2d::Menu::create(menu_button_item, NULL);
	menu_button->setPosition(cocos2d::Vec2::ZERO);
	layer->addChild(menu_button);

	//创建menu layer
	_menu_layer = cocos2d::LayerColor::create(cocos2d::Color4B(255, 255, 255, 0));
	_menu_layer->setName("menu_layer");
	_scene->addChild(_menu_layer);
	_menu_layer->retain();

	//创建 menu Back Button
	auto menu_back_item1 = cocos2d::MenuItemImage::create("back-720p.png", "back-pressed-720p.png", [&]
		(cocos2d::Ref *pSender)
	{
		auto menu_layer_fadeto = cocos2d::FadeTo::create(0.3, 0);
		menu_layer_fadeto->startWithTarget(_menu_layer);
		_menu_layer->runAction(menu_layer_fadeto);
		for (auto child : _menu_layer->getChildren())
		{
			auto fadeto = cocos2d::FadeTo::create(0.3, 0);
			fadeto->startWithTarget(child);
			child->runAction(fadeto);
			
		}
		cocos2d::Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(_menu_layer, true);
		_in_menu = false;
	});
	menu_back_item1->setName("menu_back");
	menu_back_item1->setPosition(cocos2d::Vec2(visibleSize.width / 2,
		menu_back_item1->getContentSize().height / 2 + visibleSize.height / 2));

	auto menu_back_item = cocos2d::MenuItemLabel::create(cocos2d::LabelTTF::create("Back", "Arial", 54));
	menu_back_item->setColor(cocos2d::Color3B(255, 255, 255));
	menu_back_item->setPosition(cocos2d::Vec2(menu_back_item1->getContentSize().width/2,
		menu_back_item1->getContentSize().height / 2));
	menu_back_item1->addChild(menu_back_item);
	auto menu_back = cocos2d::Menu::create(menu_back_item1,NULL);
	menu_back->setPosition(cocos2d::Vec2::ZERO);
	menu_back->setOpacity(0);
	_menu_layer->addChild(menu_back);

	//创建menu exit button
	auto menu_exit_item = cocos2d::MenuItemImage::create("back-720p.png", "back-pressed-720p.png", [&]
		(cocos2d::Ref *pSender)
	{
		//if (_in_menu)
		cocos2d::Director::getInstance()->end();
		//else
			//cocos2d::Director::getInstance()->getEventDispatcher()->pauseEventListenersForTarget(_menu_layer, true);
	});
	menu_exit_item->setName("menu_exit");
	menu_exit_item->setPosition(cocos2d::Vec2(visibleSize.width / 2,
		menu_exit_item->getContentSize().height / 2 + visibleSize.height / 2-120));

	auto menu_exit_txt = cocos2d::MenuItemLabel::create(cocos2d::LabelTTF::create("Exit", "Arial", 54));
	menu_exit_txt->setColor(cocos2d::Color3B(255, 255, 255));
	menu_exit_txt->setPosition(cocos2d::Vec2(menu_exit_item->getContentSize().width / 2,
		menu_exit_item->getContentSize().height / 2));
	menu_exit_item->addChild(menu_exit_txt);
	auto menu_exit = cocos2d::Menu::create(menu_exit_item, NULL);
	menu_exit->setPosition(cocos2d::Vec2::ZERO);
	menu_exit->setOpacity(0);
	_menu_layer->addChild(menu_exit);

	auto menu_layer_listener = cocos2d::EventListenerTouchOneByOne::create();
	menu_layer_listener->setSwallowTouches(true);
	menu_layer_listener->onTouchBegan = [&](cocos2d::Touch *touch, cocos2d::Event *evt)->bool
	{
		
		return _in_menu;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->
		addEventListenerWithSceneGraphPriority(menu_layer_listener, _menu_layer);
	//this is not useful in TouchEvent;
	
	//创建main layer touch监听
	auto moveListener = cocos2d::EventListenerTouchOneByOne::create();
	moveListener->setSwallowTouches(true);
	moveListener->onTouchBegan = [&](cocos2d::Touch * touch, cocos2d::Event* evt)->bool
	{
		auto target = dynamic_cast<cocos2d::Layer *>(evt->getCurrentTarget());
		
		if (target)
		{
			_move_start_loc = touch->getLocation();
			_move_start = true;
		}
		return true;
	};
	moveListener->onTouchMoved = [&](cocos2d::Touch * touch, cocos2d::Event* evt)
	{

		auto target = dynamic_cast<cocos2d::Layer *>(evt->getCurrentTarget());
		if (target)
		{
			auto loc = touch->getLocation();
			if (_move_start &&loc.getDistance(_move_start_loc) > 100)
			{
				bool produce_ret;
				bool mov_ret = false;
				push_history_data();
				if (loc.x - _move_start_loc.x > 0 && fabs(loc.x - _move_start_loc.x) - 2 * fabs(loc.y - _move_start_loc.y) > 0) //move RIGHT
				{
					mov_ret = this->move(RIGHT);
					_move_start = false;
				}
				else if (loc.x - _move_start_loc.x < 0 && fabs(loc.x - _move_start_loc.x) - 2 * fabs(loc.y - _move_start_loc.y) > 0) //move LEFT
				{
					mov_ret = this->move(LEFT);
					_move_start = false;
				}
				else if (loc.y - _move_start_loc.y > 0 && 2 * fabs(loc.x - _move_start_loc.x) - fabs(loc.y - _move_start_loc.y) < 0)//move UP
				{
					mov_ret = this->move(UP);
					_move_start = false;
				}
				else if (loc.y - _move_start_loc.y < 0 && 2 * fabs(loc.x - _move_start_loc.x) - fabs(loc.y - _move_start_loc.y) < 0)//move DOWN
				{
					mov_ret = this->move(DOWN);
					
					_move_start = false;
				}
				
				if (mov_ret)
				{
					produce(1);
					updateScore();
					bool failed = true;
					for (int pos_x = 0; pos_x < DIM; ++pos_x)
					for (int pos_y = 1; pos_y < DIM; ++pos_y)
					{
						if (data[pos_x + pos_y*DIM] == data[pos_x + (pos_y - 1)*DIM] ||
							data[pos_x + pos_y*DIM] == 0 || data[pos_x + (pos_y - 1)*DIM] == 0)
							failed = false;
					}
					for (int pos_y = 0; pos_y < DIM; ++pos_y)
					for (int pos_x = 1; pos_x < DIM; ++pos_x)
					{
						if (data[pos_x + pos_y*DIM] == data[pos_x - 1 + (pos_y)*DIM] ||
							data[pos_x + pos_y*DIM] == 0 || data[pos_x - 1 + (pos_y)*DIM] == 0)
							failed = false;
					}
					static_cast<cocos2d::MenuItem*>(_undo_btn->getChildByName("undo_button"))->setEnabled(true);
					if (failed)
					{
						gameover();
					}
				}
				else
				{
					pop_history_data();
				}
			}
		}
	};
	moveListener->onTouchEnded = [&](cocos2d::Touch * touch, cocos2d::Event* evt)
	{

	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(moveListener, layer);

	//是否存在默认存档
	if (Save::hasSave(0))
	{
		load(0);
	}
	else
	{
		initdata();
		clear_history_data();
		for (int i = 0; i < DIM*DIM; ++i)
		{
			ProduceEvent evt(i % DIM, i / DIM, data[i]);
			processProduceEvent(evt);
		}
		_status = GAME_STARTED;
	}
	loadScoreBoard();
	return true;
}
void MainScene::restart()
{
	initdata();
	clear_history_data();
	static_cast<cocos2d::MenuItem*>(_undo_btn->getChildByName("undo_button"))->setEnabled(false);
	for (int i = 0; i < DIM*DIM; ++i)
	{
		if (data[i] != 0)
		{
			ss.clear();
			ss.str("");
			ss << data[i];
			static_cast<cocos2d::LabelTTF *>(_blocks[i]->getChildByName("block_txt"))->setString(ss.str());
		}
		else
		{
			static_cast<cocos2d::LabelTTF *>(_blocks[i]->getChildByName("block_txt"))->setString("");
		}
	}
	_score = 0;
	updateScore();
}
bool MainScene::undo()
{
	int * h_d;
	if (_his_data.size() > 0)
	{
		_score = _his_score.back();
		_max = _his_max.back();
		h_d = _his_data.back();
		memcpy(data, h_d, sizeof(int)*DIM*DIM);
		_his_score.pop_back();
		_his_data.pop_back();
		_his_max.pop_back();
		delete h_d;
		for (int i = 0; i < DIM*DIM; ++i)
		{
			if (data[i] != 0)
			{
				ss.clear();
				ss.str("");
				ss << data[i];
				static_cast<cocos2d::LabelTTF *>(_blocks[i]->getChildByName("block_txt"))->setString(ss.str());
			}
			else
			{
				static_cast<cocos2d::LabelTTF *>(_blocks[i]->getChildByName("block_txt"))->setString("");
			}
		}
		updateScore();
		if (_his_data.size() <= 0)
			static_cast<cocos2d::MenuItem*>(_undo_btn->getChildByName("undo_button"))->setEnabled(false);
		return true;
	}
	return false;
}
void MainScene::updateScore()
{
	ss.clear();
	ss.str("");
	ss << _score;

	this->_score_label->setString(ss.str());

	ss.clear();
	ss.str("");
	ss << _max;

	this->_max_label->setString(ss.str());
	if (_score > _all_max_score)
		_all_max_score = _score;
	if (_max > _all_max_max)
		_all_max_max = _max;

	ss.clear();
	ss.str("");
	ss << _all_max_score;
	this->_max_score_label->setString(ss.str());

	ss.clear();
	ss.str("");
	ss << _all_max_max;
	this->_max_max_label->setString(ss.str());
}
cocos2d::Scene * MainScene::getScene()
{
	return _scene;
}

void MainScene::gameover()
{
	_status = GAME_OVER;
	cocos2d::Director::getInstance()->end();
}
void MainScene::push_history_data()
{
	if (_his_data.size() >= MAX_HISTORY)
	{
		for (int i = 0; i < MAX_HISTORY / 2; ++i)
		{
			delete _his_data.front();
			_his_data.pop_front();
			_his_score.pop_front();
			_his_max.pop_front();
		}
	}
	int * new_his_data = new int[DIM*DIM];
	memcpy(new_his_data, data, sizeof(int)*DIM*DIM);
	_his_data.push_back(new_his_data);
	_his_max.push_back(_max);
	_his_score.push_back(_score);
}
void MainScene::clear_history_data()
{
	while (_his_max.size() > 0)
		_his_max.pop_front();
	while (_his_score.size() > 0)
		_his_score.pop_front();
	while (_his_data.size() > 0)
	{
		delete _his_data.front();
		_his_data.pop_front();
	}
}
void MainScene::pop_history_data()
{
	if (_his_data.size() > 0)
	{
		delete _his_data.back();
		_his_data.pop_back();
		_his_score.pop_back();
		_his_max.pop_back();
	}
}
void MainScene::processMoveEvent(const MoveEvent &evt)
{
	std::stringstream ss;
	ss << evt.origin_data + evt.des_data;
	static_cast<cocos2d::LabelTTF *>(_blocks[evt.desy *DIM + evt.desx]->getChildren().at(0))->setString(ss.str());
	changeFontSize(evt.origin_data + evt.des_data,
		static_cast<cocos2d::LabelTTF *>(_blocks[evt.desy *DIM + evt.desx]->getChildren().at(0)));
	static_cast<cocos2d::LabelTTF *>(_blocks[evt.originy *DIM + evt.originx]->getChildren().at(0))->setString("");
}
void MainScene::processProduceEvent(const ProduceEvent &evt)
{
	if (evt.data == 0)
		static_cast<cocos2d::LabelTTF *>(_blocks[evt.posy *DIM + evt.posx]->getChildren().at(0))->setString("");
	else
	{
		std::stringstream ss;
		ss << evt.data;
		static_cast<cocos2d::LabelTTF *>(_blocks[evt.posy *DIM + evt.posx]->getChildren().at(0))->setString(ss.str());
		changeFontSize(evt.data,
			static_cast<cocos2d::LabelTTF *>(_blocks[evt.posy *DIM + evt.posx]->getChildren().at(0)));
	}
}

bool MainScene::move(enum DIRECTION direction)
{
	/*
	 y=DIM
	 |
	 |
	 |
	 |
	 |
	 y=0
	 x=0 ---------> x=DIM          x axs
	 */
	bool flag;
	int step;
	int pos_current;
	bool ret;
	ret = false;
	std::stringstream labelStr;
	switch (direction)
	{
	case LEFT:
		//scan along x axs for each y
		//scan strategy: once an x axs merged, then it will stop scan this x axs, and then  scan next x axs.
		//when scaned all the map , it will scan the map again unless the flag is false.
		// 从左到右逐行扫描， 如果某一行发生合并，则中断扫描该行而扫描下一行。
		// 当所有行都扫描完成，则进行一下一次迭代，除非刚才的扫描中没有任何一行合并。
		for (int pos_y = 0; pos_y < DIM; ++pos_y)
		{
			for (int pos_x = 1; pos_x < DIM; ++pos_x)
			{
				step = 0;
				flag = false;
				if (data[pos_y*DIM + pos_x] != 0)
				{
					for (int k = pos_x - 1; k >= 0; --k)
					{
						if (data[pos_y*DIM + k] == 0)
						{
							++step;
						}
						else if (data[pos_y*DIM + k] == data[pos_y*DIM + pos_x])
						{
							++step;
							flag = true;
							break;
						}
						else
						{
							break;
						}
					}
				}
				//TODO :post move animation from pos_x -> pos_x-step
				// post add animation if flag is true
				if (step != 0)
				{
					ret = true;
					MoveEvent mv_evt(pos_x, pos_y, pos_x - step, pos_y, data[pos_y*DIM + pos_x], data[pos_y*DIM + pos_x - step], flag);
					this->processMoveEvent(mv_evt);
					_score += 2*data[pos_y*DIM + pos_x - step];
					data[pos_y*DIM + pos_x - step] += data[pos_y*DIM + pos_x];
					if (data[pos_y*DIM + pos_x - step] > _max)
						_max = data[pos_y*DIM + pos_x - step];
					data[pos_y*DIM + pos_x] = 0;
				}
			}
		};
		break;
	case RIGHT:
		for (int pos_y = 0; pos_y < DIM; ++pos_y)
		{
			for (int pos_x = DIM - 2; pos_x >= 0; --pos_x)
			{
				step = 0;
				flag = false;
				if (data[pos_y*DIM + pos_x] != 0)
				{
					for (int k = pos_x + 1; k < DIM; ++k)
					{
						if (data[pos_y*DIM + k] == 0)
						{
							++step;
						}
						else if (data[pos_y*DIM + k] == data[pos_y*DIM + pos_x])
						{
							++step;
							flag = true;
							break;
						}
						else
						{
							break;
						}
					}
				}
				//TODO :post move animation from pos_x -> pos_x-step
				// post add animation if flag is true
				if (step != 0)
				{
					ret = true;
					MoveEvent mv_evt(pos_x, pos_y, pos_x + step, pos_y, data[pos_y*DIM + pos_x], data[pos_y*DIM + pos_x + step], flag);
					this->processMoveEvent(mv_evt);
					_score += 2 * data[pos_y*DIM + pos_x + step];
					data[pos_y*DIM + pos_x + step] += data[pos_y*DIM + pos_x];
					if (data[pos_y*DIM + pos_x + step] > _max)
						_max = data[pos_y *DIM + pos_x + step];
					data[pos_y*DIM + pos_x] = 0;
				}
			}
		};
		break;
	case DOWN:
		for (int pos_x = 0; pos_x < DIM; ++pos_x)
		{
			for (int pos_y = 1; pos_y < DIM; ++pos_y)
			{
				step = 0;
				flag = false;
				if (data[pos_y*DIM + pos_x] != 0)
				{
					for (int k = pos_y - 1; k >= 0; --k)
					{
						if (data[k*DIM + pos_x] == 0)
						{
							++step;
						}
						else if (data[k*DIM + pos_x] == data[pos_y*DIM + pos_x])
						{
							++step;
							flag = true;
							break;
						}
						else
						{
							break;
						}
					}
				}
				//TODO :post move animation from pos_y -> pos_y-step
				// post add animation if flag is true
				if (step != 0)
				{
					ret = true;
					MoveEvent mv_evt(pos_x, pos_y, pos_x, pos_y - step, data[pos_y*DIM + pos_x], data[(pos_y - step)*DIM + pos_x], flag);
					this->processMoveEvent(mv_evt);
					_score += 2 * data[(pos_y - step)*DIM + pos_x];
					data[(pos_y - step)*DIM + pos_x] += data[pos_y*DIM + pos_x];
					if (data[(pos_y - step)*DIM + pos_x] > _max)
						_max = data[(pos_y - step)*DIM + pos_x];
					data[pos_y*DIM + pos_x] = 0;
				}
			}
		};
		break;
	case UP:
		for (int pos_x = 0; pos_x < DIM; ++pos_x)
		{
			for (int pos_y = DIM - 2; pos_y >= 0; --pos_y)
			{
				step = 0;
				flag = false;
				if (data[pos_y*DIM + pos_x] != 0)
				{
					for (int k = pos_y + 1; k < DIM; ++k)
					{
						if (data[k*DIM + pos_x] == 0)
						{
							++step;
						}
						else if (data[k*DIM + pos_x] == data[pos_y*DIM + pos_x])
						{
							++step;
							flag = true;
							break;
						}
						else
						{
							break;
						}
					}
				}
				//TODO :post move animation from pos_y -> pos_y-step
				// post add animation if flag is true
				if (step != 0)
				{
					ret = true;
					MoveEvent mv_evt(pos_x, pos_y, pos_x, pos_y + step, data[pos_y*DIM + pos_x], data[(pos_y + step)*DIM + pos_x], flag);
					this->processMoveEvent(mv_evt);
					_score += 2 * data[(pos_y + step)*DIM + pos_x];
					data[(pos_y + step)*DIM + pos_x] += data[pos_y*DIM + pos_x];
					if (data[(pos_y + step)*DIM + pos_x] > _max)
						_max = data[(pos_y + step)*DIM + pos_x];
					data[pos_y*DIM + pos_x] = 0;
				}
			}
		};
		break;
	}
	return ret;
}
void MainScene::initdata()
{
	srand(time(0));
	int x, y;
	memset(data, 0, DIM*DIM*sizeof(int));
	_max = 2;
	_score = 0;
	//data[0] = 0;
	//data[1] = 4;
	//data[2] = 4;
	//data[3] = 2;
	//for (int i = 0; i < 15; ++i)
	//	data[i + 1] = 1 << (i + 1);
	for (int i = 0; i < 2; ++i)
	{
		x = rand() % DIM;
		y = rand() % DIM;
		if (data[y*DIM + x] != 0)
		{
			--i;
			continue;
		}
		else
		{
			int r = rand();
			if (r > RAND_MAX / 4)
			{
				data[y*DIM + x] = 2;
			}
			else
			{
				data[y*DIM + x] = 4;
				_max = 4;
			}
		}
	}
}
bool MainScene::produce(int num)
{
	//棋盘有效位置
	int  valid_pos[DIM*DIM];
	//棋盘中有效位置的个数
	int valid_num = 0;
	//待产生的数字
	int p_data;
	//查找有效位置
	for (int i = 0; i < DIM*DIM; ++i)
	{
		if (data[i] == 0)
			valid_pos[valid_num++] = i;
	}
	//有效位置过少
	if (valid_num < num)
		return false;
	//逐个随机产生数字
	for (int i = 0; i < num; ++i)
	{
		int pos_idx = rand() % valid_num;
		if (-1 == valid_pos[pos_idx])
		{
			--i;
			continue;
		}
		//根据概率表产生数字
		int maxbit = _max / 16;
		float p;
		p = ((double)rand()) / RAND_MAX;
		if (maxbit == 0)
			maxbit = 1;
		maxbit = log2f((float)maxbit);
		int idx;
		for (idx = 0; idx < PROBALITY_DIM; ++idx)
		{
			if (probality[maxbit][idx] >= p)
				break;
		}
		p_data = 2 << idx;
		
		data[valid_pos[pos_idx]] = p_data;
		//TODO: post produce animation
		//TODO: change UI pan
		ProduceEvent evt(valid_pos[pos_idx] % DIM, valid_pos[pos_idx] / DIM, p_data);
		processProduceEvent(evt);
	}
	return true;
}

//保存当前状态到存档并更新所有UI 这里的状态表示棋盘当前状态，以及历史操作
bool MainScene::save(int saveidx)
{
	auto save = new Save(saveidx);
	memcpy(save->_data, data, sizeof(int)*DIM*DIM);
	save->_score = _score;
	save->_max = _max;
	save->_history_score.assign(_his_score.begin(), _his_score.end());
	save->_history_max.assign(_his_max.begin(), _his_max.end());
	for (auto it = _his_data.begin(); it != _his_data.end(); ++it)
	{
		auto d = new int[DIM*DIM];
		memcpy(d, (*it), DIM*DIM*sizeof(int));
		save->_history_data.push_back(d);
	}
	save->save();
	return true;
}
//从存档中加载状态并更新所有UI 这里的状态表示棋盘当前状态，以及历史操作
bool MainScene::load(int saveidx)
{
	if (Save::hasSave(saveidx))
	{
		auto save = new Save(saveidx);
		save->load();
		this->_max = save->_max;
		this->_score = save->_score;
		clear_history_data();
		_his_max.assign(save->_history_max.begin(), save->_history_max.end());
		
		_his_score.assign(save->_history_score.begin(), save->_history_score.end());
		memcpy(data, save->_data, DIM*DIM*sizeof(int));
		for (int i = 0; i < save->_history_data.size(); ++i)
		{
			auto d = new int[DIM*DIM];
			memcpy(d, save->_history_data.at(i), DIM*DIM*sizeof(int));
			_his_data.push_back(d);
		}
		for (int i = 0; i < DIM*DIM; ++i)
		{
			ProduceEvent evt(i % DIM, i / DIM, data[i]);
			processProduceEvent(evt);
		}
		updateScore();
		this->_status = GAME_STARTED;
		if (_his_data.size()>0)
			static_cast<cocos2d::MenuItem*>(_undo_btn->getChildByName("undo_button"))->setEnabled(true);
		else
			static_cast<cocos2d::MenuItem*>(_undo_btn->getChildByName("undo_button"))->setEnabled(false);
	}
	else
		return false;
}

void MainScene::saveScoreBoard()
{
	cocos2d::UserDefault::getInstance()->setIntegerForKey("max_score", _all_max_score);
	cocos2d::UserDefault::getInstance()->setIntegerForKey("max_max", _all_max_max);
}
void MainScene::loadScoreBoard()
{
	_all_max_score = cocos2d::UserDefault::getInstance()->getIntegerForKey("max_score", _all_max_score);
	_all_max_max = cocos2d::UserDefault::getInstance()->getIntegerForKey("max_max", _all_max_max);
	updateScore();
}

//使用AI计算最优方向 
//@maxDepth: 搜索最大深度 默认是5
//@maxMillisecond: 搜索最大时间 默认是INT_MAX
enum MainScene::DIRECTION MainScene::getOptimalDirection(int maxDepth, int maxMillisecond)
{
	return MainScene::DIRECTION::LEFT;
}