Etapes pour exécuter le projet

1 - Installer graphviz: sudo apt-get install graphviz
1 - Se positioner dans le dossier src (cd)
2 - exécuter la comande suivante: make
3 - un fichier exécutale gal sera créé dans le dossier src
3 - exécuter en utilisant la comande: ./gal <nom_du_fichier_contenant_lexpression_rationelle>
4 - les différentes représentations graphiques seront créées dans le dossier graphs.
5 - le code c de l'analiseur lexical sera également généré dans le repertoire src: gal.c
6 - compiler l'analyseur lexical: gcc -g -Wall -o al al.c
7 - exécuter l'analyseur lexical en lui passant une chaine de caractère à analyser: ./al <chaine_à_analyser>