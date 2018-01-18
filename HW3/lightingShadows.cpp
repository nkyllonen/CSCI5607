//1. determine if there are any objects inbetween point and cur_LIGHT
toLight_VEC = cur_LIGHT->getToLightVec(iPt);
toLight_RAY = Ray(iPt, toLight_VEC);

//1.1 cast toLight_RAY
light_sect = findIntersection(toLight_RAY, scene, cam);


switch(cur_LIGHT->getType())
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
		(light_sect.getIfIntersected()) ? inShadow = true :	inShadow = false;
		break;

	case SPOT_LIGHT:

		break;

	default:
		printf("ERROR invalid light type.\n");
		break;
}

//2. if no objects, then calc lighting
if (!inShadow)	newPix = newPix + cur_LIGHT->calcLighting(iSect);