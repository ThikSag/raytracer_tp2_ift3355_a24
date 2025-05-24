#include "raytracer.h"

void Raytracer::render(const Scene& scene, Frame* output)
{       
    // Crée le z_buffer.
    double *z_buffer = new double[scene.resolution[0] * scene.resolution[1]];
    for(int i = 0; i < scene.resolution[0] * scene.resolution[1]; i++) {
        z_buffer[i] = scene.camera.z_far; //Anciennement DBL_MAX. À remplacer avec la valeur de scene.camera.z_far
    }

	//---------------------------------------------------------------------------------------------------------------
	// Nous vous fournissons ci-bas du code pour une caméra orthographique très simple. Cette caméra peut être utilisée pour tester l’intersection avec la sphère.
	// Vous devez utiliser la scène de test portho.ray pour utiliser cette caméra. 
	// Notez que votre code de caméra ne doit pas être basé sur ce code de caméra. Ce code n’est là que pour prendre en compte le développement initial du test d’intersection.
	// Pour utiliser cette caméra, vous devez supprimer les commentaires qui rendent inactive cette partie du code, et mettre en commentaires la boucle d’image originale.

	/*CameraOrthographic camOrth;
	double3 uVec{ 0,1,0 };
	double3 vVec{ 0,0,1 };
	double y_shift = 2.0 / scene.resolution[1];
	double x_shift = 2.0 / scene.resolution[0];

	for (int y = 0; y < scene.resolution[1]; y++) {
		if (y % 40) {
			std::cout << "\rScanlines completed: " << y << "/" << scene.resolution[1] << '\r';
		}

		for (int x = 0; x < scene.resolution[0]; x++) {
			double3 color{ 0,0,0 };

			Intersection hit;
			double3 rayOrigin = camOrth.minPosition + uVec * x_shift * x + vVec * y_shift * y;
			double3 rayDirection{ 1,0,0 };
			Ray ray = Ray(rayOrigin, rayDirection);
			double itHits = 0;

			double z_depth = scene.camera.z_far;
			if (scene.container->intersect(ray, EPSILON, z_depth, &hit)) {
				Material& material = ResourceManager::Instance()->materials[hit.key_material];
				color = material.color_albedo;
				itHits = 1.0f;
			}

			output->set_color_pixel(x, y, color);
			output->set_depth_pixel(x, y, itHits);
		}
	}*/

	//---------------------------------------------------------------------------------------------------------------

	// @@@@@@ VOTRE CODE ICI
	// Calculez les paramètres de la caméra pour les rayons.

	    //Paramètres de la caméra pour la profondeur de champs
        double focus_distance = scene.camera.focus_distance;
        double defocus_angle = scene.camera.defocus_angle ;

	// Paramètres de la caméra dans l'espace monde
	double3 posCam = scene.camera.position;
	double3 dirVue = normalize(scene.camera.center - scene.camera.position); // direction de vue de la caméra

	// Système de coordonnées orthonormé uvw de la caméra (espace Caméra)
	double3 w = normalize(-dirVue);
	double3 u = normalize(cross(scene.camera.up, w));
	double3 v = cross(w, u);

	// Matrices nécessaires au passage du système de coordonnées xyz (espace Monde) à uvw (espace Caméra)
	double4x4 mAligneUVW = {{u[0], v[0], w[0], 0}, {u[1], v[1], w[1], 0}, {u[2], v[2], w[2], 0}, {0,0,0,1}}; 
	double4x4 mTranslatOrigin = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {-posCam[0], -posCam[1], -posCam[2], 1}};
	double4x4 mMondeACamera = mul(mAligneUVW, mTranslatOrigin);
	double4x4 mCameraAMonde = inverse(mMondeACamera);

	// Position de la caméra en coordonnées homogènes dans l'espace Monde et l'espace Caméra
	double4 pCam_Monde = {posCam[0], posCam[1], posCam[2], 1};
	double4 pCam_Camera = mul(mMondeACamera, {posCam[0], posCam[1], posCam[2], 1});

	// Paramètre du plan-image dans l'espace Caméra
	double fovyRadian = deg2rad(scene.camera.fovy);
	double top_image = tan(fovyRadian/2) * abs(scene.camera.z_near); // demi-hauteur de l'image
	double right_image = scene.camera.aspect * top_image; // demi-largeur de l'image
	double taillePixel = 2 * right_image / scene.resolution[0];
	double3 image_Camera = {pCam_Camera[0] - right_image, 
							pCam_Camera[1] - top_image,
							pCam_Camera[2] - scene.camera.z_near}; // origine du plan-image au coin inférieur gauche

    // Itère sur tous les pixels de l'image.
    for(int y = 0; y < scene.resolution[1]; y++) {
		if (y % 40){
			std::cout << "\rScanlines completed: " << y << "/" << scene.resolution[1] << '\r';
		}

        for(int x = 0; x < scene.resolution[0]; x++) {

			int avg_z_depth = 0;
			double3 avg_ray_color{0,0,0};
			
			for(int iray = 0; iray < scene.samples_per_pixel; iray++) {
				// Génère le rayon approprié pour ce pixel.
				Ray ray;
				// Initialise la profondeur de récursivité du rayon.
				int ray_depth = 0;
				// Initialize la couleur du rayon
				double3 ray_color{0,0,0};

				// @@@@@@ VOTRE CODE ICI
				// Mettez en place le rayon primaire en utilisant les paramètres de la caméra.
				// Lancez le rayon de manière uniformément aléatoire à l'intérieur du pixel dans la zone délimité par jitter_radius. 
				//Faites la moyenne des différentes couleurs obtenues suite à la récursion.
				
				// l'origine du rayon primaire est la position de l'oeil de la caméra dans l'espace Monde
				/*ray.origin[0] = scene.camera.position[0];
				ray.origin[1] = scene.camera.position[1];
				ray.origin[2] = scene.camera.position[2];*/

				// point aléatoire à l'intérieur de la zone/cercle de sampling qui est délimitée par le jitter_radius
				double2 pAleatoire = random_in_unit_disk();
				pAleatoire = pAleatoire * taillePixel * scene.jitter_radius;
				
				    //Pertubation de la position origine de la camera (profondeur de champs)
                    double3 offset{0,0,0};
                    if (defocus_angle > 0){
                        double2 disk_rand = random_in_unit_disk();
                        double cercle_defocus = focus_distance * tan(defocus_angle/2);
                        disk_rand = disk_rand * taillePixel * scene.jitter_radius;
                        offset = disk_rand.x * cercle_defocus * u + disk_rand.y * cercle_defocus * v;
                    }
                    ray.origin = scene.camera.position + offset;

                    //Calculer la direction du nouveau point focus
                    double3 new_focus = scene.camera.position + focus_distance * dirVue;
                    ray.direction = normalize(new_focus - ray.origin);
				
				// position aléatoire dans le pixel actuel en coordonnée homogène
				double4 pixel_Camera;
				pixel_Camera[0] = image_Camera[0] + (x * taillePixel) + (taillePixel / 2) + pAleatoire[0];
				pixel_Camera[1] = image_Camera[1] + (y * taillePixel) + (taillePixel / 2) + pAleatoire[1];
				pixel_Camera[2] = image_Camera[2];
				pixel_Camera[3] = 1;
				double4 pixel_Monde = mul(mCameraAMonde, pixel_Camera);
				
				// direction normalisé du rayon primaire dans l'espace Monde
				double3 dirRayon = {pixel_Monde[0], pixel_Monde[1], pixel_Monde[2]};
				dirRayon = dirRayon - ray.origin;
				dirRayon = normalize(dirRayon);
				ray.direction[0] = dirRayon[0];
				ray.direction[1] = dirRayon[1];
				ray.direction[2] = dirRayon[2];

				// initialise la profondeur en z du rayon
				double ray_z_depth = scene.camera.z_far;

				// lancer du rayon
				trace(scene, ray, ray_depth, &ray_color, &ray_z_depth);

				// ajout de la couleur et de la profondeur du rayon à leur moyenne respective
				avg_ray_color += ray_color;
				avg_z_depth += ray_z_depth;
			}

			avg_z_depth = avg_z_depth / scene.samples_per_pixel;
			avg_ray_color = avg_ray_color / scene.samples_per_pixel;

			// Test de profondeur
			if(avg_z_depth >= scene.camera.z_near && avg_z_depth <= scene.camera.z_far && 
				avg_z_depth < z_buffer[x + y*scene.resolution[0]]) {
				z_buffer[x + y*scene.resolution[0]] = avg_z_depth;

				// Met à jour la couleur de l'image (et sa profondeur)
				output->set_color_pixel(x, y, avg_ray_color);
				output->set_depth_pixel(x, y, (avg_z_depth - scene.camera.z_near) / 
										(scene.camera.z_far-scene.camera.z_near));
			}
        }
    }

    delete[] z_buffer;
}

// @@@@@@ VOTRE CODE ICI
// Veuillez remplir les objectifs suivants:
// 		- Détermine si le rayon intersecte la géométrie.
//      	- Calculer la contribution associée à la réflexion.
//			- Calculer la contribution associée à la réfraction.
//			- Mettre à jour la couleur avec le shading + 
//			  Ajouter réflexion selon material.reflection +
//			  Ajouter réfraction selon material.refraction 
//            pour la couleur de sortie.
//          - Mettre à jour la nouvelle profondeure.
void Raytracer::trace(const Scene& scene,
					  Ray ray, int ray_depth,
					  double3* out_color, double* out_z_depth)
{
	Intersection hit;
	// Fait appel à l'un des containers spécifiées.
	if(scene.container->intersect(ray,EPSILON,*out_z_depth,&hit)) {		
		Material& material = ResourceManager::Instance()->materials[hit.key_material];

		// @@@@@@ VOTRE CODE ICI
		// Déterminer la couleur associée à la réflection d'un rayon de manière récursive.
		bool a_reflect = false;

		if (ray_depth < scene.max_ray_depth && material.k_reflection > 0) { 
			// On calcule la réflexion jusqu'à atteindre la profondeur maximale de récursivité
			// si et seulement si le matériel est réfléchissant.
			a_reflect = true;
		}

		// Couleur de la réflexion
		double3 reflex_color = {0,0,0};

		if (a_reflect) {
			// Définition du rayon de réflexion
			double3 v_dir = -ray.direction;
			double3 reflex_dir = 2 * dot(v_dir, hit.normal) * hit.normal - v_dir;

			Ray ray_reflex;
			ray_reflex.direction = normalize(reflex_dir);
			ray_reflex.origin = hit.position + (ray_reflex.direction * EPSILON);

			double reflex_out_z_depth = DBL_MAX;

			// Lancé du rayon de réflexion
			trace(scene, ray_reflex, ray_depth+1, &reflex_color, &reflex_out_z_depth);
		}
		
		// @@@@@@ VOTRE CODE ICI
		// Déterminer la couleur associée à la réfraction d'un rayon de manière récursive.
		// 
		// Assumez que l'extérieur/l'air a un indice de réfraction de 1.
		//
		// Toutes les géométries sont des surfaces et non pas de volumes.
		bool a_refract = false;

		if (ray_depth < scene.max_ray_depth && material.k_refraction > 0) { 
			// On calcule la réfraction jusqu'à atteindre la profondeur maximale de récursivité
			// si et seulement si le matériel est réfractif.
			a_refract = true;
		}

		// Couleur de la réfraction
		double3 refrac_color = {0,0,0};

		if (a_refract) {
			// Définition du rayon de réfraction selon les lois de Snell-Descartes
			double3 v_dir = -ray.direction;
			double heta = 1 / material.refractive_index;
			double dot_NV = dot(hit.normal, v_dir);

			double3 refrac_dir = (hit.normal * ((heta * dot_NV) - sqrt(1 - (heta * heta * (1 - (dot_NV * dot_NV)))))) - (heta * v_dir);

			Ray ray_refrac;
			ray_refrac.direction = normalize(refrac_dir);
			ray_refrac.origin = hit.position + (ray_refrac.direction * EPSILON);

			double refrac_out_z_depth = DBL_MAX;

			// Lancé du rayon de réfraction
			trace(scene, ray_refrac, ray_depth+1, &refrac_color, &refrac_out_z_depth);
		}


		// Calcul de la couleur finale
		double3 color_shade = shade(scene, hit);

		if (a_reflect) {
			color_shade = color_shade + (material.k_reflection * reflex_color);
		}

		if (a_refract) {
			color_shade = color_shade + (material.k_refraction * refrac_color);
		}

		*out_color = color_shade;
		*out_z_depth = hit.depth;
	}
}

// @@@@@@ VOTRE CODE ICI
// Veuillez remplir les objectifs suivants:
// 		* Calculer la contribution des lumières dans la scène.
//			- Itérer sur toutes les lumières.
//				- Inclure la contribution spéculaire selon le modèle de Blinn en incluant la composante métallique.
//	          	- Inclure la contribution diffuse. (Faites attention au produit scalare. >= 0)
//   	  	- Inclure la contribution ambiante
//      * Calculer si le point est dans l'ombre
//			- Itérer sur tous les objets et détecter si le rayon entre l'intersection et la lumière est occludé.
//				- Ne pas considérer les points plus loins que la lumière.
//			- Par la suite, intégrer la pénombre dans votre calcul
//		* Déterminer la couleur du point d'intersection.
//        	- Si texture est présente, prende la couleur à la coordonnées uv
//			- Si aucune texture, prendre la couleur associé au matériel.

double3 Raytracer::shade(const Scene& scene, Intersection hit) {
    Material &material = ResourceManager::Instance()->materials[hit.key_material];

	// Choix de la couleur selon si une texture est présente ou non dans le matériel.
	double3 color;

	if (! material.texture_albedo.operator!()) {
		// Une texture est présente
		unsigned int u = hit.uv.x * material.texture_albedo.width();
		unsigned int v = hit.uv.y * material.texture_albedo.height();
		unsigned char red = material.texture_albedo.red_channel(u, v);
		unsigned char green = material.texture_albedo.green_channel(u, v);
		unsigned char blue = material.texture_albedo.blue_channel(u, v);
		color.x = double(red) / 255 ;
		color.y = double(green) /255 ;
		color.z = double(blue) / 255 ;

	} else {
		// Aucune texture, on prend la couleur du matériel
		color = material.color_albedo;
	}

    // Calcule de l'éclairage ambiant
    double3 ambiante = scene.ambient_light * color * material.k_ambient;

    //Initialisation éclairage diffus et spéculaire
    double3 diffus = {0,0,0};
    double3 speculaire = {0,0,0};

    double3 d_camvue = normalize(scene.camera.position - hit.position);

    //Loop pour chaque source de lumiere
    for (int i=0; i<scene.lights.size(); ++i) {
        const SphericalLight light = scene.lights[i];

        double3 d_light = normalize(light.position - hit.position);
        double r = length(light.position - hit.position);
        double reality = 1.0 / (r * r);

		double contribution = 1;

        //Vérifier si la lumière est sphérique
        if (light.radius > 0) {

            //Calculation de la pénombre lorsque lumière est sphérique
            int ray_sample = 5;
            int ray_hit_light = 0;

            for (int j = 0; j < ray_sample; ++j) {
                double2 disk_sample = random_in_unit_disk();
                double3 offset = disk_sample.x * d_light + disk_sample.y * normalize(cross(d_light, hit.normal));
                double3 d_sample = normalize(d_light + light.radius * offset / r);

                //Créer des rayons pour la pénombre
                Ray ray_penombre;
                ray_penombre.origin = hit.position + hit.normal * EPSILON;
                ray_penombre.direction = d_sample;

                //Vérifie si le rayon intersecte un objet
                Intersection penombre_hit;
                bool penombre = scene.container->intersect(ray_penombre, EPSILON, r, &penombre_hit);
                if (!penombre){
                    ray_hit_light++;
                }
            }

            //Calculer de facteur d'occlusion
            contribution = double(ray_hit_light) / ray_sample;

        } else {
            //Si la lumière n'est pas seulement sphérique (création d'ombre)
            //Créer des rayons d'ombre
            Ray ray_ombre;
            ray_ombre.origin = hit.position + hit.normal * EPSILON;
            ray_ombre.direction = d_light;

            //Vérifier si le rayon d'ombre intersecte un objet
            Intersection ombre_hit;
            bool ombre = scene.container->intersect(ray_ombre, EPSILON, r ,&ombre_hit);

            if (ombre){
				contribution = 0;

            } else {
				contribution = 1;
			}
        }

		if (contribution != 0) {
			//Calcul de l'intensité diffus avec la pénombre (inclut occlusion)
	        double intense_diffuse = dot(hit.normal, d_light);

	        if (intense_diffuse > 0){
	            diffus += contribution * reality * intense_diffuse * material.k_diffuse * color * light.emission;
	        }

	        //Calcul de l'intensité spéculaire de Blinn avec pénombre (inclut occlusion)
	        double3 d_demi = normalize(d_light + d_camvue);
	        double intense_spec = dot(hit.normal, d_demi);

	        if (intense_spec > 0) {
	            intense_spec = pow(intense_spec, material.shininess);
	            speculaire += contribution * reality * intense_spec * material.k_specular * light.emission *
	                          (material.metallic * color + (1 - material.metallic));
	        }
		}
    }

    //Calcule de l'éclairage total
    double3 eclairage = ambiante + diffus + speculaire;
    return eclairage;
}
