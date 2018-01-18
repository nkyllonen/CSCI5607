#ifndef SCENE_INCLUDED
#define SCENE_INCLUDED

#include <vector>

#include "Vec3D.h"
#include "Primitive.h"
#include "Pixel.h"
#include "Light.h"

class Scene
{
public:
	//CONSTRUCTORS AND DESTRUCTORS
	Scene();
	~Scene();

	//SETTERS
	void setBGColor(Pixel c);
	void setAmbColor(Pixel c);

	//GETTERS
	Primitive* getObj(int index);
	Pixel getBGColor();
	Pixel getAmbColor();
	int getNumOBJS();
	int getMaxT();
	int getNumLights();
	Light* getLightAt(int i);

	//OTHER
	void addObj(Primitive* p);
	void addLight(Light* l);

private:
	std::vector<Primitive*> scene_OBJS; //vector of Primitive pointers to objs in Scene
	int num_OBJS;
	int minT;
	int maxT;
	Pixel bg_COLOR;
	
	std::vector<Light*> scene_LIGHTS; //vector of Light pointers
	int num_LIGHTS;

};

#endif