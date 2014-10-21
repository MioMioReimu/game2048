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
	if (!LayerColor::initWithColor(cocos2d::Color4B(163,201,235,255)))
	{
        return false;
    }
	
	
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("2048", "Arial", 128);
    
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
		origin.y +visibleSize.height- label->getContentSize().height/2));

    // add the label as a child to this layer
    this->addChild(label, 1);

	auto label1 = LabelTTF::create("512", "Arial", 18);
	// position the label on the center of the screen
	label1->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - 30-label1->getContentSize().height));
	this->addChild(label1, 1);

	auto label2 = LabelTTF::create("64", "Arial", 24);
	// position the label on the center of the screen
	label2->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - 50 - label2->getContentSize().height));
	this->addChild(label2, 1);

	auto label3 = LabelTTF::create("16384", "Arial", 9);
	// position the label on the center of the screen
	label3->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - 90 - label3->getContentSize().height));
	this->addChild(label3, 1);

	for (int i = 0; i < 1; ++i)
	{
		// add "HelloWorld" splash screen"
		auto sprite = Sprite::create("HelloWorld.png");

		// position the sprite on the center of the screen
		sprite->setPosition(Vec2(visibleSize.width / 2 + i + origin.x, visibleSize.height / 2 + i + origin.y));

		// add the sprite as a child to this layer
		this->addChild(sprite, 0);
	}
    return true;
}


void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
