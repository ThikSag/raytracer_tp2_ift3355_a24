#include "container.h"

// @@@@@@ VOTRE CODE ICI
// - Parcourir l'arbre DEPTH FIRST SEARCH selon les conditions suivantes:
// 		- S'il s'agit d'une feuille, faites l'intersection avec la géométrie.
//		- Sinon, il s'agit d'un noeud altérieur.
//			- Faites l'intersection du rayon avec le AABB gauche et droite. 
//				- S'il y a intersection, ajouter le noeud à ceux à visiter. 
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool BVH::intersect(Ray ray, double t_min, double t_max, Intersection* hit) {
	bool it_hit = false;
	std::vector<BVHNode*> a_visiter({root});

	while (a_visiter.size()>0) {

		// Récupère le prochain noeud à visiter dans l'arbre
		BVHNode* node = a_visiter.back();
		a_visiter.pop_back();

		if (node->left == nullptr && node->right == nullptr) {
			// C'est une feuille

			Intersection* obj_hit = new Intersection();

			if (objects[node->idx]->intersect(ray, t_min, t_max, obj_hit)) { 
				// Intersection entre le rayon et la géométrie détectée.
				it_hit = true;

				if (obj_hit->depth < hit->depth) { 
					// On garde l'intersection qui a la profondeur la plus petite.

					hit->depth = obj_hit->depth;
					hit->key_material = obj_hit->key_material;
					hit->normal = obj_hit->normal;
					hit->position = obj_hit->position;
					hit->uv = obj_hit->uv;
				}
			}

			delete obj_hit;

		} else {
			// C'est un noeud intérieur

			if (node->right->aabb.intersect(ray, t_min, t_max)) {
				// Intersection entre le rayon et le AABB de droite.
				a_visiter.push_back(node->right);
			}

			if (node->left->aabb.intersect(ray, t_min, t_max)) {
				// Intersection entre le rayon et le AABB de gauche.
				a_visiter.push_back(node->left);
			}
		}
	}

	return it_hit;
}

// @@@@@@ VOTRE CODE ICI
// - Parcourir tous les objets
// 		- Détecter l'intersection avec l'AABB
//			- Si intersection, détecter l'intersection avec la géométrie.
//				- Si intersection, mettre à jour les paramètres.
// - Retourner l'intersection avec la profondeur maximale la plus PETITE.
bool Naive::intersect(Ray ray, double t_min, double t_max, Intersection* hit) {
	bool it_hit = false; // Indique si une intersection avec un objet a été détectée.

	for (int iobj = 0; iobj < objects.size(); iobj++) {

		if (aabbs[iobj].intersect(ray, t_min, t_max)) {
			// Intersection entre le rayon et le aabb de l'objet détectée.

			Intersection* obj_hit = new Intersection();

			if (objects[iobj]->intersect(ray, t_min, t_max, obj_hit)) { 
				// Intersection entre le rayon et la géométrie détectée.
				it_hit = true;

				if (obj_hit->depth < hit->depth) { 
					// On garde l'intersection qui a la profondeur la plus petite.

					hit->depth = obj_hit->depth;
					hit->key_material = obj_hit->key_material;
					hit->normal = obj_hit->normal;
					hit->position = obj_hit->position;
					hit->uv = obj_hit->uv;
				}
			}

			delete obj_hit;
		}
	}
	
	return it_hit;
}
