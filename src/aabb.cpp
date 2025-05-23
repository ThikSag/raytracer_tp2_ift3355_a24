#include "aabb.h" 
#include "object.h"

// @@@@@@ VOTRE CODE ICI
// Implémenter l'intersection d'un rayon avec un AABB dans l'intervalle décrit.
bool AABB::intersect(Ray ray, double t_min, double t_max)  {
	std::vector<double3> coins = retrieve_corners(*this);
	double3 min = coins[0];
	double3 max = coins[7];
	double3 dir = ray.direction;
	double3 ori = ray.origin;

	// normal des 3 plans alignés sur les axes
	double3 normalX = {1, 0, 0};
	double3 normalY = {0, 1, 0};
	double3 normalZ = {0, 0, 1};

	bool parallelXY = dot(dir, normalZ) == 0 ; // le rayon est parallèle au plan XY
	bool parallelXZ = dot(dir, normalY) == 0 ; // le rayon est parallèle au plan XZ
	bool parallelYZ = dot(dir, normalX) == 0 ; // le rayon est parallèle au plan YZ

	// Distances d'intersection avec les 6 plans des côtés du AABB.
	double t1_x, t2_x, t1_y, t2_y, t1_z, t2_z;
	double3 t_near;
	double3 t_far;

	if (!parallelXY) {
		// Trouver les intersection en Z des plans de l'avant et de l'arrière du AABB.
		t1_z = (min.z - ori.z) / dir.z;
		t2_z = (max.z - ori.z) / dir.z;

		if (t1_z < t2_z) {
			t_near.z = t1_z;
			t_far.z = t2_z;

		} else {
			t_near.z = t2_z;
			t_far.z = t1_z;
		}

	} else {
		t1_z = -DBL_MAX; // valeur d'erreur pour indiqué qu'il n'y a pas d'intersection
		t2_z = -DBL_MAX;
		t_near.z = t1_z;
		t_far.z = t2_z;
	}
	
	if (!parallelXZ) {
		// Trouver les intersection en Y des plans du top et du bottom du AABB.
		t1_y = (min.y - ori.y) / dir.y;
		t2_y = (max.y - ori.y) / dir.y;

		if (t1_y < t2_y) {
			t_near.y = t1_y;
			t_far.y = t2_y;

		} else {
			t_near.y = t2_y;
			t_far.y = t1_y;
		}

	} else {
		t1_y = -DBL_MAX; // valeur d'erreur pour indiqué qu'il n'y a pas d'intersection
		t2_y = -DBL_MAX;
		t_near.y = t1_y;
		t_far.y = t2_y;
	}

	if (!parallelYZ) {
		// Trouver les intersection en X des plans de gauche et de droit du AABB.
		t1_x = (min.x - ori.x) / dir.x;
		t2_x = (max.x - ori.x) / dir.x;

		if (t1_x < t2_x) {
			t_near.x = t1_x;
			t_far.x = t2_x;

		} else {
			t_near.x = t2_x;
			t_far.x = t1_x;
		}

	} else {
		t1_x = -DBL_MAX; // valeur d'erreur pour indiqué qu'il n'y a pas d'intersection
		t2_x = -DBL_MAX;
		t_near.x = t1_x;
		t_far.x = t2_x;
	}

	// Le rayon intersecte le AABB si toutes les intersections les plus proche sont plus petite que toutes les intersection les plus loins.
	double near_max = linalg::maxelem(t_near);
	double far_min = linalg::minelem(t_far);

	if (near_max<far_min) {

		double near_min = linalg::minelem(t_near);
		if(near_min > t_max) {
			// l'intersection la plus proche est plus grande que l'intervalle donnée
			return false;
		}
		
		double far_max = linalg::maxelem(t_far);
		if(far_max < t_min) {
			// l'intersection la plus loin est plus petite que l'intervalle donnée
			return false;
		}

		return true;
	}

	return false;
};

// @@@@@@ VOTRE CODE ICI
// Implémenter la fonction qui permet de trouver les 8 coins de notre AABB.
std::vector<double3> retrieve_corners(AABB aabb) {
	//       G_________H     A est le coins aabb.min qui contient les valeurs minimum en x, y et z du AABB.
	//      /|        /|     H est le coins aabb.max qui contient les valeurs maximum en x, y et z du AABB.
	//     / |       / |     
	//    /  E______/__F     Les coins ABCD sont sur le même plan XY.
	//   /  /      /  /      Les coins EFGH sont sur le même plan XY.
	//  C__/______D  /       Les coins ABEF sont sur le même plan XZ.
	//  | /       | /        Les coins CDGH sont sur le même plan XZ.
	//  |/        |/         Les coins ACEG sont sur le même plan YZ.
	//  A_________B          Les coins BDFH sont sur le même plan YZ.

	// Récupérer les coordonnées individuelles de min et max
	double x0 = aabb.min.x;
	double y0 = aabb.min.y;
	double z0 = aabb.min.z;

	double x1 = aabb.max.x;
	double y1 = aabb.max.y;
	double z1 = aabb.max.z;

	// Définition des 8 coins du AABB
	double3 a {aabb.min};
	double3 b = {x1, y0, z0};
	double3 c = {x0, y1, z0};
	double3 d = {x1, y1, z0};
	double3 e = {x0, y0, z1};
	double3 f = {x1, y0, z1};
	double3 g = {x0, y1, z1};
	double3 h {aabb.max};

	std::vector<double3> coins{a, b, c, d, e, f, g, h};
	return coins;
};

// @@@@@@ VOTRE CODE ICI
// Implémenter la fonction afin de créer un AABB qui englobe tous les points.
AABB construct_aabb(std::vector<double3> points) {
	// Initialisation de min et max avec les valeurs opposé les plus extrême.
	double3 min {DBL_MAX,DBL_MAX,DBL_MAX};
	double3 max {-DBL_MAX,-DBL_MAX,-DBL_MAX};

	// Itère au travers de tout les points pour trouver les coordonnées les plus petites et les plus grandes en x, y et z pour former un AABB qui englobe tous les points.
	for (double3 p : points) {
		if (p.x < min.x) {
			min.x = p.x;
		}
		if (p.y < min.y) {
			min.y = p.y;
		}
		if (p.z < min.z) {
			min.z = p.z;
		}
		if (p.x > max.x) {
			max.x = p.x;
		}
		if (p.y > max.y) {
			max.y = p.y;
		}
		if (p.z > max.z) {
			max.z = p.z;
		}
	}

	// Ajout d'une distance d'epsilon entre les côtés de AABB et les points pour que le AABB englobe les points et non que les points soient sur les côtés du AABB.
	double3 epsilon = {EPSILON, EPSILON, EPSILON};
	min -= epsilon; 
	max += epsilon;

	return AABB{min,max};
};

AABB combine(AABB a, AABB b) {
	return AABB{min(a.min,b.min),max(a.max,b.max)};
};

bool compare(AABB a, AABB b, int axis){
	return a.min[axis] < b.min[axis];
};