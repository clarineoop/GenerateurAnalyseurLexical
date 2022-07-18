## Etapes pour exécuter le projet

* Installer graphviz: sudo apt-get install graphviz
* Se positioner dans le dossier src (cd)
* exécuter la comande suivante: make
* un fichier exécutale gal sera créé dans le dossier src
* exécuter en utilisant la comande: ./gal <nom_du_fichier_contenant_lexpression_rationelle>
* les différentes représentations graphiques seront créées dans le dossier graphs.
* le code c de l'analiseur lexical sera également généré dans le repertoire src: gal.c
* compiler l'analyseur lexical: gcc -g -Wall -o al al.c
* exécuter l'analyseur lexical en lui passant une chaine de caractère à analyser: ./al <chaine_à_analyser>