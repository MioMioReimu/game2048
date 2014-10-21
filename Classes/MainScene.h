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
	//ִ���ƶ����� ��������UI
	bool move(enum DIRECTION direction);
	//��������������������µ����֣�num�ǲ��������ָ���
	//������̲����Բ������򷵻�false
	//��������UI
	bool produce(int num = 1);
	//����scoreUI
	void updateScore();
	//gameover����
	void gameover();
	//���¿�ʼ  ��������UI
	void restart();
	//����  ��������UI
	bool undo();
	//�Ӵ浵�м���״̬����������UI �����״̬��ʾ���̵�ǰ״̬���Լ���ʷ����
	bool save(int saveidx);
	//���浱ǰ״̬���浵����������UI �����״̬��ʾ���̵�ǰ״̬���Լ���ʷ����
	bool load(int saveidx);

	//����ǰ������״̬push����ʷ������
	void push_history_data();
	//���������ʷ����
	void clear_history_data();
	//ɾ�����һ������ʷ����
	void pop_history_data();
private:
	//��ʼ��2048�̵����� 1/4�ļ�����4��3/4�ļ�����2 ��ʼ��ʱ4������
	void initdata();
	
	//ִ��Move������UI���� ��������
	void processMoveEvent(const MoveEvent &evt);
	//ִ���²�����UI���� 
	void processProduceEvent(const ProduceEvent &evt);
	//2048��״̬
	int *data;
	//
	// 2048�̵����ֵ
	int _max;
	//�÷�
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

	//��ߵ÷ּ�¼
	int _all_max_score;
	//��ʷ��ߵ�max
	int _all_max_max;

	void saveScoreBoard();
	void loadScoreBoard();

	//ʹ��AI�������ŷ��� 
	//@maxDepth: ���������� Ĭ����5
	//@maxMillisecond: �������ʱ�� Ĭ����INT_MAX
	enum DIRECTION getOptimalDirection(int maxDepth = 5, int maxMillisecond = INT_MAX);

	bool _in_menu;
	
};
#endif