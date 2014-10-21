#include "ccLayerColorOpacityPropagated.h"


LayerColorOpacityPropagated::LayerColorOpacityPropagated()
{
}


LayerColorOpacityPropagated::~LayerColorOpacityPropagated()
{
}

LayerColorOpacityPropagated* LayerColorOpacityPropagated::create()
{
	LayerColorOpacityPropagated* ret = new (std::nothrow) LayerColorOpacityPropagated();
	if (ret && ret->init())
	{
		ret->autorelease();
	}
	else
	{
		CC_SAFE_DELETE(ret);
	}
	return ret;
}

LayerColorOpacityPropagated * LayerColorOpacityPropagated::create(const cocos2d::Color4B& color, GLfloat width, GLfloat height)
{
	LayerColorOpacityPropagated * layer = new (std::nothrow) LayerColorOpacityPropagated();
	if (layer && layer->initWithColor(color, width, height))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}

LayerColorOpacityPropagated * LayerColorOpacityPropagated::create(const cocos2d::Color4B& color)
{
	LayerColorOpacityPropagated * layer = new (std::nothrow) LayerColorOpacityPropagated();
	if (layer && layer->initWithColor(color))
	{
		layer->autorelease();
		return layer;
	}
	CC_SAFE_DELETE(layer);
	return nullptr;
}

void LayerColorOpacityPropagated::setOpacity(GLubyte opacity)
{
	auto children = this->getChildren();
	for (auto child : children)
	{
		float co = child->getOpacity();
		float o = getOpacity();
		float op = co*(float)opacity / getOpacity()==0?1:getOpacity();
		child->setOpacity((GLubyte)op);
	}
	return Layer::setOpacity(opacity);
}
