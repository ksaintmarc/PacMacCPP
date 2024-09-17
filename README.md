# Projet de Programmation Avancée - PACMAN

## Sujet
### Consignes 

Faire un remake du jeu « PAC-MAN » de Namco (1980) qui sera le plus fidèle possible à la version
originale. La programmation se fera en C++ moderne et l’affichage avec la librairie SDL2. Un code
d’exemple C permettant de tracer des lignes sera fourni.
L’affichage se fera uniquement par copie de sprite à partir d’une ou plusieurs planches (images en
format BMP), comme dans l’exemple fourni. Penser à gérer correctement la vitesse du jeu. La librairie
SDL2 est la seule autorisée. Le code fourni compile et fonctionne sur tous les systèmes. Votre
démonstration devra compiler et fonctionner sur les machines des salle de TP.
Vous trouverez les détails et règles du jeu sur https://strategywiki.org/wiki/Pac-Man. Pour trouver des
vidéos sur youtube, cherchez « pacman namco 1980 ».
L’évaluation prendra en compte le contenu, mais surtout la modernité (pas de code à la C), la
propreté, l’organisation la lisibilité et l’élégance du code.

### Critères
Vous respecterez les critères suivants :
- Utilisez les possibilités du C++11/14/17/20 (lambda, for range, …)
- Ecrivez des classes pour vos objets et encapsulation de la SDL2
- Utilisation de l’héritage et du polymorphisme.
- Utilisation de const et des références autant que possible.
- Pas de tableau ni statique ni dynamique, utilisez les containers de la STL
- Utilisation d’algorithme de la STL
- Pas de pointeurs utilisez unique, shared et weak_ptr
- Pour les nombre aléatoires pas de rand()/srand() utiliser std::uniform_real_distribution et
std::uniform_int_distribution
- Documentez votre code
- Utilisez une convention de codage
