#include "Scene.h"


/*----------------------------*/
// CONSTRUCTORS AND DESTRUCTORS
/*----------------------------*/
Scene::Scene()
{
	num_OBJS = 0;
	minT = 0;
	maxT = 5;
	bg_COLOR = Pixel(0, 0, 0);
	scene_LIGHTS.push_back(new Light(Pixel(0, 0, 0))); //first light is always AMBIENT
	num_LIGHTS = 1;
}


Scene::~Scene()
{
	//each Primitive is dynamically allocated
	for (int i = 0; i < num_OBJS; i++)
	{
		delete scene_OBJS.at(i);
	}

	//each Light is dynamically allocated
	for (int i = 0; i < num_LIGHTS; i++)
	{
		delete scene_LIGHTS.at(i);
	}
}

/*----------------------------*/
// SETTERS
/*----------------------------*/
void Scene::setBGColor(Pixel c)
{
	bg_COLOR = c;
}

void Scene::setAmbColor(Pixel c)
{
	scene_LIGHTS.at(0)->setColor(c);
}

/*----------------------------*/
// GETTERS
/*----------------------------*/
Primitive* Scene::getObj(int index)
{
	if (index >= 0 && index < num_OBJS)
	{
		return scene_OBJS.at(index);
	}
	else
	{
		fprintf(stderr, "ERROR in Scene.cpp: Invalid vector index.");
		return NULL;
	}
}

Pixel Scene::getBGColor()
{
	return bg_COLOR;
}

Pixel Scene::getAmbColor()
{
	return scene_LIGHTS.at(0)->getColor();
}

int Scene::getNumOBJS()
{
	return num_OBJS;
}

int Scene::getMaxT()
{
	return maxT;
}

int Scene::getNumLights()
{
	return num_LIGHTS;
}

Light * Scene::getLightAt(int i)
{
	return scene_LIGHTS.at(i);
}

/*----------------------------*/
// OTHER
/*----------------------------*/
void Scene::addObj(Primitive* p)
{
	scene_OBJS.push_back(p);
	num_OBJS++;
}

void Scene::addLight(Light * l)
{
	scene_LIGHTS.push_back(l);
	num_LIGHTS++;
}