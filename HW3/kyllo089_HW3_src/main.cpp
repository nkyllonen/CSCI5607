#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>

#include "Image.h"
#include "Pixel.h"

#include "Scene.h"
#include "Ray.h"
#include "Vec3D.h"
#include "Camera.h"

#include "Intersection.h"
#include "Material.h"

#include "Primitive.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Norm_Triangle.h"

#include "Light.h"
#include "Dir_Light.h"
#include "Point_Light.h"
#include "Spot_Light.h"

#define _USE_MATH_DEFINES
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION //only place once in one .cpp file
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION //only place once in one .cpp files
#include "stb_image_write.h"

using namespace std;

/*=============================*/
// Global Default Parameters
/*=============================*/
//FILM_RESOLUTION
int img_WIDTH = 640;
int img_HEIGHT = 480;
//OUTPUT_IMG
char* outFile = "./Images/raytraced.bmp";
//SCENE GEOMETRY
vector<Vec3D> vertex_pool;
vector<Vec3D> normal_pool;

int max_rec_depth = 1;

/*=============================*/
// Helper Function Declarations
/*=============================*/
void castRays(Image* img, Scene* scene, Camera* cam);
Ray constructRayThroughPixel(Camera* cam, int i, int j);
Intersection findIntersection(Ray r, Scene* scene, Camera* cam);
Pixel buildColor(Intersection iSect, Scene* scene, Camera* cam, int cur_rec_depth);
Pixel evaluateRayTree(Ray r, Scene* scene, Camera* cam, int cur_rec_depth);

Vec3D calcReflectance(Intersection iSect, Scene* scene, Camera* cam, int cur_rec_depth);
Vec3D calcTransmittance(Intersection iSect, Scene* scene, Camera* cam, int cur_rec_depth);
float calcOutgoingAngle(float n_in, float a_in, float n_out);
Vec3D buildTransmittedRay(float n_in, float a_in, float n_out, float a_out, Vec3D n, Vec3D l);

bool isValidIndex(int i, bool isVertex);

/*==============================================================*/
// MAIN
/*==============================================================*/
int main(int argc, char* argv[])
{
	Image *img = NULL;
	Scene* scene = new Scene();
	Camera* cam = new Camera();
	Material curr_MAT = Material();

	int max_Vertices = 0;
	int max_Normals = 0;
	
	float specifiedOutFile = false;

	if (argc != 2) {
		cout << "ERROR: Incorrect usage. Expected ./a.out scenefile\n";
		exit(0);
	}

	char* fileName = argv[1];
	string line;

	// open the file containing the scene description
	ifstream input(fileName);

	// check for errors in opening the file
	if (input.fail()) {
		cout << "Can't open file '" << fileName << "'" << endl;
		return 0;
	}

	// determine the file size (this is optional -- feel free to delete the 6 lines below)
	streampos begin, end;
	begin = input.tellg();
	input.seekg(0, ios::end);
	end = input.tellg();
	cout << "File '" << fileName << "' is: " << (end - begin) << " bytes long.\n\n";
	input.seekg(0, ios::beg);

	//Loop through reading each line
	string command;
	while (input >> command) { //Read first word in the line (i.e., the command type)
		cout << "\nJust read: " << command << endl;

		if (command[0] == '#') { //If the command is a comment
			getline(input, line); //skip rest of line
			cout << "Skipping comment: " << command << line << endl;
			continue;
		}
		///////////////////////////////////////////
		//			CAMERA PARAMETERS			 //
		///////////////////////////////////////////
		else if (command == "camera")
		{
			float tempx, tempy, tempz, ha;

			//position
			input >> tempx >> tempy >> tempz;
			Vec3D pos = Vec3D(tempx, tempy, tempz);
			cam->setPos(pos);

			//direction
			input >> tempx >> tempy >> tempz;
			Vec3D dir = Vec3D(tempx, tempy, tempz);
			dir.normalize();

			//up
			input >> tempx >> tempy >> tempz;
			Vec3D up = Vec3D(tempx, tempy, tempz);
			up.normalize();

			//make sure up and direction are orthogonal
			Vec3D right = cross(dir, -1 * up);
			up = -1 * cross(right, dir);

			cout << "\tpos : ";
			pos.print();
			cout << "\tdir : ";
			dir.print();
			cout << "\tup : ";
			up.print();
			cout << "\tright : ";
			right.print();

			cam->setRight(right);
			cam->setDir(dir);
			cam->setUp(up);

			//half angle (in degrees)
			input >> ha;
			cam->setHA(ha); //converted to radians within setter
		}
		else if (command == "film_resolution")
		{
			input >> img_WIDTH >> img_HEIGHT;
			cout << "Render an image of size: " << img_WIDTH << " by " << img_HEIGHT << endl;
		}
		else if (command == "output_image") //If the command is an output_image command
		{
			string temp;
			input >> temp;

			int size = strlen(temp.c_str()) + 1;

			outFile = new char[size];
			strcpy_s(outFile, size, temp.c_str());

			printf("Render to file named: %s\n", outFile);
			specifiedOutFile = true;
		}
		///////////////////////////////////////////
		//				SCENE GEOMETRY			 //
		///////////////////////////////////////////
		else if (command == "max_vertices")
		{
			int max;
			input >> max;

			(max < 3) ? max = 3 : max = max;

			printf("Max number of vertices specified : %i\n", max);

			max_Vertices = max;
		}
		else if (command == "max_normals")
		{
			int max;
			input >> max;

			(max < 1) ? max = 1 : max = max;

			printf("Max number of normals specified : %i\n", max);

			max_Normals = max;
		}
		else if (command == "vertex")
		{
			float x, y, z;
			input >> x >> y >> z;

			if (max_Vertices == 0)
			{
				cout << "ERROR. Vertex specified before max number of vertices." << endl;
				cout << "Program terminated." << endl;
				exit(1);
			}
			else
			{
				if (vertex_pool.size() >= max_Vertices)
				{
					cout << "ERROR. Max number of vertices already reached." << endl;
					cout << "Program terminated." << endl;
					exit(1);
				}
				else
				{
					Vec3D v = Vec3D(x, y, z);
					v.print();
					vertex_pool.push_back(v);
					printf("Added vertex #%i\n", vertex_pool.size());
				}
			}
		}
		else if (command == "normal")
		{
			float x, y, z;
			input >> x >> y >> z;

			if (max_Normals == 0)
			{
				cout << "ERROR. Normal specified before max number of normals." << endl;
				cout << "Program terminated." << endl;
				exit(1);
			}
			else
			{
				if (normal_pool.size() >= max_Normals)
				{
					cout << "ERROR. Max number of normals already reached." << endl;
					cout << "Program terminated." << endl;
					exit(1);
				}
				else
				{
					Vec3D n = Vec3D(x, y, z);
					n.normalize();
					n.print();
					normal_pool.push_back(n);
					printf("Added normal #%i\n", normal_pool.size());
				}
			}
		}
		else if (command == "triangle")
		{
			int v1, v2, v3;
			input >> v1 >> v2 >> v3;
			printf("Triangle with vertex indices %i %i %i\n", v1, v2, v3);

			//check if valid indices
			if (!isValidIndex(v1, true) || !isValidIndex(v2, true) || !isValidIndex(v3, true))
			{
				cout << "ERROR. Invalid vertex indices." << endl;
				cout << "Program terminated." << endl;
				exit(1);
			}

			//get vertices from pool
			Vec3D v1_VEC = vertex_pool.at(v1);
			Vec3D v2_VEC = vertex_pool.at(v2);
			Vec3D v3_VEC = vertex_pool.at(v3);

			//add to Scene
			Triangle* temp = new Triangle(v1_VEC, v2_VEC, v3_VEC);
			temp->setMaterial(curr_MAT);	

			scene->addObj(temp);
		}
		else if (command == "normal_triangle")
		{
			int v1, v2, v3;
			input >> v1 >> v2 >> v3;
			printf("Triangle with vertex indices %i %i %i\n", v1, v2, v3);

			int n1, n2, n3;
			input >> n1 >> n2 >> n3;
			printf("Triangle with normal indices %i %i %i\n", n1, n2, n3);

			//check if valid indices
			if (!isValidIndex(v1, true) || !isValidIndex(v2, true) || !isValidIndex(v3, true))
			{
				cout << "ERROR. Invalid vertex indices." << endl;
				cout << "Program terminated." << endl;
				exit(1);
			}

			if (!isValidIndex(n1, false) || !isValidIndex(n2, false) || !isValidIndex(n3, false))
			{
				cout << "ERROR. Invalid normal indices." << endl;
				cout << "Program terminated." << endl;
				exit(1);
			}

			//get vertices from pool
			Vec3D v1_VEC = vertex_pool.at(v1);
			Vec3D v2_VEC = vertex_pool.at(v2);
			Vec3D v3_VEC = vertex_pool.at(v3);

			//get normals from pool
			Vec3D n1_VEC = normal_pool.at(n1);
			Vec3D n2_VEC = normal_pool.at(n2);
			Vec3D n3_VEC = normal_pool.at(n3);

			//add to Scene
			Norm_Triangle* temp = new Norm_Triangle(v1_VEC, v2_VEC, v3_VEC, n1_VEC, n2_VEC, n3_VEC);
			temp->setMaterial(curr_MAT);
			scene->addObj(temp);
		}
		else if (command == "sphere") //If the command is a sphere command
		{
			float x, y, z, r;
			input >> x >> y >> z >> r;
			printf("Sphere at position (%f,%f,%f) with radius %f\n", x, y, z, r);

			//add to Scene
			Sphere* temp = new Sphere(Vec3D(x, y, z), r);
			temp->setMaterial(curr_MAT);
			scene->addObj(temp);
		}
		else if (command == "background") //If the command is a background command
		{
			float r, g, b;
			input >> r >> g >> b;
			Component c_R = ComponentClamp(r * 255);
			Component c_G = ComponentClamp(g * 255);
			Component c_B = ComponentClamp(b * 255);
			printf("Background color of (%i, %i, %i)\n", c_R, c_G, c_B);
			scene->setBGColor(Pixel(c_R, c_G, c_B));
		}
		///////////////////////////////////////////
		//			MATERIAL COMMANDS			 //
		///////////////////////////////////////////
		else if (command == "material")
		{
			cout << "New material: " << endl;

			float r, g, b, x;
			Component c_R, c_G, c_B;
			input >> r >> g >> b;
			c_R = ComponentClamp(r * 255);
			c_G = ComponentClamp(g * 255);
			c_B = ComponentClamp(b * 255);
			curr_MAT.setAmbient(Pixel(c_R, c_G, c_B));
			printf("\tAmbient color of (%i, %i, %i)\n", c_R, c_G, c_B);

			input >> r >> g >> b;
			c_R = ComponentClamp(r * 255);
			c_G = ComponentClamp(g * 255);
			c_B = ComponentClamp(b * 255);
			curr_MAT.setDiffuse(Pixel(c_R, c_G, c_B));
			printf("\tDiffuse color of (%i, %i, %i)\n", c_R, c_G, c_B);

			input >> r >> g >> b;
			c_R = ComponentClamp(r * 255);
			c_G = ComponentClamp(g * 255);
			c_B = ComponentClamp(b * 255);
			curr_MAT.setSpecular(Pixel(c_R, c_G, c_B));
			printf("\tSpecular color of (%i, %i, %i)\n", c_R, c_G, c_B);

			input >> x;
			curr_MAT.setNS(x);
			cout << "\tPhong cosine power: " << x << endl;

			input >> r >> g >> b;
			c_R = ComponentClamp(r * 255);
			c_G = ComponentClamp(g * 255);
			c_B = ComponentClamp(b * 255);
			curr_MAT.setTransmissive(Pixel(c_R, c_G, c_B));
			printf("\tTransmissive color of (%i, %i, %i)\n", c_R, c_G, c_B);

			input >> x;
			curr_MAT.setIOR(x);
			cout << "\tIOR: " << x << endl;
		}
		///////////////////////////////////////////
		//			LIGHTING COMMANDS			 //
		///////////////////////////////////////////
		else if (command == "directional_light")
		{
			Dir_Light* dl = new Dir_Light();
			cout << "Directional light added" << endl;

			float r, g, b;
			input >> r >> g >> b;
			printf("\tlight color set to : (%f, %f, %f)\n", r, g, b);
			dl->setColor(Pixel(r, g, b));

			input >> r >> g >> b;
			Vec3D dl_DIR = Vec3D(r, g, b);
			dl_DIR.normalize();
			dl->setDir(dl_DIR);

			cout << "\tlight direction :";
			dl_DIR.print();

			scene->addLight(dl);
		}
		else if (command == "point_light")
		{
			Point_Light* pl = new Point_Light();
			cout << "Point light added" << endl;
			
			float r, g, b;
			input >> r >> g >> b;
			printf("\tlight color set to : (%f, %f, %f)\n", r, g, b);
			pl->setColor(Pixel(r, g, b));

			float x, y, z;
			input >>x >> y >> z;
			Vec3D pl_POS = Vec3D(x, y, z);
			cout << "\tlight position :";
			pl_POS.print();

			pl->setPos(pl_POS);

			scene->addLight(pl);
		}
		else if (command == "spot_light")
		{
			Spot_Light* sl = new Spot_Light();
			cout << "Spot light added" << endl;

			Component c_R, c_G, c_B;
			float r, g, b;
			input >> r >> g >> b;
			c_R = ComponentClamp(r * 255);
			c_G = ComponentClamp(g * 255);
			c_B = ComponentClamp(b * 255);
			printf("\tlight color set to : (%i, %i, %i)\n", c_R, c_G, c_B);
			sl->setColor(Pixel(c_R, c_G, c_B));

			input >> r >> g >> b;
			Vec3D sl_POS = Vec3D(r, g, b);

			cout << "\tlight position :";
			sl_POS.print();
			sl->setPos(sl_POS);

			input >> r >> g >> b;
			Vec3D sl_DIR = Vec3D(r, g, b);

			cout << "\tlight direction :";
			sl_DIR.print();
			sl->setDir(sl_DIR);

			float a1, a2;
			input >> a1 >> a2;
			printf("\tmin angle : %f    max angle : %f\n", a1, a2);
			sl->setMinAngle(a1);
			sl->setMaxAngle(a2);

			scene->addLight(sl);
		}
		else if (command == "ambient_light")
		{
			float r, g, b;
			Component c_R, c_G, c_B;
			input >> r >> g >> b;
			c_R = ComponentClamp(r * 255);
			c_G = ComponentClamp(g * 255);
			c_B = ComponentClamp(b * 255);
			scene->setAmbColor(Pixel(c_R, c_G, c_B));
			printf("Scene ambient color set to : (%i, %i, %i)\n", c_R, c_G, c_B);
		}
		///////////////////////////////////////////
		//				MISC COMMANDS			 //
		///////////////////////////////////////////
		else if (command == "max_depth")
		{
			int md;
			input >> md;
			max_rec_depth = md;

			cout << "Max recursion depth set to : " << max_rec_depth << endl;
		}
		else
		{
			getline(input, line); //skip rest of line
			cout << "WARNING. Unknown command: " << command << endl;
		}
	}

	cout << "\nNumber of lights in scene: " << scene->getNumLights() << endl;

	///////////////////////////////////////////
	//			CONSTRUCTING IMAGE			 //
	///////////////////////////////////////////
	img = new Image(img_WIDTH, img_HEIGHT);
	printf("Vertices entered: %i / %i\n", vertex_pool.size(), max_Vertices);
	printf("Normals entered: %i / %i\n", normal_pool.size(), max_Normals);
	cout << "\nBegin casting rays through " << scene->getNumOBJS() << " primitive(s)." << endl;
	cout << "-------------------------------------" << endl;
	castRays(img, scene, cam);


	///////////////////////////////////////////
	//			OUTPUTING IMAGE				//
	///////////////////////////////////////////
	if (img == NULL)
	{
		cout << "ERROR. Image pointer is NULL." << endl;
	}
	else
	{
		img->Write(outFile);
	}

	///////////////////////////////////////////
	//			CLEANING UP OBJS			 //
	///////////////////////////////////////////
	delete img;
	if (specifiedOutFile) delete[] outFile; //only delete if has been dynamically allocated
	scene->~Scene();
	delete cam;

	return 0;
}

/*--------------------------------------------------------------*/
// castRays : loop through image pixels and cast rays through each
/*--------------------------------------------------------------*/
void castRays(Image* img, Scene* scene, Camera* cam)
{	
	auto started = std::chrono::high_resolution_clock::now();

	//Loop through each pixel
	#pragma omp parallel for schedule(dynamic)
	for (int i = 0; i < img_WIDTH; i++)
	{
		//cout << "Casting rays through row " << i << endl;
		for (int j = 0; j < img_HEIGHT; j++)
		{
			Ray r_ij = constructRayThroughPixel(cam, i, j);
			img->GetPixel(i, j) = evaluateRayTree(r_ij, scene, cam, 0);
		}
	}

	auto done = std::chrono::high_resolution_clock::now();

	cout << "\nTotal time : "
		<< std::chrono::duration_cast<std::chrono::milliseconds>(done - started).count()
		<< " ms" << endl;

	printf("\n%s successfully COMPLETED\n", outFile);
}

/*--------------------------------------------------------------*/
// constructRayThroughPixel : generates Ray from Camera and through
//								the pixel at (i,j)
/*--------------------------------------------------------------*/
Ray constructRayThroughPixel(Camera* cam, int i, int j)
{
	Ray result;
	result.setOrigin(cam->getPos());

	//calculate scalar distance between cam and img plane
	float cam_to_img_dist = (img_HEIGHT) / (2*tan(cam->getHA()));

	Vec3D D = cam_to_img_dist*cam->getDir();

	//calculate ray direction
	//(subtract indices by half width and height to center img)
	Vec3D r_dir = D + (i - img_WIDTH / 2)*cam->getRight()
					+ (-j + img_HEIGHT / 2)*cam->getUp();

	r_dir.normalize();
	result.setDir(r_dir);

	return result;
}
/*--------------------------------------------------------------*/
// evaluateRayTree :
/*--------------------------------------------------------------*/
Pixel evaluateRayTree(Ray r, Scene* scene, Camera* cam, int cur_rec_depth)
{
	//base case = at maximum recursive depth
	if (cur_rec_depth == max_rec_depth)
	{
		return Pixel(0, 0, 0);
	}

	Intersection iSect = findIntersection(r, scene, cam);

	if (iSect.getIfIntersected()) return buildColor(iSect, scene, cam, cur_rec_depth);
	
	return scene->getBGColor();
}

/*--------------------------------------------------------------*/
// findIntersection : returns Intersection object containing
//						data about the (un)found intersection pt
/*--------------------------------------------------------------*/
Intersection findIntersection(Ray r, Scene* scene, Camera* cam)
{
	int num_objs = scene->getNumOBJS();
	float minT = INFINITY;
	int min_obj_index = 0;
	Primitive* cur_Prim;
	Intersection cur_iSect;					//current intersection of obj at obj_index
	Intersection min_iSect;					//current minimum intersection
	float minT_val = cur_iSect.getMinT();
	
	cur_iSect.setColor(scene->getBGColor());//default to background color
	
	//determine pt to camera vector
	Vec3D to_cam = -1 * r.getDir();			//reverse of ray direction
	cur_iSect.setToCamVec(to_cam);

	for (int obj_index = 0; obj_index < num_objs; obj_index++)
	{
		cur_Prim = scene->getObj(obj_index);

		//find minimum t for intersection
		if (cur_Prim->findIntersectionPoint(r, cur_iSect))
		{
			if (cur_iSect.getT() > minT_val && cur_iSect.getT() < minT)
			{
				minT = cur_iSect.getT();
				min_obj_index = obj_index;
				min_iSect = cur_iSect;
			}
		}
	}
	
	if (min_iSect.getIfIntersected())
	{
		//make sure Intersection holds values associated with minT
		min_iSect.setT(minT);
		min_iSect.setSceneObjIndex(min_obj_index);

		//determine intersection point
		Vec3D pt = r.getOrigin() + minT*r.getDir();
		min_iSect.setPoint(pt);

		//set intersection material
		min_iSect.setMaterial(scene->getObj(min_obj_index)->getMaterial());

		//store incoing Ray
		min_iSect.setIncomingRay(r);
	}
	else //redundant, but just to be safe
	{
		min_iSect.setColor(scene->getBGColor());
	}

	return min_iSect;
}

/*--------------------------------------------------------------*/
// buildColor : returns the calculated Pixel value corresponding
//				to the given intersection pt
/*--------------------------------------------------------------*/
Pixel buildColor(Intersection iSect, Scene* scene, Camera* cam, int cur_rec_depth)
{
	Vec3D newPixV = Vec3D(0, 0, 0);
	Light* cur_LIGHT;
	Ray toLight_RAY;
	Vec3D toLight_VEC, iPt;
	Intersection light_sect;
	float toLight_t;
	bool inShadow = false;

	for (int i = 0; i < scene->getNumLights(); i++)
	{
		cur_LIGHT = scene->getLightAt(i);
		iPt = iSect.getPoint();
		inShadow = false;

		//1. determine if there are any objects inbetween point and cur_LIGHT
		toLight_VEC = cur_LIGHT->getToLightVec(iPt);
		toLight_RAY = Ray(iPt + 0.01*toLight_VEC, toLight_VEC);

		//1.1 cast toLight_RAY
		light_sect = findIntersection(toLight_RAY, scene, cam);
		
		switch (cur_LIGHT->getType())
		{
			case AMB_LIGHT:
				inShadow = false;
				break;

			case POINT_LIGHT:
				toLight_t = toLight_VEC.getMagnitude(); //t value = mag if dir = unit vec

				if (light_sect.getIfIntersected())
				{
					//1.2 compare t values
					(toLight_t < light_sect.getT()) ? inShadow = false : inShadow = true;
				}
				else
				{
					inShadow = false;
				}
				break;

			case DIR_LIGHT:
				if (light_sect.getIfIntersected())
				{
					inShadow = true;
				}
				else
				{
					inShadow = false;
				}
				break;

			case SPOT_LIGHT:
				toLight_t = toLight_VEC.getMagnitude(); //t value = mag if dir = unit vec

				if (light_sect.getIfIntersected())
				{
					//1.2 compare t values
					(toLight_t < light_sect.getT()) ? inShadow = false : inShadow = true;
				}
				else
				{
					inShadow = false;
				}
				break;

			default:
				printf("ERROR invalid light type.\n");
				break;
		}

		//2. if no objects, then calc lighting at this point
		if (!inShadow)
		{
			newPixV = newPixV + cur_LIGHT->calcLighting(iSect);
		}
	}//END FOR EACH LIGHT

	//3. add reflections
	newPixV = newPixV + calcReflectance(iSect, scene, cam, cur_rec_depth);

	//4. add transmission
	newPixV = newPixV + calcTransmittance(iSect, scene, cam, cur_rec_depth);

	//convert summation to Pixel value
	Pixel result;
	util::vec_to_pixel(newPixV, result);

	return result;
}

Vec3D calcReflectance(Intersection iSect, Scene* scene, Camera* cam, int cur_rec_depth)
{
	Vec3D ref_dir;
	Vec3D v = iSect.getToCamVec();
	v.normalize();
	Vec3D n = iSect.getNormal();
	n.normalize();
	ref_dir = 2 * dotProduct(n, v)*n - v;

	Ray ref_Ray = Ray(iSect.getPoint() + 0.01*ref_dir, ref_dir);

	//recursive call - increment current depth of recursion
	Vec3D specularV;
	util::pixel_to_vec(iSect.getMaterial().getSpecular(), specularV);

	Vec3D evalV;
	util::pixel_to_vec(evaluateRayTree(ref_Ray, scene, cam, cur_rec_depth + 1), evalV);

	return (1.0 / 255) * specularV * evalV; //shift specular values [0,255] -> [0,1]
}

Vec3D calcTransmittance(Intersection iSect, Scene* scene, Camera* cam, int cur_rec_depth)
{
	float index_refr = iSect.getMaterial().getIOR();
	float n_air = 1.0;
	Vec3D n = iSect.getNormal();
	n.normalize();
	Vec3D transV;
	util::pixel_to_vec(iSect.getMaterial().getTransmissive(), transV);

	//4.1 determine if going into or out of an obj
	Ray incoming_RAY = iSect.getIncomingRay();
	Vec3D incoming_DIR = incoming_RAY.getDir();
	Vec3D t_VEC;

	//4.1.1 find angle between incoming Ray dir and normal (n) at intersection pt
	float between_angle = acos(dotProduct(-1 * incoming_DIR, n));
	float outgoing_angle = M_PI / 4.0;
	//4.1.2 acute = entering // obtuse = exiting
	if (between_angle < M_PI / 2.0)
	{
		outgoing_angle = calcOutgoingAngle(n_air, between_angle, index_refr);
		t_VEC = buildTransmittedRay(n_air, between_angle, index_refr, outgoing_angle, n, -1 * incoming_DIR);
	}
	else
	{
		n = -1 * n; //flip normal when exiting
		between_angle = acos(dotProduct(-1 * incoming_DIR, n)); //recalculate incoming angle?
		outgoing_angle = calcOutgoingAngle(index_refr, between_angle, n_air);
		t_VEC = buildTransmittedRay(index_refr, between_angle, n_air, outgoing_angle, n, -1 * incoming_DIR);
	}

	//5. build transmitted Ray
	Ray trans_RAY = Ray(iSect.getPoint() + 0.01*t_VEC, t_VEC);

	//6. recursive call - increment current depth of recursion
	Vec3D evalV;
	util::pixel_to_vec(evaluateRayTree(trans_RAY, scene, cam, cur_rec_depth + 1), evalV);

	return (1.0 / 255) * transV * evalV; //shift transmittance values [0,255] -> [0,1]
}

/*--------------------------------------------------------------*/
// calcOutgoingAngle : returns refraction angle using Snell's Law
/*--------------------------------------------------------------*/
float calcOutgoingAngle(float n_in, float a_in, float n_out)
{
	return asin((n_in / n_out)*sin(a_in));
}

/*--------------------------------------------------------------*/
// buildTransmittedRay : returns Vec of refracted ray using
//							a modification of Snell's Law
/*--------------------------------------------------------------*/
Vec3D buildTransmittedRay(float n_in, float a_in, float n_out, float a_out, Vec3D n, Vec3D l)
{
	Vec3D result = ((n_in / n_out)*cos(a_in) - cos(a_out))*n - (n_in / n_out)*l;
	result.normalize();
	return result;
}

bool isValidIndex(int i, bool isVertex)
{
	bool result;

	if (isVertex) result = (i < vertex_pool.size());
	else result = (i < normal_pool.size());

	return result;
}
