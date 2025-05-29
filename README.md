# Lancer de rayons - TP 2 - IFT3355 Infographie - Automne 2024
raytracer_tp2_ift3355_a24

<br>
⚠️Je travaille présentement sur l’écriture du README de ce projet. Certaines sections 
ne sont donc pas encore rédigées.⚠️

## Introduction
Ce projet a été réalisé dans le cadre du **_Travail pratique 2_** du cours **_IFT3355 
– Infographie_** de l’**_Université de Montréal_** donné par Pierre Poulin à la session 
d’automne 2024. 

Merci à ma coéquipière [Grace](https://github.com/gracenl2) avec qui j’ai fait ce projet.

De plus, le projet a été réalisé dans le cadre d’un cours donné en français. Ainsi, les 
commentaires dans les fichiers de code source du projet (les fichiers du dossier 
[_src_](src)) sont presque tous en **français**. 

## Description sommaire du projet
Dans ce projet, il fallait implémenter un algorithme « simple » de lancer de rayons qui 
calcule l’éclairage local, les ombres, la réflexion et la réfraction à l’aide d’une 
structure accélératrice. Pour réaliser ce projet, un squelette de code nous a été remis 
par notre professeur et nous devions le compléter afin d’avoir un algorithme de lancer de 
rayon complet et fonctionnel.

L’algorithme de lancer de rayon reçoit la définition d’une scène qu’il doit rendre en 
deux partie. La première partie est le _shading_ de la scène qui sert à produire une image 
en couleur de la scène. La deuxième partie est le _buffer_ de profondeur de la scène qui 
sert à produire une image en noir et blanc de la profondeur des éléments dans la scène. 
L’algorithme lance des rayons primaires dans la scène qui génèrent d’autres rayons secondaires, 
comme des rayons d’ombre. Le lancer de rayon peut avoir un ou plusieurs rayons par pixel 
selon la définition de la scène reçue. 

Malheureusement, dans la version actuelle du code, l’algorithme de lancer de rayon n’est 
pas complet. Il y a deux éléments du lancer de rayon qui sont problématiques, le mapping UV 
de texture du _mesh_ et la profondeur de champ. Il y a des erreurs dans l’implémentation de 
ces deux éléments ce qui fait qu’ils ne produisent pas les résultats attendus lors du rendement 
de 3 scènes du projet. Le problème avec le mapping UV de texture du _mesh_ affecte le rendu 
du _mesh_ (en forme de cube) dans les scènes [_tcage.ray_](data/scene/tcage.ray) et 
[_twindows.ray_](data/scene/twindows.ray), alors que le problème avec la profondeur de champ 
affecte le rendu de la scène [pdof.ray](data/scene/pdof.ray) au complet.

L’objectif principal de ce projet était de mettre en pratique diverses notions d’infographie 
vue dans le cours, telles que les projections, le lancer de rayon, le tampon de profondeur 
(_z-buffer_), les volumes englobants, les ombres, les lumières et le mapping de texture.

Ce projet utilise [_CMake_](https://cmake.org) pour compiler le projet.

Ce projet utilise la libraire [_linalg.h_](https://github.com/sgorsten/linalg) qui fournie 
des outils mathématiques de base tels que les vecteurs et les matrices. Cette librairie est 
très utile puisqu’elle implémente plusieurs fonctions qui sont très pratiques en rendu.

Une description plus complète du projet est disponible dans la section 
[_Description détaillée du projet_](https://github.com/ThikSag/raytracer_tp2_ift3355_a24?tab=readme-ov-file#description-d%C3%A9taill%C3%A9e-du-projet).

De plus, une section concernant les [améliorations possibles](https://github.com/ThikSag/raytracer_tp2_ift3355_a24?tab=readme-ov-file#am%C3%A9lioration-possible-du-projet) 
du projet est incluse après la description détaillée.

## Explications des différentes branches du répertoire
- La branche [_main_](https://github.com/ThikSag/raytracer_tp2_ift3355_a24/tree/main) 
  contient la version du code que ma coéquipière et moi avons remis pour l’évaluation de 
  ce travail scolaire.
<br><br>
- La branche [_original_files_](https://github.com/ThikSag/raytracer_tp2_ift3355_a24/tree/original_files) 
  contient les fichiers **originaux** du squelette de code qui nous a été remis par notre 
  professeur pour réaliser ce travail.

## Installation, configuration et lancement du projet
### Installation
Pour ce projet, nous avons utilisé l’éditeur de code 
[_Visual Studio Code_](https://code.visualstudio.com) (_VS Code_). Il est donc recommandé de 
l’installer afin de pouvoir configurer et lancer le projet simplement en suivant les instructions 
ci-dessous. Vous pouvez télécharger _VS Code_ selon votre système d’exploitation sur le site web 
suivant : https://code.visualstudio.com/Download. 

Ce projet a été fait en C++ et il utilise _CMake_. Il est donc nécessaire d’avoir un compilateur 
C++ et _CMake_ d’installer afin de pouvoir compiler le projet. Si vous êtes sur __Windows__, vous 
pouvez télécharger un compilateur C++ avec _WSL_ ou [_MinGW_](https://code.visualstudio.com/docs/cpp/config-mingw) 
et vous pouvez télécharger _CMake_ sur le site web suivant : https://cmake.org/download/. Si vous 
être sur __Linux__, vous pouvez utiliser votre _package manager_ pour installer `gdb` et `cmake`.

### Configuration
1. Lancer _VS Code_, puis installer l’extension _C/C++ Extension Pack_.
<br><br>
2. Dans _VS Code_, cliquer sur `Open Folder…` et ouvrir le dossier contenant le projet CMake. Il est 
   important d’ouvrir le dossier qui contient le fichier [_CMakeLists.txt_](CMakeLists.txt) et les 
   dossiers [src](src), [extern](extern), [data](data).
<br><br>
3. __Si__ une liste de configuration s’ouvre en haut de la fenêtre de _VS Code_, choisir le 
   compilateur C++ installé précédemment dans la liste proposée (ex. : GCC).

   __Sinon__, dans _VS Code_, sélectionner l’extension _CMake_ dans la barre d’activité grise sur 
   la gauche de la fenêtre. Puis, cliquer sur le bouton `✏️ Select a Kit` et choisir le 
   compilateur C++ installé précédemment dans la liste proposée (ex. : GCC).
<br><br>
4. Dans _VS Code_, ...

## Explication des fichiers, des dossiers et du fonctionnement du programme.

## Description détaillée du projet
Tel que mentionné précédemment, ce projet a été réalisé dans le cadre d’un travail pratique scolaire. 
Dans cette section, je vais donc décrire les [tâches à accomplir](https://github.com/ThikSag/raytracer_tp2_ift3355_a24#les-t%C3%A2ches-%C3%A0-accomplir) 
pour compléter ce travail scolaire. J’apporterai ensuite quelques [précisions](https://github.com/ThikSag/raytracer_tp2_ift3355_a24#pr%C3%A9cisions-suppl%C3%A9mentaires-sur-certaines-parties-du-travail) 
concernant certaines parties du travail. 

Pour ce travail, un squelette de code nous a été fourni et il nous fallait le compléter. Il est 
possible de consulter le squelette de code tel qu’il nous a été remis dans la branche [_original_files_](https://github.com/ThikSag/raytracer_tp2_ift3355_a24/tree/original_files). 
Vous pouvez consulter la section [_Explication des fichiers, des dossiers et du fonctionnement du programme_](https://github.com/ThikSag/raytracer_tp2_ift3355_a24#explication-des-fichiers-des-dossiers-et-du-fonctionnement-du-programme) 
pour avoir une brève description des différents éléments fournis dans le squelette de code.

### Les tâches à accomplir
1. 


2. 


3. 


4. 


5. 


6. 

### Précisions supplémentaires sur certaines parties du travail

- 

## Amélioration possible du projet
- Corriger le mapping UV de texture pour le _mesh_.
<br><br>
- Corriger la profondeur de champ.
<br><br>
- 

**Merci encore à ma coéquipière [Grace](https://github.com/gracenl2) avec qui j’ai réalisé ce projet.**

[Retour en haut ⤴️](https://github.com/ThikSag/raytracer_tp2_ift3355_a24#lancer-de-rayons---tp-2---ift3355-infographie---automne-2024)

<!-- 
-->
