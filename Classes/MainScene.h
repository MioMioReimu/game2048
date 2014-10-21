#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__
#include "cocos2d.h"
//#include "ccLayerColorOpacityPropagated.h"
#define DIM 4
class MainScene;

class MoveEvent
{
public:
	MoveEvent(int originx, int originy, int desx, int desy, int origin_data, int des_data, bool b_add) :
		originx(originx), originy(originy), desx(desx), desy(desy), origin_data(origin_data), des_data(des_data), b_add(b_add){};
public:
	int originx;
	int originy;
	int desx;
	int desy;
	int origin_data;
	int des_data;
	bool b_add;
};
class ProduceEvent
{
public:
	ProduceEvent(int posx, int posy, int data):
		posx(posx), posy(posy), data(data)
	{};
public:
	int posx;
	int posy;
	int data;
};
class Save
{
public:
	Save(int idx);
	~Save();
	static bool hasSave(int save_idx);
	static const std::vector<int> getSaveIdxs();
	void save();
	bool load();
	static void deleteSave(int save_idx);
public:
	int _save_idx;
	int _max;
	int _score;
	int *_data;
	std::vector<int *> _history_data;
	std::vector<int> _history_score;
	std::vector<int> _history_max;
	bool _loaded;
	static bool idx_loaded;
	static int save_num;
	static std::vector<int> save_idxs;
private:
	static void saveIdx();
	static void loadIdx();
};

class MainScene:public cocos2d::Scene
{
public:
	static cocos2d::Scene* create();
	virtual void onEnterTransitionDidFinish() override;
	MainScene();
	~MainScene();
	cocos2d::Scene * getScene();
	cocos2d::Sprite ** getBlocks(){ return _blocks; };
	int * getdata() { return data;};
	virtual bool init();
	enum DIRECTION
	{
		LEFT,
		RIGHT,
		UP,
		DOWN
	};
	enum GAMESTATUS
	{
		GAME_UNSTARTED,
		GAME_STARTED,
		GAME_OVER,
	};
	//执行移动操作 更新棋盘UI
	bool move(enum DIRECTION direction);
	//根据现有棋盘随机产生新的数字，num是产生的数字个数
	//如果棋盘不足以产生，则返回false
	//更新棋盘UI
	bool produce(int num = 1);
	//更新scoreUI
	void updateScore();
	//gameover处理
	void gameover();
	//重新开始  更新棋盘UI
	void restart();
	//撤销  更新棋盘UI
	bool undo();
	//从存档中加载状态并更新所有UI 这里的状态表示棋盘当前状态，以及历史操作
	bool save(int saveidx);
	//保存当前状态到存档并更新所有UI 这里的状态表示棋盘当前状态，以及历史操作
	bool load(int saveidx);

	//将当前的棋盘状态push到历史操作中
	void push_history_data();
	//清空所有历史操作
	void clear_history_data();
	//删除最后一步的历史操作
	void pop_history_data();
private:
	//初始化2048盘的数据 1/4的几率是4，3/4的几率是2 初始化时4个数字
	void initdata();
	
	//执行Move动作的UI更新 包括动画
	void processMoveEvent(const MoveEvent &evt);
	//执行新产生的UI更新 
	void processProduceEvent(const ProduceEvent &evt);
	//2048盘状态
	int *data;
	//
	// 2048盘的最大值
	int _max;
	//得分
	int _score;
	cocos2d::Scene * _scene;
	cocos2d::Layer * _layer;
	cocos2d::Sprite ** _blocks;
	cocos2d::Menu * _restart_btn;
	cocos2d::Menu * _undo_btn;
	cocos2d::LabelTTF * _score_label;
	cocos2d::LabelTTF * _max_label;
	cocos2d::LabelTTF * _max_score_label;
	cocos2d::LabelTTF * _max_max_label;
	cocos2d::Vec2 _move_start_loc;
	bool _move_start;

	cocos2d::Layer * _menu_layer;
	
	enum GAMESTATUS _status;
	std::list<int *>_his_data;
	std::list<int> _his_score;
	
	std::list<int> _his_max;

	//最高得分纪录
	int _all_max_score;
	//历史最高的max
	int _all_max_max;

	void saveScoreBoard();
	void loadScoreBoard();

	//使用AI计算最优方向 
	//@maxDepth: 搜索最大深度 默认是5
	//@maxMillisecond: 搜索最大时间 默认是INT_MAX
	enum DIRECTION getOptimalDirection(int maxDepth = 5, int maxMillisecond = INT_MAX);

	bool _in_menu;
	
};
#endif