#pragma once
#include "cocos2d.h"
class LayerColorOpacityPropagated :
	public cocos2d::LayerColor
{
CC_CONSTRUCTOR_ACCESS:
	virtual ~LayerColorOpacityPropagated();
	LayerColorOpacityPropagated();
public:
	/** creates a fullscreen black layer */
	static LayerColorOpacityPropagated* create();
	/** creates a Layer with color, width and height in Points */
	static LayerColorOpacityPropagated * create(const cocos2d::Color4B& color, GLfloat width, GLfloat height);
	/** creates a Layer with color. Width and height are the window size. */
	static LayerColorOpacityPropagated * create(const cocos2d::Color4B& color);
	
	virtual void setOpacity(GLubyte opacity) override;
};

