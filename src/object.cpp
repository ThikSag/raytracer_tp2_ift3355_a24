#include "object.h"

// Fonction retournant soit la valeur v0 ou v1 selon le signe.
int rsign(double value, double v0, double v1) {
	return (int(std::signbit(value)) * (v1-v0)) + v0;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection d'une sphère.
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Sphere::local_intersect(Ray ray, 
							 double t_min, double t_max, 
							 Intersection *hit) {
    //Paramètre de résolution d'équation implicite de la sphère
    double a = dot(ray.direction, ray.direction);
    double b = 2.0 * dot(ray.origin, ray.direction);
    double c = dot(ray.origin, ray.origin) - radius * radius;
    double d = b * b - 4 * a * c;

    if (d < 0){
       return false; //pas intersection si d est négatif
    }

    //Calculer les intersections possibles
    double sqrt_cercle = sqrt(d);
    double t1 = (-b - sqrt_cercle) / (2.0 * a);
    double t2 = (-b + sqrt_cercle) / (2.0 * a);

    if ((t1 > 0) && (t1 > t_min) && (t1 < t_max)) {
        // l'origine du rayon est a l'extérieur de la sphère et le rayon intersecte la sphère dans l'intervalle donnée
        hit->depth = t1;
        hit->position = ray.origin + t1 * ray.direction;
        hit->normal = normalize(hit->position);

    } else if ((t1 < 0) && (t2 > t_min) && (t2 < t_max)) {
        // l'origine du rayon est à l'intérieur de la sphère et le rayon intersecte la sphère dans l'intervalle donnée
        hit->depth = t2;
        hit->position = ray.origin + t2 * ray.direction;
        hit->normal = normalize(hit->position) * -1; // on inverse la normal lorsque le rayon intersecte l'intérieur de la sphère

    } else {
        // peut importe l'origine du rayon, la sphère n'est pas intersecté dan sl'intervalle donnée
        return false;
    }

    //parametrisation UV
    double x_formule = hit->position.z;
    double y_formule = hit->position.x;
    double z_formule = hit->position.y;

    double coord_v = acos(z_formule / radius) / PI;
    double coord_u;

    if (z_formule == radius || z_formule == -radius) { 
        // Cas spécial : l'intersection est a un pole de la sphère et x_formule = y_formule = 0
        // On prend la même valeur que v pour u afin de simplifier.
        coord_u = coord_v; 

    } else { // cas général
        coord_u = acos(x_formule / sqrt(x_formule * x_formule + y_formule * y_formule)) / (2 * PI);

        if (y_formule < 0) {
            coord_u = 1 - coord_u;
        }
    }

    hit->uv = {coord_u, coord_v};

    return true;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour la sphère.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire (comme ici).
AABB Sphere::compute_aabb() {
    // Les points extrêmes de la sphère sur un axe dans l'espace local sont : 
    //     centre_de_la_sphère +/- rayon_de_la_sphère_sur_l'axe
    double3 extrem_X = {radius, 0, 0};
    double3 extrem_Y = {0, radius, 0};
    double3 extrem_Z = {0, 0, radius};
    std::vector<double3> extrem_local = {extrem_X, -extrem_X, extrem_Y, -extrem_Y, extrem_Z, -extrem_Z};

    // Création du AABB dans l'espace local
    AABB aabb_local = construct_aabb(extrem_local);
    std::vector<double3> coins_local = retrieve_corners(aabb_local);

    // Transformation des coins du AABB local dans l'espace global
    std::vector<double3> coins_local_tranform;

    for (double3 point_local : coins_local) {
        double4 point_local_homogene = {point_local.x, point_local.y, point_local.z, 1};
        double4 point_global_homogene = mul(transform, point_local_homogene);
        double3 point_global = {point_global_homogene.x, point_global_homogene.y, point_global_homogene.z};
        coins_local_tranform.push_back(point_global);
    }

    // Création du AABB dans l'espace global    
    AABB aabb_global = construct_aabb(coins_local_tranform);

	return aabb_global;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un quad (rectangle).
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Quad::local_intersect(Ray ray, 
							double t_min, double t_max, 
							Intersection *hit) {
    // Paramètre de résolution d'équation implicite du plan
	double3 normal = {0, 0, 1};
	double A = normal.x;
	double B = normal.y;
	double C = normal.z;
	double D = 0;
	double denom = A * ray.direction.x + B * ray.direction.y + C * ray.direction.z;

	if (std::abs(denom) < EPSILON) {
        return false; //rayon est parallèle au plan
    }

    double t = -(A * ray.origin.x + B * ray.origin.y + C * ray.origin.z + D) / denom;

    if (t < t_min || t > t_max) {
        return false; // intersection n'est pas dans l'interval
    }

    double3 p = ray.origin + t * ray.direction;

    // Vérifiez si le point d'intersection est dans le quad
    if (std::abs(p.x) > half_size || std::abs(p.y) > half_size) {
        return false;
    }

    hit->position = p;
    hit->depth = t;

    // la direction du rayon et la normal du plan doivent être en sens inverse
    if (dot(normal, ray.direction) > 0) {
        hit->normal = -normal;

    } else {
        hit->normal = normal;
    }

    //parametrisation UV
    double coord_x = (p.x + half_size) / (2 * half_size);
    double coord_y = (p.y + half_size) / (2 * half_size);
    hit->uv = {coord_x, 1 - coord_y};

    return true;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le quad (rectangle).
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire.
AABB Quad::compute_aabb() {
    // 4 coins du quad dans l'espace local
    double4 c1_local = {-half_size, -half_size, 0, 1};
    double4 c2_local = {half_size, -half_size, 0, 1};
    double4 c3_local = {half_size, half_size, 0, 1};
    double4 c4_local = {-half_size, half_size, 0, 1};
    std::vector<double4> c_local = {c1_local, c2_local, c3_local, c4_local};

    // Transformation des coins du quad dans l'espace global
    std::vector<double3> c_global;

    for (double4 p : c_local) {
        double4 p_global_homogene = mul(transform, p);
        double3 p_global = {p_global_homogene.x, p_global_homogene.y, p_global_homogene.z};
        c_global.push_back(p_global);
    }

    // Création du AABB dans l'espace global 
    AABB aabb_global = construct_aabb(c_global);

	return aabb_global;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un cylindre.
//
// Référez-vous au PDF pour la paramétrisation des coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
bool Cylinder::local_intersect(Ray ray,
                               double t_min, double t_max,
                               Intersection *hit) {
    // Paramètre de résolution d'équation implicite du cylindre
    double a = ray.direction.x * ray.direction.x + ray.direction.z * ray.direction.z;
    double b = 2.0 * (ray.origin.x * ray.direction.x + ray.origin.z * ray.direction.z);
    double c = ray.origin.x * ray.origin.x + ray.origin.z * ray.origin.z - radius * radius;
    double d = b * b - 4 * a * c;

    if (d < 0) {
        return false; //pas intersection lorsque d est négatif
    }
	
    //calculer les possibilités d'intersection
    double sqrt_cercle = sqrt(d);
    double t1 = (-b - sqrt_cercle) / (2.0 * a);
    double t2 = (-b + sqrt_cercle) / (2.0 * a);
    double t_proche = t_max;
    bool intersecte = false;
	
    //vérifier si t1 est dans l'intervalle t_min, t_max
    if (t1 >= t_min && t1 <= t_max) {
	    //Vérifier si pt d'intersection est dans les limites du cylindre
        double y1 = ray.origin.y + t1 * ray.direction.y;

        if (y1 >= -half_height && y1 <= half_height) {
	        //Affecter t1 comme le t_proche si valide
            t_proche = t1;
            hit->position = ray.origin + t1 * ray.direction;
            hit->normal = normalize(double3{hit->position.x, 0, hit->position.z});
            hit->depth = t1;

            if (dot(ray.direction, hit->normal) > 0) {
                hit->normal = -hit->normal;
            }

            intersecte = true;
        }
    }

    //regarder intersection t2 si t1 invalide ou t2 est plus proche
    if (t2 >= t_min && t2 <= t_max && (!intersecte || t2 < t_proche)) {
        double y2 = ray.origin.y + t2 * ray.direction.y;
        
        if (y2 >= -half_height && y2 <= half_height) {
            t_proche = t2;
            hit->position = ray.origin + t2 * ray.direction;
            hit->normal = normalize(double3{hit->position.x, 0, hit->position.z});
            hit->depth = t2;

            if (dot(ray.direction, hit->normal) > 0) {
                hit->normal = -hit->normal;
            }

            intersecte = true;
        }
    }

    // Paramétrisation UV
    double x_formule = hit->position.x;
    double y_formule = hit->position.z;
    double z_formule = hit->position.y / half_height;

    double coord_v = (z_formule + 1) / 2;
    double coord_u = acos(x_formule / radius) / (2 * PI);

    if (y_formule < 0) {
        coord_u = 1 - coord_u;
    }

    hit->uv = {1 - coord_u, 1 - coord_v};

    return intersecte;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le cylindre.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire (comme ici).
AABB Cylinder::compute_aabb() {
    // Les plan en XY et ZY du cylindre 3D dans l'espace local sont des rectangles centré à l'origine de (radius x half_height) et le plan XZ lui est un cercle de rayon radius centré à l'origine.

    // Création des points extrême de chacun des 3 plans dans l'espace local.
    std::vector<double3> points_local;
    // plan XY
    points_local.push_back(double3 {-radius, -half_height, 0});
    points_local.push_back(double3 {radius, -half_height, 0});
    points_local.push_back(double3 {radius, half_height, 0});
    points_local.push_back(double3 {-radius, half_height, 0});
    // plan YZ
    points_local.push_back(double3 {0, -half_height, -radius});
    points_local.push_back(double3 {0, -half_height, radius});
    points_local.push_back(double3 {0, half_height, radius});
    points_local.push_back(double3 {0, half_height, -radius});
    // plan XZ
    points_local.push_back(double3 {0, 0, -radius});
    points_local.push_back(double3 {radius, 0, 0});
    points_local.push_back(double3 {0, 0, radius});
    points_local.push_back(double3 {-radius, 0, 0});

    // Création du AABB dans l'espace local
    AABB aabb_local = construct_aabb(points_local);
    std::vector<double3> coins_local = retrieve_corners(aabb_local);

    // Transformation des coins du AABB local dans l'espace global
    std::vector<double3> points_global;

    for (double3 c_local : coins_local) {
        double4 c_local_homogene = {c_local.x, c_local.y, c_local.z, 1};
        double4 p_global_homogene = mul(transform, c_local_homogene);
        double3 p_global = {p_global_homogene.x, p_global_homogene.y, p_global_homogene.z};
        points_global.push_back(p_global);
    }

    // Création du AABB dans l'espace global    
    AABB aabb_global = construct_aabb(points_global);

	return aabb_global;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un mesh.
//
// Référez-vous au PDF pour la paramétrisation pour les coordonnées UV.
//
// Pour plus de d'informations sur la géométrie, référez-vous à la classe object.h.
//
bool Mesh::local_intersect(Ray ray,
                           double t_min, double t_max,
                           Intersection* hit) {
    
    bool intersecte = false;
    double t_proche = t_max;

    for (size_t i=0; i<triangles.size(); i++){
        const Triangle& tri = triangles[i];
        Intersection temp_hit;

        if (intersect_triangle(ray, t_min, t_proche, tri, &temp_hit)){
            if (temp_hit.depth < t_proche){
                t_proche = temp_hit.depth;
                *hit = temp_hit;
                intersecte = true;
            }
        }
    }

    return intersecte;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de trouver l'intersection avec un triangle.
// S'il y a intersection, remplissez hit avec l'information sur la normale et les coordonnées texture.
bool Mesh::intersect_triangle(Ray  ray,
                              double t_min, double t_max,
                              Triangle const tri,
                              Intersection *hit)
{
    // Triangle en question. Respectez la convention suivante pour vos variables.
    //
    //     A
    //    / \
    //   /   \
    //  B --> C
    //
    // Respectez la règle de la main droite pour la normale.

    // @@@@@@ VOTRE CODE ICI
    // Décidez si le rayon intersecte le triangle (p0,p1,p2).
    // Si c'est le cas, remplissez la structure hit avec les informations
    // de l'intersection et renvoyez true.
    // Pour plus de d'informations sur la géométrie, référez-vous à la classe dans object.hpp.
    //
    // NOTE : hit.depth est la profondeur de l'intersection actuellement la plus proche,
    // donc n'acceptez pas les intersections qui occurent plus loin que cette valeur.
    
    // Extrait chaque position de sommet des données du maillage.
    double3 const &p0 = positions[tri[0].pi]; // ou Sommet A (Pour faciliter les explications)
    double3 const &p1 = positions[tri[1].pi]; // ou Sommet B
    double3 const &p2 = positions[tri[2].pi]; // ou Sommet C

    //normal d'un triangle
    double3 edge1 = p1 - p0;
    double3 edge2 = p2 - p0;
    double3 e1_x_e2 = cross(edge1, edge2);
    double3 normal = normalize(e1_x_e2);

    //ray parallele au triangle?
    double denom = dot(normal, ray.direction);
    if (fabs(denom) < EPSILON) return false;//pas d'intersection

    //intersection rayon et triangle
    double t = dot((p0 - ray.origin), normal) / denom;

    if (t < t_min || t > t_max) return false;

    //point d'intersection
    double3 pt_intersect = ray.origin + t * ray.direction;

    //trouver position du point intersection
    double3 edge3 = p2 - p1;
    double3 edge4 = p0 - p2;

    double3 c0 = pt_intersect - p0;
    double3 c1 = pt_intersect - p1;
    double3 c2 = pt_intersect - p2;

    double3 e1_X_c0 = cross(edge1, c0);
    double3 e3_X_c1 = cross(edge3, c1);
    double3 e4_X_c2 = cross(edge4, c2);

    if (dot(normal, e1_X_c0) < 0) return false;
    if (dot(normal, e3_X_c1) < 0) return false;
    if (dot(normal, e4_X_c2) < 0) return false;

    //update information du hit
    hit->position = pt_intersect;
    hit->depth = t;

    //coordonnées barycentric avec l'aire pour l'interpolation des normals et de coordonnées uv
    double aire_total = length(e1_x_e2)/2;
    double aire_p1p2pt = length(e3_X_c1) /2 /aire_total;
    double aire_p2p0pt = length(e4_X_c2) /2 /aire_total;
    double aire_p0p1pt = aire_total - aire_p1p2pt - aire_p2p0pt;

    // normal a la position du point d'intersection avec le triangle
    double3 const &n0 = normals[tri[0].ni]; // ou Sommet A (Pour faciliter les explications)
    double3 const &n1 = normals[tri[1].ni]; // ou Sommet B
    double3 const &n2 = normals[tri[2].ni]; // ou Sommet C
    double3 n = (n0*aire_p1p2pt) + (n1*aire_p2p0pt) + (n2*aire_p0p1pt);
    n = normalize(n);

    // la direction du rayon et la normal du plan doivent être en sens inverse
    if (dot(n, ray.direction) > 0) {
        hit->normal = -n;
    } else {
        hit->normal = n;
    }
    
    // paramétrisation uv à la position du point d'intersection
    double2 const &uv0 = tex_coords[tri[0].ti]; // ou Sommet A (Pour faciliter les explications)
    double2 const &uv1 = tex_coords[tri[1].ti]; // ou Sommet B
    double2 const &uv2 = tex_coords[tri[2].ti]; // ou Sommet C
    double2 uv = (uv0*aire_p1p2pt) + (uv1*aire_p2p0pt) + (uv2*aire_p0p1pt);
    
    //hit->uv = {uv.x/uv.x, uv.y/uv.y};
    hit->uv ={aire_p1p2pt, aire_p2p0pt};

    return true;
}

// @@@@@@ VOTRE CODE ICI
// Occupez-vous de compléter cette fonction afin de calculer le AABB pour le Mesh.
// Il faut que le AABB englobe minimalement notre objet à moins que l'énoncé prononce le contraire.
AABB Mesh::compute_aabb() {
    // Transformation en espace global de tous les sommets qui composent le Mesh
    std::vector<double3> position_global;

    for (double3 p : positions) {
        double4 p_global = mul(transform, {p.x, p.y, p.z, 1});
        position_global.push_back({p_global.x, p_global.y, p_global.z});
    }

    // Création du AABB dans l'espace global
    AABB aabb_global = construct_aabb(position_global);

	return aabb_global;
}
